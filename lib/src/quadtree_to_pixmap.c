#include <stdlib.h>
#include <stdio.h>
#include "../include/quadtree.h"
#include "../include/pixmap.h"

static void remplir_pixmap(QuadTree *tree, Pixmap *pixmap, int x, int y, int size, int index);



Pixmap* quadtree_vers_pixmap(QuadTree *tree) {
    if (!tree) return NULL;

    // Calculer la taille de l'image
    int size = 1 << tree->depth; // 2^depth

    // Allouer la structure Pixmap
    Pixmap *pixmap = malloc(sizeof(Pixmap));
    if (!pixmap) {
        fprintf(stderr, "Erreur : allocation de Pixmap\n");
        return NULL;
    }

    pixmap->width = size;
    pixmap->height = size;
    pixmap->pixels = malloc(size * size * sizeof(unsigned char));
    if (!pixmap->pixels) {
        fprintf(stderr, "Erreur : allocation de la mémoire pour les pixels\n");
        free(pixmap);
        return NULL;
    }

    // Initialiser tous les pixels à zéro
    for (int i = 0; i < size * size; i++) {
        pixmap->pixels[i] = 0;
    }

    // Remplir les pixels à partir du QuadTree
    remplir_pixmap(tree, pixmap, 0, 0, size, 0);

    return pixmap;
}


/**
 * @brief Remplit les pixels d'une Pixmap à partir d'un QuadTree.
 * 
 * Fonction récursive qui parcourt un QuadTree et remplit la Pixmap en fonction
 * des données de chaque nœud. Les noeuds uniformes remplissent directement une
 * zone, tandis que les noeuds non uniformes sont divisés en sous-zones.
 * 
 * @param tree Le QuadTree contenant les données.
 * @param pixmap La Pixmap à remplir.
 * @param x La coordonnée x du coin supérieur gauche de la zone.
 * @param y La coordonnée y du coin supérieur gauche de la zone.
 * @param size La taille de la zone (côté du carré).
 * @param index L'index du nœud actuel dans le QuadTree.
 */
static void remplir_pixmap(QuadTree *tree, Pixmap *pixmap, int x, int y, int size, int index) {
    if (index >= tree->total_nodes) return;

    QuadNode *node = &tree->nodes[index];

    if (node->u == 1) {
        // Si le noeud est uniforme, remplir la zone correspondante
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                pixmap->pixels[(y + i) * pixmap->width + (x + j)] = node->m;
            }
        }
    } else {
        // Sinon, diviser la zone en 4 et traiter les enfants
        int half_size = size / 2;
        remplir_pixmap(tree, pixmap, x, y, half_size, 4 * index + 1);        // Enfant haut-gauche
        remplir_pixmap(tree, pixmap, x + half_size, y, half_size, 4 * index + 2); // Enfant haut-droite
        remplir_pixmap(tree, pixmap, x, y + half_size, half_size, 4 * index + 4); // Enfant bas-gauche
        remplir_pixmap(tree, pixmap, x + half_size, y + half_size, half_size, 4 * index + 3); // Enfant bas-droite
    }
}
