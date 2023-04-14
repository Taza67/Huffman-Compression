#ifndef _ARBRE_HUFFMAN_H_
#define _ARBRE_HUFFMAN_H_

/* ********************Structure huffman****************** */

typedef struct struct_noeud {
    int initial;                /* Caractere */
    int occurence;
    int code;                   /* code binaire */
    int bits;                   /* Nombre de bits du code */
    struct struct_noeud * gauche; /* fils gauche */
    struct struct_noeud * droit;  /* fils droit */
} noeud;

/* ******************************************************* */

/* Compte le nombre d'occurence de chaque caractère d'un fichier
   et les mets dans le tableau tab */
void occurence(char *nom_fichier, int tab[256]);

/* Calcule la taille du fichier, nombre de caractères */
int total_occurence_fichier(char *nom_fichier);

/* Crée et renvoie une feuille */
noeud * creer_feuille(int* tab, int index);

/* Crée les noeuds de l'arbre */
void creer_tous_noeuds(noeud *arbre_huffman[], int occurence[256], int *nbr_char, int *taille_fichier);

/* Recherche les deux noeuds avec les plus petites occurences */
int chercher2petits(noeud * arbre_huffman[], int * first, int * second);

/* Crée l'arbre de huffman */
void creer_noeud(noeud * arbre_huffman[], int taille);

/* Crée les codes binaires des feuilles de l'arbre */
void creer_code(noeud* element, int code, int profondeur, noeud* alphabet[256]);

/* Affiche le code des noeuds en binaire */
void affichage_code(int nbr_bits, int codage);

/* ********************Affichage************************** */

/* Calcule la profondeur de l'arbre */
int calculer_profondeur_arbre(noeud* a);

/* Fonction auxiliaire de afficher_arbre */
void afficher_arbre_aux(noeud *a, int prof);

/* Fonction qui affiche l'arbre sur le terminal */
void afficher_arbre(noeud *a );

/* Fonction qui dessine l'arbre dans une fenetre MLV */
void dessiner_arbre(noeud *a, int x, int y, int ecart, int hauteur);

/* Fonction qui dessine un noeud */
void dessiner_noeud(noeud *a, int x, int y);

/* affiche l'arbre de huffman en MLV */
void affiche_arbre_graphique(noeud *a);

/* ******************************************************* */

#endif