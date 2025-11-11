# Projet : Compression d'Images Numériques avec QuadTree

## Description du Projet

Ce projet implémente un format de compression d'image numérique basé sur les QuadTrees. Il est structuré autour de deux fonctionnalités principales :
1. **Encodage** : Conversion d'un fichier PGM en un fichier QTC compressé.
2. **Décodage** : Reconstruction d'un fichier PGM à partir d'un fichier QTC.

Le projet est divisé en deux parties :
- **Bibliothèque partagée** : Contient les fonctions principales pour l'encodage, le décodage, et la manipulation des QuadTrees.
- **Application utilisateur** : Fournit une interface en ligne de commande simple pour utiliser la bibliothèque.

---

## Organisation du Projet

- **`app/`** : Répertoire pour l'application utilisateur.
  - `bin/` : Contient l'exécutable `codec`.
  - `doc/` : Documentation simplifiée destinée à l'utilisateur (html/latex).
  - `include/` : Contient uniquement `qtc.h`, l'interface publique de la bibliothèque.
  - `PGM/` et `QTC/` : Répertoires pour les fichiers pgm/qtc.
  - `src/main.c` : Fichier principal qui gère les options en ligne de commande.
  - `Doxyfile` : Permet de générer la doc.
  - `Makefile` : Permet de compiler l'exécutable `codec` en utilisant la bibliothèque partagée `libqtc.so`.

- **`lib/`** : Répertoire pour la bibliothèque partagée.
  - `obj/` : Contient les fichiers objets temporaires (ils seront supprimés une fois la bibliothèque es créée).
  - `doc/` : Documentation complète de toutes les fonctions publics.
  - `include/` : Tous les fichiers d'en-tête publics (`bitstream.h`, `quadtree.h`, etc.).
  - `src/` : Implémentation des fonctions principales.
  - `libqtc.so` : Bibliothèque partagée générée.
  - `Doxyfile` : Permet de générer la doc.
  - `Makefile` : Compile les fichiers et génère la bibliothèque.

---

## Compilation et Exécution

### Compilation

1. Compiler la bibliothèque partagée :
   ```
   cd lib
   make
   ```

2. Compiler l'application utilisateur :
   ```
   cd app
   make
   ```

### Utilisation
Se mettre dans le rep `app`
Exécuter l'application avec :
```bash
./bin/codec [OPTIONS]
```

#### Options Disponibles
- `-c` : Encode un fichier PGM en QTC.
- `-u` : Décode un fichier QTC en PGM.
- `-i` : Spécifie le fichier d'entrée.
- `-o` : Spécifie le fichier de sortie (en l'absence de cette option, le fichier sera sous le nom de out.qtc/pgm) (facultatif).
- `-a` : Paramètre alpha pour l'encodage avec perte (facultatif).
- `-g` : Génère une grille de segmentation (facultatif) dans PGM.
- `-v` : Active le mode verbeux.
- `-h` : Affiche l'aide.

#### Exemples
- **Encodage avec perte** :
   ```bash
   ./bin/codec -c -i PGM/input.pgm -o QTC/output.qtc -a 0.5 -g
   ```
- **Décodage** :
   ```bash
   ./bin/codec -u -i QTC/output.qtc -o PGM/output.pgm
   ```

---

## Difficultés Rencontrées

1. Gestion des commentaires (quelques fichiers où ça marchait, d'autre non mais j'ai reùssi à le faire fonctionner).
2. Gestion efficace de la mémoire car il fallait initialiser un buffer de maniere dynamique comme tout les fichiers ne sont pas de taille unique.

---

## Conclusion

Le projet est fonctionnel et respecte toutes les consignes. L'application `codec` est simple à utiliser et documentée pour faciliter son utilisation.
