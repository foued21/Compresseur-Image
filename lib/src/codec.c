#include "../include/bitstream.h"
#include "../include/segmentation.h"
#include <stdio.h>   
#include <stdbool.h>
#include <string.h> 
#include <stdlib.h>
#include <math.h>   
#include <time.h>
#include <libgen.h>


// fonctions d'encodage
static void encoder_entete(BitStream* stream);
static void encoder_taux_compression(BitStream* stream, double compression_rate);
static void encoder_profondeur(BitStream* stream, QuadTree* tree);
static void encoder_quadtree(BitStream* stream, QuadTree* tree, size_t *encoded_bits);
static void ecrire_quadtree(QuadTree* tree, const char* fichier);
static void retirer_extension(const char *filename, char *basename, size_t size);

// fonctions de décodage
static unsigned char* lire_donnees(FILE *fp, size_t *taille_lue);
static int lire_magic_number(FILE *fp);
static void decoder_quadtree(BitStream* stream, QuadTree* tree);
static void ignorer_commentaires(FILE *fp);
static QuadTree* charger_quadtree(const char* fichier);

// fonctions partages
static bool est_4_fils(int nodeIndex);
static bool est_feuille(QuadTree *tree, int nodeIndex);



int coder_fichier(const char *input_file, const char *output_file, double alpha, int generer_grille, int verbose) {
    if (verbose) printf("Encodage : %s -> %s\n", input_file, output_file);

    Pixmap *pixmap = pgm_vers_pixmap(input_file);
    if (!pixmap) {
        fprintf(stderr, "Erreur lors du chargement de %s\n", input_file);
        return 1;
    }

    int depth = (int)log2(pixmap->width);
    if (pixmap->width != pixmap->height || (1 << depth) != pixmap->width) {
        fprintf(stderr, "L'image doit être carrée et de taille 2^n x 2^n\n");
        liberer_pixmap(pixmap);
        return 1;
    }

    QuadTree *tree = creer_quadtree(depth);
    construire_quadtree(tree, pixmap, 0, 0, pixmap->width, 0);

    if (verbose) printf("QuadTree construit avec succès.\n");

    if (alpha > 0.0) {
        appliquer_filtrage(tree, alpha);
        if (verbose) printf("Filtrage appliqué avec alpha = %.2f.\n", alpha);
    }

    // nom de fichier de sortie par defaut
    char default_output_file[256] = "QTC/out.qtc";
    if (!output_file) {
        output_file = default_output_file;
    }

    ecrire_quadtree(tree, output_file);
    if (verbose) printf("Fichier QTC écrit : %s\n", output_file);

    if (generer_grille) {
        char output_filename[256];
        strncpy(output_filename, basename((char *)output_file), sizeof(output_filename)); // recuperer uniquement le nom de base
        output_filename[sizeof(output_filename) - 1] = '\0';

        char basename_without_ext[200];
        retirer_extension(output_filename, basename_without_ext, sizeof(basename_without_ext));

        char grille_file[256];
        snprintf(grille_file, sizeof(grille_file), "PGM/%s_g.pgm", basename_without_ext);
        generer_grille_segmentation(pixmap, tree, grille_file);
        if (verbose) printf("Grille de segmentation générée : %s\n", grille_file);
    }


    liberer_quadtree(tree);
    liberer_pixmap(pixmap);

    return 0;
}


