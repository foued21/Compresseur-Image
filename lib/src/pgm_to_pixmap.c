#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include "libmtrack.h"
#include "../include/pixmap.h"


static void ignorer_commentaires_et_espaces(FILE *fp);

Pixmap* pgm_vers_pixmap(const char *fichier) {
    FILE *fp = fopen(fichier, "rb");
    if (!fp) {
        fprintf(stderr, "I=ompossible d'ouvrir le fichier %s\n", fichier);
        return NULL;
    }

    // lire le firmat pgm
    char format[3];
    if (!fgets(format, sizeof(format), fp)) {
        fprintf(stderr, "impossible de lire le format dans %s\n", fichier);
        fclose(fp);
        return NULL;
    }

    // verifier que c P5
    if (format[0] != 'P' || format[1] != '5') {
        fprintf(stderr, "le fichier %s n'est pas au format PGM P5\n", fichier);
        fclose(fp);
        return NULL;
    }
    
    ignorer_commentaires_et_espaces(fp);

    // lire les dimensions
    int width, height;
    if (fscanf(fp, "%d %d", &width, &height) != 2) {
        fprintf(stderr, "dimensions invalides dans %s\n", fichier);
        fclose(fp);
        return NULL;
    }

    ignorer_commentaires_et_espaces(fp);

    int max_val;
    if (fscanf(fp, "%d", &max_val) != 1) {
        fprintf(stderr, "impossible de lire la valeur maximale dans %s\n", fichier);
        fclose(fp);
        return NULL;
    }

    if (max_val != 255) {
        fprintf(stderr, "valeur maximale incorrecte (%d) dans %s\n", max_val, fichier);
        fclose(fp);
        return NULL;
    }

    // igonrer le '\n' final après max_val
    fgetc(fp);
    ignorer_commentaires_et_espaces(fp);

    Pixmap *pixmap = malloc(sizeof(Pixmap));
    if (!pixmap) {
        fprintf(stderr, "probleme de memoire pour la pixmap\n");
        fclose(fp);
        return NULL;
    }
    pixmap->width = width;
    pixmap->height = height;

    pixmap->pixels = malloc(width * height * sizeof(unsigned char));
    if (!pixmap->pixels) {
        fprintf(stderr, "probleme de memoire pour les pixels\n");
        free(pixmap);
        fclose(fp);
        return NULL;
    }

    if (fread(pixmap->pixels, sizeof(unsigned char), width * height, fp) != (size_t)(width * height)) {
        fprintf(stderr, "probleme de lecture de pixels dans %s\n", fichier);
        free(pixmap->pixels);
        free(pixmap);
        fclose(fp);
        return NULL;
    }

    fclose(fp);
    return pixmap;
}


void liberer_pixmap(Pixmap *pixmap) {
    if (pixmap) {
        free(pixmap->pixels);
        free(pixmap);
    }
}

/**
 * @brief Ignore les commentaires et les espaces dans un fichier.
 * 
 * Cette fonction lit un fichier, ignore les lignes commençant par `#`
 * ainsi que les espaces, tabulations et sauts de ligne.
 * 
 * @param fp Le fichier ouvert en lecture.
 */
static void ignorer_commentaires_et_espaces(FILE *fp) {
    int c;
    while ((c = fgetc(fp)) != EOF) {
        if (c == '#') {
            while ((c = fgetc(fp)) != '\n' && c != EOF);
        } else if (c != '\n' && c != ' ' && c != '\t') {
            ungetc(c, fp);
            break;
        }
    }
}