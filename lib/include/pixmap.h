#ifndef PIXMAP_H
#define PIXMAP_H

/**
 * @file pixmap.h
 * @brief Manipulation d'images en niveaux de gris (Pixmap).
 *
 * Ce fichier contient la définition de la structure `Pixmap` et les fonctions
 * associées pour convertir entre le format PGM et la structure Pixmap, ainsi
 * que pour libérer la mémoire associée.
 */

/**
 * @brief Structure représentant une image en niveaux de gris (Pixmap).
 */
typedef struct {
    int width;                /**< Largeur de l'image. */
    int height;               /**< Hauteur de l'image. */
    unsigned char *pixels;    /**< Tableau des pixels en niveaux de gris. */
} Pixmap;

/**
 * @brief Convertit un fichier PGM en une Pixmap.
 * 
 * Cette fonction lit un fichier PGM et charge son contenu dans une structure Pixmap.
 *
 * @param fichier Chemin du fichier PGM à lire.
 * @return Un pointeur vers la Pixmap créée ou NULL en cas d'erreur.
 */
Pixmap* pgm_vers_pixmap(const char* fichier);

/**
 * @brief Sauvegarde une Pixmap au format PGM.
 * 
 * Cette fonction écrit les données d'une structure Pixmap dans un fichier PGM.
 *
 * @param fichier Chemin du fichier PGM à créer.
 * @param pixmap La Pixmap contenant les données à sauvegarder.
 */
void pixmap_vers_pgm(const char* fichier, const Pixmap* pixmap);

/**
 * @brief Libère la mémoire allouée pour une Pixmap.
 * 
 * Cette fonction libère la mémoire associée à une structure Pixmap, y compris
 * le tableau de pixels.
 *
 * @param pixmap La Pixmap à libérer.
 */
void liberer_pixmap(Pixmap *pixmap);

#endif // PIXMAP_H