int decoder_fichier(const char *input_file, const char *output_file, int generer_grille, int verbose) {
    if (verbose) printf("Décodage : %s -> %s\n", input_file, output_file);
    // on remplit le quadtree
    QuadTree *tree = charger_quadtree(input_file);
    if (!tree) {
        fprintf(stderr, "Erreur lors du décodage de %s\n", input_file);
        return 1;
    }
    if (verbose) printf("QuadTree chargé avec succès depuis %s.\n", input_file);

    // on passe du quadtree a pixmap
    Pixmap *pixmap = quadtree_vers_pixmap(tree);
    if (!pixmap) {
        fprintf(stderr, "Erreur lors de la reconstruction de la Pixmap\n");
        liberer_quadtree(tree);
        return 1;
    }
    if (verbose) printf("Conversion QuadTree -> Pixmap réussie.\n");

    char default_output_file[256] = "PGM/out.pgm";
    if (!output_file) {
        output_file = default_output_file;
    }

    pixmap_vers_pgm(output_file, pixmap);
    if (verbose) printf("Fichier PGM écrit : %s\n", output_file);

    if (generer_grille) {
        char output_filename[256];
        strncpy(output_filename, basename((char *)output_file), sizeof(output_filename)); // on recupere uniquement le nom de base
        output_filename[sizeof(output_filename) - 1] = '\0';

        char basename_without_ext[200];
        retirer_extension(output_filename, basename_without_ext, sizeof(basename_without_ext));

        char grille_file[256];
        snprintf(grille_file, sizeof(grille_file), "PGM/%s_g.pgm", basename_without_ext);
        generer_grille_segmentation(pixmap, tree, grille_file);
        if (verbose) printf("Grille de segmentation générée : %s\n", grille_file);
    }


    liberer_quadtree(tree);
    liberer_pixmap(pixmap);

    return 0;
}


/**
 * @brief Écrit un QuadTree compressé dans un fichier.
 * 
 * Cette fonction gère l'écriture de toutes les parties nécessaires à la compression,
 * y compris l'entête, la profondeur, et les données du QuadTree.
 * 
 * @param tree Le QuadTree contenant les données à écrire.
 * @param fichier Le chemin vers le fichier de sortie.
 */
static void ecrire_quadtree(QuadTree* tree, const char* fichier) {
    FILE* fp = fopen(fichier, "wb");
    if (!fp) {
        fprintf(stderr, "Erreur : impossible de créer le fichier %s\n", fichier);
        return;
    }
    // calculer la taille de l'image originale en bits
    size_t taille_image = (1 << tree->depth) * (1 << tree->depth); 
    size_t bits_original = taille_image * 8;

    size_t bits_encodes = 0;

    unsigned char buffer_entete[1024] = {0};
    memset(buffer_entete, 0, sizeof(buffer_entete));
    BitStream tampon_entete = { .ptr = buffer_entete, .capa = CHAR_BIT };

    size_t buffer_size = (tree->total_nodes * 8);
    unsigned char* buffer_donnees = malloc(buffer_size);
    if (!buffer_donnees) {
        fprintf(stderr, "erreur : memoire insuffisante pour le tampon.\n");
        fclose(fp);
        return;
    }
    memset(buffer_donnees, 0, buffer_size);
    BitStream tampon_donnees = { .ptr = buffer_donnees, .capa = CHAR_BIT };
    encoder_entete(&tampon_entete);

    encoder_profondeur(&tampon_donnees, tree);

    encoder_quadtree(&tampon_donnees, tree, &bits_encodes);

    // calculer le taux de compression
    size_t encoded_bytes = (bits_encodes + 7) / 8; 
    size_t encoded_total_bits = encoded_bytes * 8; 
    double compression_rate = ((double)encoded_total_bits / bits_original) * 100;

    encoder_taux_compression(&tampon_entete, compression_rate);

    // ecrire le tampon entete dans le fichier
    size_t ascii_size = tampon_entete.ptr - buffer_entete;
    fwrite(buffer_entete, sizeof(unsigned char), ascii_size, fp);

    // ecrire le tampon donnees dans le fichier
    size_t binary_size = tampon_donnees.ptr - buffer_donnees + (tampon_donnees.capa != CHAR_BIT ? 1 : 0);
    fwrite(buffer_donnees, sizeof(unsigned char), binary_size, fp);

    fclose(fp);
    free(buffer_donnees);
}


/**
 * @brief Encode l'entête du fichier compressé.
 * 
 * Cette fonction ajoute des informations comme l'identification du fichier,
 * la date et l'heure actuelles dans le flux binaire.
 * 
 * @param stream Le flux binaire où écrire les données.
 */
