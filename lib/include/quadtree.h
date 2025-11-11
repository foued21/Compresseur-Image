#ifndef QUADTREE_H
#define QUADTREE_H

#include "pixmap.h"

/**
 * @file quadtree.h
 * @brief Déclarations pour la gestion des QuadTrees.
 *
 * Ce fichier contient les définitions des structures et des fonctions
 * permettant de manipuler des QuadTrees, une structure utilisée pour
 * représenter des images de manière hiérarchique.
 */

/**
 * @brief Structure représentant un nœud dans le QuadTree.
 *
 * Chaque nœud contient les informations suivantes :
 * - `m` : La moyenne des niveaux de gris des pixels dans le bloc représenté.
 * - `epsilon` : L'erreur de quantification.
 * - `u` : Indique si le bloc est uniforme (1 : uniforme, 0 : non uniforme).
 * - `v` : La variance des niveaux de gris dans le bloc.
 */
typedef struct {
    unsigned char m;       /**< Moyenne des niveaux de gris dans le nœud. */
    unsigned char epsilon; /**< Erreur de quantification. */
    unsigned char u;       /**< Uniformité du nœud (1 si uniforme, 0 sinon). */
    double v;              /**< Variance des niveaux de gris dans le nœud. */
} QuadNode;

/**
 * @brief Structure représentant un QuadTree.
 *
 * Le QuadTree est une représentation hiérarchique d'une image.
 * - `depth` : La profondeur maximale du QuadTree.
 * - `total_nodes` : Le nombre total de nœuds dans le QuadTree.
 * - `nodes` : Tableau contenant tous les nœuds du QuadTree.
 */
typedef struct {
    int depth;             /**< Profondeur maximale du QuadTree. */
    int total_nodes;       /**< Nombre total de nœuds dans le QuadTree. */
    QuadNode *nodes;       /**< Tableau de nœuds. */
} QuadTree;

/**
 * @brief Crée un QuadTree vide avec une profondeur donnée.
 *
 * Alloue la mémoire pour un QuadTree et initialise ses nœuds.
 *
 * @param depth La profondeur maximale du QuadTree.
 * @return Un pointeur vers le QuadTree créé ou NULL en cas d'erreur.
 */
QuadTree* creer_quadtree(int depth);

/**
 * @brief Construit un QuadTree à partir d'une Pixmap.
 *
 * Cette fonction divise une image Pixmap en blocs pour remplir les données
 * d'un QuadTree.
 *
 * @param tree Le QuadTree à construire.
 * @param pixmap La Pixmap source.
 * @param x Coordonnée x du coin supérieur gauche du bloc.
 * @param y Coordonnée y du coin supérieur gauche du bloc.
 * @param taille Taille du bloc actuel (côté du carré).
 * @param index L'index du nœud actuel dans le QuadTree.
 */
void construire_quadtree(QuadTree* tree, Pixmap* pixmap, int x, int y, int taille, int index);

/**
 * @brief Reconstruit une Pixmap à partir d'un QuadTree.
 *
 * Cette fonction recrée une image Pixmap en niveaux de gris en utilisant
 * les données d'un QuadTree.
 *
 * @param tree Le QuadTree contenant les données de l'image.
 * @return Un pointeur vers la Pixmap recréée ou NULL en cas d'erreur.
 */
Pixmap* quadtree_vers_pixmap(QuadTree *tree);

/**
 * @brief Applique un filtrage sur un QuadTree.
 *
 * Simplifie le QuadTree en fonction de la variance des blocs pour réduire
 * la taille ou améliorer la compression.
 *
 * @param tree Le QuadTree à filtrer.
 * @param alpha Facteur de pondération pour ajuster la tolérance à la variance.
 */
void appliquer_filtrage(QuadTree *tree, double alpha);

/**
 * @brief Libère la mémoire allouée pour un QuadTree.
 *
 * Cette fonction libère la mémoire utilisée par les nœuds et la structure
 * du QuadTree.
 *
 * @param tree Le QuadTree à libérer.
 */
void liberer_quadtree(QuadTree *tree);

#endif // QUADTREE_H
