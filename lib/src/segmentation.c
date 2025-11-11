#include "../include/segmentation.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Dessine les bordures des blocs définis par le QuadTree dans une Pixmap.
 * 
 * Cette fonction parcourt récursivement un QuadTree et dessine les bordures
 * des blocs uniformes directement dans une Pixmap en niveaux de gris. Les bordures
 * sont représentées par des pixels noirs (valeur 0).
 * 
 * @param pixmap La Pixmap à remplir avec la grille de segmentation.
 * @param tree Le QuadTree représentant les blocs de l'image.
 * @param x La coordonnée x du coin supérieur gauche du bloc.
 * @param y La coordonnée y du coin supérieur gauche du bloc.
 * @param taille La taille du bloc actuel (côté du carré).
 * @param index L'index du nœud actuel dans le QuadTree.
 */
static void dessiner_segmentation(Pixmap *pixmap, const QuadTree *tree, int x, int y, int taille, int index) ;


void generer_grille_segmentation(const Pixmap *pixmap, const QuadTree *tree, const char *output_file) {
    Pixmap *grille = malloc(sizeof(Pixmap));
    if (!grille) {
        fprintf(stderr, "Erreur d'allocation memeoire pour la grille\n");
        return;
    }

    grille->width = pixmap->width;
    grille->height = pixmap->height;
    grille->pixels = malloc(grille->width * grille->height * sizeof(unsigned char));
    if (!grille->pixels) {
        fprintf(stderr, "Erreur d'allocation memoire pour les pixels de la grille\n");
        free(grille);
        return;
    }

    for (int i = 0; i < grille->width * grille->height; i++) {
        grille->pixels[i] = 255; // blanc
    }

    dessiner_segmentation(grille, tree, 0, 0, grille->width, 0);

    // sauvegarder la grille dans un fichier PGM
    pixmap_vers_pgm(output_file, grille);

    free(grille->pixels);
    free(grille);
}

static void dessiner_segmentation(Pixmap *pixmap, const QuadTree *tree, int x, int y, int taille, int index) {
    if (index >= tree->total_nodes || taille <= 1) return;

    QuadNode *node = &tree->nodes[index];

    // verifier si le noeud est une feuille (pas de fils)
    int first_child_index = 4 * index + 1;
    if (node->u == 1 ) {
        if(first_child_index < tree->total_nodes){
        // le noeud est une feuille, dessiner ses bordures
            for (int i = 0; i < taille; i++) {
                // ligne superieure
                if (y < pixmap->height && x + i < pixmap->width)
                    pixmap->pixels[y * pixmap->width + (x + i)] = 0;
                // ligne inferieure
                if (y + taille - 1 < pixmap->height && x + i < pixmap->width)
                    pixmap->pixels[(y + taille - 1) * pixmap->width + (x + i)] = 0;
                // colonne gauche
                if (x < pixmap->width && y + i < pixmap->height)
                    pixmap->pixels[(y + i) * pixmap->width + x] = 0;
                // colonne droite
                if (x + taille - 1 < pixmap->width && y + i < pixmap->height)
                    pixmap->pixels[(y + i) * pixmap->width + (x + taille - 1)] = 0;
            }
        }
        return;
    }

    // si le noeud a des fils, diviser en 4 enfants
    int demi_taille = taille / 2;
    dessiner_segmentation(pixmap, tree, x, y, demi_taille, 4 * index + 1);
    dessiner_segmentation(pixmap, tree, x + demi_taille, y, demi_taille, 4 * index + 2);
    dessiner_segmentation(pixmap, tree, x, y + demi_taille, demi_taille, 4 * index + 4);
    dessiner_segmentation(pixmap, tree, x + demi_taille, y + demi_taille, demi_taille, 4 * index + 3);
}

