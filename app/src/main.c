/**
 * @file main.c
 * @brief Point d'entrée principal du programme.
 *
 * Ce fichier contient la fonction `main` qui sert d'interface utilisateur 
 * pour gérer les options de compression et décompression des fichiers 
 * d'images. Il parse les arguments de la ligne de commande et appelle 
 * les fonctions correspondantes pour l'encodage ou le décodage.
 */

#include "../include/qtc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void afficher_aide() {
    printf("Usage : codec [OPTIONS]\n");
    printf("Options :\n");
    printf("  -c : Encodeur (PGM vers QTC)\n");
    printf("  -u : Décodeur (QTC vers PGM)\n");
    printf("  -i input : Fichier d'entrée (PGM ou QTC)\n");
    printf("  -o output : Fichier de sortie (QTC ou PGM)\n");
    printf("  -a alpha : Paramètre alpha pour l'encodage avec perte\n");
    printf("  -g : Générer une grille de segmentation\n");
    printf("  -h : Afficher cette aide\n");
    printf("  -v : Mode verbeux\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        afficher_aide();
        return 1;
    }

    const char *input_file = NULL;
    const char *output_file = NULL;
    double alpha = 0.0;
    int encodeur = 0;
    int decodeur = 0;
    int verbose = 0;
    int generer_grille = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0) {
            encodeur = 1;
        } else if (strcmp(argv[i], "-u") == 0) {
            decodeur = 1;
        } else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            input_file = argv[++i];
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_file = argv[++i];
        } else if (strcmp(argv[i], "-a") == 0 && i + 1 < argc) {
            alpha = atof(argv[++i]);
        } else if (strcmp(argv[i], "-g") == 0) {
            generer_grille = 1;
        } else if (strcmp(argv[i], "-h") == 0) {
            afficher_aide();
            return 0;
        } else if (strcmp(argv[i], "-v") == 0) {
            verbose = 1;
        } else {
            fprintf(stderr, "Option inconnue : %s\n", argv[i]);
            afficher_aide();
            return 1;
        }
    }

    if (!input_file) {
        fprintf(stderr, "Erreur : fichier d'entrée non spécifié.\n");
        afficher_aide();
        return 1;
    }

    if (encodeur) {
        return coder_fichier(input_file, output_file, alpha, generer_grille, verbose);
    } else if (decodeur) {
        return decoder_fichier(input_file, output_file, generer_grille, verbose);
    } else {
        fprintf(stderr, "Erreur : spécifiez -c (encodeur) ou -u (decodeur).\n");
        afficher_aide();
        return 1;
    }
}
