#ifndef _TETE_H_
#define _TETE_H_

#include "arbre_huffman.h"
#include "gestion_fichiers.h"

/* ******************************Version 0****************************** */

/* Fonction qui écrit l'entete dans l'archive */
void ecrire_entete_aux(FILE* archive, noeud* alphabet[256]);

/* Récupère l'entete de l'archive */
void recuperer_entete(FILE* archive, int occurence[256]);

/* ********************************************************************* */

/* Fonction qui créer et ensuite écrit l'entete */
void ecrire_entete(file* liste_fichiers, FILE* archive, int nombre_fichiers, noeud* alphabet[256]);

/* Calcule toutes les occurences dans tous les fichiers/sous-fichiers à compresser */
void calculer_occurences_totales(file* liste_fichiers, int nombre_fichiers, int tab_occurence[256]);



#endif