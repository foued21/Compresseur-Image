#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include "../include/quadtree.h"
#include "../include/pixmap.h"

/**
 * @file segmentation.h
 * @brief Génération de grilles de segmentation à partir de QuadTrees.
 *
 * Ce fichier contient la déclaration de la fonction permettant de générer
 * une grille de segmentation à partir d'un QuadTree. La grille est sauvegardée
 * en tant qu'image au format PGM.
 */

/**
 * @brief Génère une grille de segmentation à partir d'un QuadTree.
 * 
 * Cette fonction parcourt un QuadTree pour dessiner les bordures des blocs
 * uniformes, créant ainsi une grille de segmentation. Le résultat est ensuite
 * sauvegardé dans un fichier PGM.
 * 
 * @param pixmap La Pixmap de référence (dimensions de l'image).
 * @param tree Le QuadTree source représentant les blocs de l'image.
 * @param output_file Le nom du fichier PGM où sauvegarder la grille.
 */
void generer_grille_segmentation(const Pixmap *pixmap, const QuadTree *tree, const char *output_file);

#endif // SEGMENTATION_H
