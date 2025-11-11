#include <stdio.h>
#include <stdlib.h>
#include "../include/pixmap.h"



void pixmap_vers_pgm(const char *fichier, const Pixmap *pixmap) {
    if (!pixmap || !pixmap->pixels) {
        fprintf(stderr, "Erreur : Pixmap invalide\n");
        return;
    }

    FILE *fp = fopen(fichier, "wb");
    if (!fp) {
        fprintf(stderr, "Erreur : impossible de créer le fichier %s\n", fichier);
        return;
    }

    // ecrire l'en-tete PGM
    fprintf(fp, "P5\n");
    fprintf(fp, "# Created by Group 7 HAMMI_MAOUCHE L3Prog_2024_2025 at UGE\n");
    fprintf(fp, "%d %d\n255\n", pixmap->width, pixmap->height);

    // ecrire les pixels
    size_t total_pixels = pixmap->width * pixmap->height;
    if (fwrite(pixmap->pixels, sizeof(unsigned char), total_pixels, fp) != total_pixels) {
        fprintf(stderr, "Erreur : échec de l'écriture des pixels\n");
    }

    fclose(fp);
}
