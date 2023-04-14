#ifndef _DECOMPRESSION_H_
#define _DECOMPRESSION_H_

#include "arbre_huffman.h"

/* ******************************Version 0****************************** */

/* Récupère le contenu du fichier d'origine à partir du codage binaire dans l'archive */
void recuperer_contenu(FILE* archive, FILE* fichier, noeud* arbre_huffman[], int taille_fichier);

/* Renvoie une feuille dès qu'un code correspond au sien */
noeud * recuperer_feuille(noeud *racine, int paquet, int taille_paquet, int profondeur);

/* Décompresse un fichier dans un dossier cible qui est par défaut le dossier local */
void decompression_fichierV0(char * nom_archive, char *dossier_cible);

/* ********************************************************************* */

/* ******************************Version 1****************************** */

/* Décompresse un fichier à partir d'une archive */
void decompression_fichier(FILE *archive, char * nom_fichier, noeud* arbre_huffman[256]);

/* Décompresse une archive qui contient une liste de fichiers contenus dans l'archive */
void decompressionV1(char *nom_archive, char *dossier_cible);

/* ********************************************************************* */

/* ******************************Version 2****************************** */

/* Décompresse un dossier contenu dans une archive */
void decompressionV2(char *nom_archive, char *dossier_cible);

/* Décompresse la liste des sous-fichiers du dossier contenu dans l'archive */
void decompression_dossierV2(FILE *archive, noeud* arbre_huffman[256], char *dossier_cible);

/* ********************************************************************* */

/* ******************************Version 3-4**************************** */

/* Décompresse une liste de fichiers/dossiers contenus dans une archive */
void decompression(char *nom_archive, char *dossier_cible);

/* Décompresse la liste des sous-fichiers ou dossiers d'un dossier contenu dans l'archive */
void decompression_dossier(FILE *archive, noeud* arbre_huffman[256], char *dossier_cible);

/* ********************************************************************* */

#endif