static void encoder_entete(BitStream* stream) {
    const char* identification = "Q1\n";
    for (size_t i = 0; i < strlen(identification); i++) {
        pushbits(stream, identification[i], CHAR_BIT);
    }

    // ajouter la date et l'heure actuelle
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    char date_time[64];
    strftime(date_time, sizeof(date_time), "# %a %b %d %H:%M:%S %Y\n", t);
    for (size_t i = 0; i < strlen(date_time); i++) {
        pushbits(stream, date_time[i], CHAR_BIT);
    }

}

/**
 * @brief Encode le taux de compression dans le fichier.
 * 
 * Ajoute un commentaire contenant le taux de compression dans le fichier compressé.
 * 
 * @param stream Le flux binaire où écrire les données.
 * @param compression_rate Le taux de compression calculé.
 */
static void encoder_taux_compression(BitStream* stream, double compression_rate) {
    char compression_comment[32];
    snprintf(compression_comment, sizeof(compression_comment), "# compression rate %.2f%%\n", compression_rate);
    for (size_t i = 0; i < strlen(compression_comment); i++) {
        pushbits(stream, compression_comment[i], CHAR_BIT);
    }
}

/**
 * @brief Encode la profondeur du QuadTree.
 * 
 * La profondeur est utilisée pour définir la taille de l'image compressée.
 * 
 * @param stream Le flux binaire où écrire les données.
 * @param tree Le QuadTree contenant les données de l'image.
 */
static void encoder_profondeur(BitStream* stream, QuadTree* tree) {
    unsigned char taille_arbre = (unsigned char)(tree->depth);
    pushbits(stream, taille_arbre, CHAR_BIT);
}


/**
 * @brief Encode les données du QuadTree dans le fichier.
 * 
 * Traverse le QuadTree et écrit les données dans un format compressé.
 * 
 * @param stream Le flux binaire où écrire les données.
 * @param tree Le QuadTree contenant les données.
 * @param encoded_bits Un pointeur pour compter le nombre de bits encodés.
 */
static void encoder_quadtree(BitStream *stream, QuadTree *tree, size_t *encoded_bits) {
    for (int nodeIndex = 0; nodeIndex < tree->total_nodes; nodeIndex++) {
        QuadNode *node = &tree->nodes[nodeIndex];
        
        if (nodeIndex != 0) {
            int parentIndex = (nodeIndex - 1) / 4;
            QuadNode *parentNode = &tree->nodes[parentIndex];
            if (parentNode->u == 1) {
                continue; // sauter ce noeud si son pere est uniforme
            }
        }

        if (est_feuille(tree, nodeIndex)) {
            if (est_4_fils(nodeIndex)) {
                continue; // rien a encoder, passer au prochain
            }

            pushbits(stream, node->m, 8);
            *encoded_bits += 8;
            continue; // passer au prochain noeud
        }

        if (!est_4_fils(nodeIndex)){
            pushbits(stream, node->m, 8); // on code  m 
            *encoded_bits += 8;
        }

        pushbits(stream, node->epsilon, 2); // on code  e dans tt les cas
        *encoded_bits += 2; 

        if ( node->epsilon == 0 ){
            pushbits(stream, node->u, 1); //si e==0 on code U 
            *encoded_bits += 1;
        }
      
    }
}




/**
 * @brief Charge un QuadTree depuis un fichier compressé.
 * 
 * Lit les données compressées depuis un fichier et reconstruit un QuadTree.
 * 
 * @param fichier Le chemin vers le fichier compressé.
 * @return Un pointeur vers le QuadTree reconstruit ou NULL en cas d'erreur.
 */
