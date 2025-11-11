#include <stdlib.h>
#include <math.h>
//#include "libmtrack.h"
#include "../include/quadtree.h"
#include "../include/pixmap.h"



QuadTree* creer_quadtree(int depth) {
    QuadTree *tree = malloc(sizeof(QuadTree));
    if (!tree) return NULL;

    
    tree->total_nodes = (pow(4, depth + 1) - 1) / 3;
    tree->depth = depth;
    tree->nodes = malloc(tree->total_nodes * sizeof(QuadNode));
    if (!tree->nodes) {
        free(tree);
        return NULL;
    }

    for (int i = 0; i < tree->total_nodes; i++) {
        tree->nodes[i].m = 0;
        tree->nodes[i].epsilon = 0;
        tree->nodes[i].u = 0;
    }
    return tree;
}


void liberer_quadtree(QuadTree *tree) {
    if (tree) {
        free(tree->nodes);
        free(tree);
    }
}


void construire_quadtree(QuadTree *tree, Pixmap *pixmap, int x, int y, int taille, int index) {
    if (taille == 1) { // cas si c une feuille
        int pixel_val = pixmap->pixels[y * pixmap->width + x];
        tree->nodes[index].m = pixel_val;
        tree->nodes[index].epsilon = 0;
        tree->nodes[index].u = 1; // la feuille est tjrs unifrome
        tree->nodes[index].v = 0.0; // v nulle pour une feuille
        return;
    }

    int demi_taille = taille / 2;
    int enfants[4] = {4 * index + 1, 4 * index + 2, 4 * index + 4, 4 * index + 3}; // on respecte le sens de l'aiguille

    // construire les 4 enfants
    construire_quadtree(tree, pixmap, x, y, demi_taille, enfants[0]);
    construire_quadtree(tree, pixmap, x + demi_taille, y, demi_taille, enfants[1]);
    construire_quadtree(tree, pixmap, x, y + demi_taille, demi_taille, enfants[2]);
    construire_quadtree(tree, pixmap, x + demi_taille, y + demi_taille, demi_taille, enfants[3]);

    // la moyenne du noeud courant
    int somme = tree->nodes[enfants[0]].m + tree->nodes[enfants[1]].m +
                tree->nodes[enfants[2]].m + tree->nodes[enfants[3]].m;

    tree->nodes[index].m = somme / 4;
    tree->nodes[index].epsilon = somme % 4;

    // la varinace du noeud courant a partir des ses enfants
    double mu = 0.0;
    for (int i = 0; i < 4; i++) {
        QuadNode *fils = &tree->nodes[enfants[i]];
        mu += (fils->v * fils->v) + (tree->nodes[index].m - fils->m) * (tree->nodes[index].m - fils->m);
    }
    tree->nodes[index].v = sqrt(mu) / 4.0;

    // on verifie si le noeud courant est uniforme
    int u1 = tree->nodes[enfants[0]].u, u2 = tree->nodes[enfants[1]].u;
    int u3 = tree->nodes[enfants[2]].u, u4 = tree->nodes[enfants[3]].u;

    if (u1 == 1 && u2 == 1 && u3 == 1 && u4 == 1 &&
        tree->nodes[enfants[0]].m == tree->nodes[enfants[1]].m &&
        tree->nodes[enfants[1]].m == tree->nodes[enfants[2]].m &&
        tree->nodes[enfants[2]].m == tree->nodes[enfants[3]].m) {
        tree->nodes[index].u = 1;
        tree->nodes[index].v = 0; // si uniforme, variance = 0
    } else {
        tree->nodes[index].u = 0;
    }
}

