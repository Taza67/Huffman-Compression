#ifndef _COMPRESSION_H_
#define _COMPRESSION_H_

#include "arbre_huffman.h"
#include "gestion_fichiers.h"

/* ******************************Version 0****************************** */

/* écrit le codage de huffman du fichier à compresser dans l'archive (réutilisée 
   dans toutes les versions) */
void ecrire_codage(FILE* fichier, FILE* archive, noeud* alphabet[256]);

/* compresse un fichier dont le nom a été donné dans une archive dont le nom a 
   aussi été donné (Pour la premiere version) */
void compression_fichierV0(char *nom_archive, char *nom_fichier);

/* ********************************************************************* */

/* ******************************Version 1****************************** */

/* compresse un fichier qui a été donné sous forme de structure (voir gestion_fichier.h
   dans une archive déjà ouverte (écriture de plusieurs fichiers les uns à la suite
   des autres) */
void compression_fichier(FILE *archive, file struct_fichier, noeud *alphabet[256]);

/* compresse une liste de fichiers (struct "file") dans le nombre a été donné dans 
   une archive (Il s'agit d'une amélioration de compression_fichierV0 pour la seconde
   version) */
void compressionV1(file * liste_fichiers, int nombre_fichiers, char *nom_archive);

/* ********************************************************************* */

/* ******************************Version 2****************************** */

/* compresse les fichiers (pas les dossiers) contenus dans un dossier dans une archive donnée */
void compression_dossierV2(file * liste_fichiers, int nombre_fichiers, FILE *archive, noeud *alphabet[256]);

/* compresse une liste de fichiers dans une archive (Ici il n'y aura qu'un dossier car on souhaite 
   compresser uniquement UNE arborescence */
void compressionV2(file * liste_fichiers, int nombre_fichiers, char *nom_archive);

/* ********************************************************************* */

/* ******************************Version 3-4**************************** */

/* compresse une liste de fichiers ou dossiers contenus dans un dossier dans une archive */
void compression_dossier(file * liste_fichiers, int nombre_fichiers, FILE *archive, noeud *alphabet[256]);

/* compresse une liste de fichiers ou dossiers (arborescences) dans une archive */
void compression(file * liste_fichiers, int nombre_fichiers, char *nom_archive);

/* ********************************************************************* */

#endif