static QuadTree* charger_quadtree(const char* fichier) {
    FILE *fp = fopen(fichier, "rb");
    if (!fp) {
        fprintf(stderr, "Erreur : impossible d'ouvrir le fichier %s\n", fichier);
        return NULL;
    }

    if (!lire_magic_number(fp)) {
        fclose(fp);
        return NULL;
    }

    ignorer_commentaires(fp);

    size_t taille_lue;
    unsigned char *buffer = lire_donnees(fp, &taille_lue);
    if (!buffer) {
        fprintf(stderr, "Erreur : impossible de lire les données binaires\n");
        fclose(fp);
        return NULL;
    }

    BitStream stream = { .ptr = buffer, .capa = CHAR_BIT };
    unsigned char profondeur = 0;
    // on extrait la profondeur
    pullbits(&stream, &profondeur, 8);
    QuadTree *tree = creer_quadtree(profondeur);
    if (!tree) {
        fprintf(stderr, "Erreur : impossible de créer le QuadTree\n");
        free(buffer);
        return NULL;
    }
    // on extrait les donnees pour remplir le tree
    decoder_quadtree(&stream, tree);

    free(buffer);

    return tree;
}


/**
 * @brief verifie le magic number du fichier.
 * 
 * Le magic number identifie le format du fichier compressé.
 * 
 * @param fp Le fichier ouvert en lecture binaire.
 * @return 1 si le magic number est valide, 0 sinon.
 */
static int lire_magic_number(FILE *fp) {
    char buffer[256];

    // lire une ligne complète avec fgets
    if (fgets(buffer, sizeof(buffer), fp) == NULL) {
        fprintf(stderr, "Erreur : impossible de lire le magic number\n");
        return 0; // echec
    }

    // verifier si la ligne commence par "Q1"
    if (strncmp(buffer, "Q1", 2) != 0) {
        fprintf(stderr, "Erreur : format de fichier non valide (magic number incorrect)\n");
        return 0; // echec
    }

    return 1; 
}
/**
 * @brief Lit les données d'un fichier.
 * 
 * Cette fonction charge les données binaires d'un fichier dans un tampon.
 * 
 * @param fp Le fichier ouvert en lecture binaire.
 * @param taille_lue Un pointeur pour stocker la taille des données lues.
 * @return Un pointeur vers les données lues ou NULL en cas d'erreur.
 */
static unsigned char* lire_donnees(FILE *fp, size_t *taille_lue) {
    size_t buffer_size = 1024; // taille initiale du tampon
    size_t total_read = 0;
    unsigned char *buffer = malloc(buffer_size);

    if (!buffer) {
        fprintf(stderr, "Erreur : mémoire insuffisante pour le tampon\n");
        return NULL;
    }

    while (!feof(fp)) {
        if (total_read == buffer_size) {
            // augmenter dynamiquement la taille du tampon
            buffer_size *= 2;
            unsigned char *temp = realloc(buffer, buffer_size);
            if (!temp) {
                fprintf(stderr, "Erreur : echec de la reallocation du tampon\n");
                free(buffer);
                return NULL;
            }
            buffer = temp;
        }

        size_t read_bytes = fread(buffer + total_read, 1, buffer_size - total_read, fp);
        total_read += read_bytes;

        if (ferror(fp)) {
            fprintf(stderr, "Erreur : problème de lecture\n");
            free(buffer);
            return NULL;
        }
    }

    // reduire le tampon à la taille reellement lue
    unsigned char *final_buffer = realloc(buffer, total_read);
    if (!final_buffer) {
        fprintf(stderr, "Erreur : échec de la réduction du tampon\n");
        free(buffer);
        return NULL;
    }

    *taille_lue = total_read;
    return final_buffer;
}


/**
 * @brief Décode les données d'un QuadTree depuis un flux binaire.
 * 
 * Cette fonction reconstruit un QuadTree compressé à partir des données binaires.
 * 
 * @param stream Le flux binaire contenant les données.
 * @param tree Le QuadTree à remplir.
 */
