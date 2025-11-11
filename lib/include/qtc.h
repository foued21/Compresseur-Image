#ifndef QTC_H
#define QTC_H

/**
 * @file qtc.h
* @brief Interface pour le codage des fichiers PGM en QTC et le décodage des fichiers QTC en PGM.
 *
 * Ce fichier contient les déclarations des fonctions principales pour 
 * encoder des images PGM en fichiers QTC et pour décoder des fichiers 
 * QTC en images PGM. Il gère également des options comme le mode verbeux 
 * et la génération de grilles de segmentation.
 */



/**
 * Fonction principale pour encoder un fichier PGM en QTC.
 * 
 * @param input_file Chemin du fichier d'entrée (PGM).
 * @param output_file Chemin du fichier de sortie (QTC).
 * @param alpha Paramètre pour l'encodage avec perte. Si <= 0, aucun filtrage.
 * @param generer_grille Générer une grille de segmentation (1 : oui, 0 : non).
 * @param verbose Activer le mode verbeux (1 : oui, 0 : non).
 * @return 0 en cas de succès, 1 sinon.
 */
int coder_fichier(const char *input_file, const char *output_file, double alpha, int generer_grille, int verbose);

/**
 * Fonction principale pour décoder un fichier QTC en PGM.
 * 
 * @param input_file Chemin du fichier d'entrée (QTC).
 * @param output_file Chemin du fichier de sortie (PGM).
 * @param generer_grille Générer une grille de segmentation (1 : oui, 0 : non).
 * @param verbose Activer le mode verbeux (1 : oui, 0 : non).
 * @return 0 en cas de succès, 1 sinon.
 */
int decoder_fichier(const char *input_file, const char *output_file, int generer_grille, int verbose);

#endif // QTC_H