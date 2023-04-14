# Huffman Compressor

## Créateurs

- **Nom(s) :** AKIL, IGHIT
- **Prénom(s) :** Mourtaza, Tarik
- **Projet :** Huffman Compressor
- **Dernière modification :** <!-- dernière date de modification -->

## Programmes

### Liste des programmes

- ***huffman-compressor***

### Descriptif des programmes

- ***huffman-compressor*** :

  Le programme utilise la méthode de huffman pour compresser les fichiers. Il est conçu pour créer / décompresser une archive de fichier(s) / dossier(s).

#### Points à noter

- Éviter les **fichiers de petite taille** car les caractéristiques ou format de l'archive nécessitent une taille minimale qui pourrait éventuellement être plus grande que celle du/des fichier(s) de départ.

- Le programme ne peut traiter que des fichiers contenant des caractères de la table **ASCII (128)**.

- Il est important de **ne pas modifier l'archive** car on risque de perdre son contenu => (reconstitution/décompression impossible).

- L'archive doit respecter un **format spécifique** au programme pour pouvoir être décompressée, une archive réalisée par un autre programme ne fonctionnera pas.

## Makefile

- `make` : exécute **make build**
- `make build` : permet de compiler le programme
- `make clean` : permet de supprimer tous les fichiers objets, l'exécutable et les éventuels fichiers résiduels
- `make rebuild` : permet de recompiler le programme avec un nettoyage préalable = **make clean + make build**

## Exécution

    huffman-compressor [-option] [nom_archive] [[fichier(s)/dossier(s) (...)] || [cible_decomprssion]]

Options possibles :

    - huffman-compressor -c [nom_archive] [[fichier(s)/dossier(s)] ...] 
      // Compression

    - huffman-compressor -d [nom_archive] 
      // Décompression

    - huffman-compressor -d [nom_archive] [cible_decompression] 
      // Décompression vers cible

    - huffman-compressor -h 
      // Affichage de la notice d'utilisation