static void decoder_quadtree(BitStream *stream, QuadTree *tree) {
    for (int nodeIndex = 0; nodeIndex < tree->total_nodes; nodeIndex++) {
        QuadNode *node = &tree->nodes[nodeIndex];

        // Si le père de ce nœud est uniforme
        if (nodeIndex != 0) {
            int parentIndex = (nodeIndex - 1) / 4;
            QuadNode *parentNode = &tree->nodes[parentIndex];
            if (parentNode->u == 1) {
                node->m = parentNode->m;
                node->epsilon = 0;
                node->u = 1;
                continue; // passer au prochain noeud
            }
        }
        // Si c'est un quatrième fils
        if (est_4_fils(nodeIndex)) {
            int parentIndex = (nodeIndex - 1) / 4;
            QuadNode *parentNode = &tree->nodes[parentIndex];
            int m1 = tree->nodes[4 * parentIndex + 1].m;
            int m2 = tree->nodes[4 * parentIndex + 2].m;
            int m3 = tree->nodes[4 * parentIndex + 3].m;
            node->m = (4 * parentNode->m + parentNode->epsilon) - (m1 + m2 + m3);

            // Si c'est une feuille, on ecrit epsilon et u
            if (est_feuille(tree, nodeIndex)) {
                node->epsilon = 0;
                node->u = 1;
            } else {
                unsigned char epsilon;
                pullbits(stream, &epsilon, 2);
                node->epsilon = epsilon;

                // lire epsilon si u == 0
                if (epsilon == 0) {
                    unsigned char u;
                    pullbits(stream, &u, 1);
                    node->u = u;
                } else {
                    node->u = 0;
                }
            }
            continue;
        }

        if (est_feuille(tree, nodeIndex)) {
            unsigned char m;
            pullbits(stream, &m, 8);
            node->m = m;
            node->epsilon = 0;
            node->u = 1;
            continue; // Passer au prochain noeud
        }

        // pour les autres noeuds non feuilles
        if (!est_4_fils(nodeIndex)) {
            unsigned char m;
            pullbits(stream, &m, 8);
            node->m = m;
        }

        // lire epsilon
        unsigned char epsilon;
        pullbits(stream, &epsilon, 2);
        node->epsilon = epsilon;

        if (epsilon == 0) {
            unsigned char u;
            pullbits(stream, &u, 1);
            node->u = u;
        } else {
            node->u = 0;
        }
    }
}




/**
 * @brief Ignore les lignes de commentaires dans un fichier.
 * 
 * Les lignes qui commencent par `#` sont sautées. Les autres types de lignes (vides ou avec données) sont respectées.
 * 
 * @param fp Le fichier ouvert en lecture binaire.
 */
static void ignorer_commentaires(FILE *fp) {
    int c;
    char buffer[2048];

    while ((c = fgetc(fp)) == '#') {
        fgets(buffer, sizeof(buffer), fp);
    }

    if (c != EOF) {
        ungetc(c, fp);
    }
}


/**
 * @brief verifie si un noeud est un quatrième fils dans le QuadTree.
 * 
 * Le quatrième fils correspond au dernier noeud d'un groupe de quatre enfants.
 * 
 * @param nodeIndex L'index du nœud à vérifier.
 * @return true si c'est un quatrième fils, false sinon.
 */
static bool est_4_fils(int nodeIndex) {
   if (nodeIndex == 0) {
        return false; 
    }
    return nodeIndex % 4 == 0;
}


/**
 * @brief verifie si un noeud est une feuille dans le QuadTree.
 * 
 * Une feuille n'a pas d'enfants dans le QuadTree.
 * 
 * @param tree Le QuadTree contenant le nœud.
 * @param nodeIndex L'index du nœud à vérifier.
 * @return true si le nœud est une feuille, false sinon.
 */
static bool est_feuille(QuadTree *tree, int nodeIndex) {
    int firstChildIndex = 4 * nodeIndex + 1;
    return (firstChildIndex >= tree->total_nodes); 
}

/**
 * Retire l'extension d'un nom de fichier.
 *
 * @param filename Nom du fichier avec extension.
 * @param basename Buffer pour le nom sans extension.
 * @param size Taille maximale du buffer.
 */

static void retirer_extension(const char *filename, char *basename, size_t size) {
    strncpy(basename, filename, size);
    basename[size - 1] = '\0';
    
    char *dot = strrchr(basename, '.');
    if (dot) {
        *dot = '\0';
    }
}