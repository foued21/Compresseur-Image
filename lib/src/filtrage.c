#include <math.h>    
#include <stdio.h> 
#include "../include/quadtree.h"

static int filtrage(QuadTree *tree, int index, double sigma, double alpha);



void appliquer_filtrage(QuadTree *tree, double alpha) {
    // calculer medvar et maxvar
    double total_variance = 0.0;
    double variance_max = 0.0;
    int nb_internal_nodes = 0;

    for (int i = 0; i < tree->total_nodes; i++) {
        int firstChildIndex = 4 * i + 1;
        if (!(firstChildIndex >= tree->total_nodes)) { // on prend juste les noeuds internes
            double v = tree->nodes[i].v;
            total_variance += v;
            if (v > variance_max) {
                variance_max = v;
            }
            nb_internal_nodes++;
        }
    }

    double medvar = (nb_internal_nodes > 0) ? (total_variance / nb_internal_nodes) : 0.0;

    // calculer le seuil initial sigma
    double sigma = medvar / variance_max;

    filtrage(tree, 0, sigma, alpha); // commencer par la racine
}



/**
 * @brief Filtre un noeud et ses descendants dans le QuadTree.
 * 
 * Cette fonction parcourt récursivement les noeuds descendants d'un noeud donné
 * et applique un filtrage en fonction de leur variance. Les noeuds homogènes 
 * (dont tous les enfants répondent aux critères) sont marqués comme uniformes.
 * 
 * @param tree Le QuadTree à filtrer.
 * @param index L'index du noeud actuel dans le QuadTree.
 * @param sigma Le seuil de variance pour ce noeud.
 * @param alpha Le paramètre d'atténuation du seuil.
 * @return 1 si le noeud est marqué comme uniforme, sinon 0.
 */
static int filtrage(QuadTree *tree, int index, double sigma, double alpha) {
    QuadNode *node = &tree->nodes[index];
    int firstChildIndex = 4 * index + 1;

    if (node->u == 1 || firstChildIndex >= tree->total_nodes) {
        return 1;
    }

    int enfants[4] = {4 * index + 1, 4 * index + 2, 4 * index + 3, 4 * index + 4};

    int s = 0;
    for (int i = 0; i < 4; i++) {
        s += filtrage(tree, enfants[i], sigma * alpha, alpha);
    }

    if (s < 4 || node->v > sigma) return 0;
    node->u = 1;  
    node->epsilon = 0; 
    return 1;  

}
