#include <stdio.h>
#include <stdlib.h>
#include <MLV/MLV_all.h>
#include "allocation.h"
#include "arbre_huffman.h"

void occurence(char *nom_fichier, int tab[256]) {
    /* *Déclaration de variables */
    int caractere = 0;
    FILE *fichier = NULL;
    /* *Ouverture du fichier */
    if ((fichier = fopen(nom_fichier, "r")) == NULL) {
        fprintf(stderr, "- Erreur -> fonction occurence(char *nom_fichier, int tab[256]) : ouverture du fichier %s impossible !\n", nom_fichier);
        exit(EXIT_FAILURE);
    } else {
        /* *Parcours du fichier caractère par caractère + incrémentation occurence */
        while ((caractere = fgetc(fichier)) != EOF) {
            tab[caractere]++;
        }
    }
    /* *Libération de la mémoire */
    /* **Fichiers */
    fclose(fichier);
}

int total_occurence_fichier(char *nom_fichier) {
    /* *Déclaration de variables */
    int total_caracteres = 0;
    FILE *fichier = NULL;
    /* *Ouverture du fichier */
    if ((fichier = fopen(nom_fichier, "r")) == NULL) {
        fprintf(stderr, "- Erreur -> fonction total_occurence_fichier(char *nom_fichier) : ouverture du fichier %s impossible !\n", nom_fichier);
        exit(EXIT_FAILURE);
    } else {
        /* *Parcours du fichier */
        while (fgetc(fichier) != EOF) total_caracteres++;
    }
    /* *Libération de la mémoire */
    /* **Fichiers */
    fclose(fichier);
    /* *Retour */
    return total_caracteres;
}

noeud * creer_feuille(int* tab, int index) {
    /* *Déclaration de variables + allocation */
    noeud* no = (noeud*)allocation_mem_init0(1, sizeof(noeud));
    /* *Insertion données */
    no->initial = index;
    no->occurence = tab[index];
    /* *Retour */
    return no;
}

void creer_tous_noeuds(noeud *arbre_huffman[], int occurence[256], int *nbr_char, int *taille_fichier) {
    /* *Déclaration des variables */
    int i = 0,
        local_nbr_char = 0,
        local_taille_fichier = 0;
    /* *Parcours des 256 caractères */
    for (; i < 256; i++) {
        if (occurence[i] != 0) {
            arbre_huffman[i] = creer_feuille(occurence, i);
            local_nbr_char++;
            local_taille_fichier += occurence[i];
        }
    }
    /* *Affectation des variables */
    *nbr_char = local_nbr_char;
    *taille_fichier = local_taille_fichier;
}

int chercher2petits(noeud * arbre_huffman[], int * first, int * second) {
    /* *Déclaration de variables */
    int i;
    /* *Initialisations */
    *first = *second = -1;
    /* *Parcours pour récupérer les indices des noeuds avec les deux plus petites occurences */
    for (i = 0; i < 256; i++) {
        /* **On ne prend en compte que les noeuds associés à des caractères du fichier */
        if (arbre_huffman[i] != NULL) {
            /* ***Initialisation du noeud first */
            if (*first == -1) *first = i;
            /* ***Initialisation du noeud second */
            else if (*second == -1) *second = i;
            /* ***Recherche des deux plus petites occurences */
            else if (arbre_huffman[i]->occurence < arbre_huffman[*first]->occurence) {
                *first = i;
            } else if (arbre_huffman[i]->occurence <= arbre_huffman[*second]->occurence)
                *second = i;
        }
    }
    /* *Retour => Si on a trouvé les DEUX noeuds alors retour = 1 sinon retour = 0 */
    return (*first != -1 && *second != -1) ? 1 : 0;
}


void creer_noeud(noeud * arbre_huffman[], int taille) {
    /* *Déclaration de variables */
    int t = taille, new_occurence, first, second;
    noeud * tmp = NULL;
    /* *On boucle jusqu'à n'avoir plus qu'un seul noeud */
    while (t > 1) {
        /* **On cherche les deux noeuds aux plus petites occurences */
        chercher2petits(arbre_huffman, &first, &second);
        /* **On stocke le premier dans une variable temporaire */
        tmp = arbre_huffman[first];
        /* **On le rédifinit en lui allouant de la mémoire -> On souhaite garder tmp */
        arbre_huffman[first] = (noeud*)allocation_mem_init0(1, sizeof(noeud));
        /* **On lui donne une nouvelle occurence -> somme des deux noeuds trouvés */
        new_occurence = tmp->occurence + arbre_huffman[second]->occurence;
        arbre_huffman[first]->occurence = new_occurence;
        /* **On lui donne comme fils droit, le second noeud et comme fils gauche, le premier qui est dans tmp */
        arbre_huffman[first]->gauche = tmp;
        arbre_huffman[first]->droit = arbre_huffman[second];
        /* **On enlève le second noeud du tableau => algo de huffman 
           Le premier ayant été enlevé par l'allocation */
        arbre_huffman[second] = NULL;
        /* **On passe aux prochains sommets */
        t--;
    }
    /* **Le sommet restant est la racine de arbre_huffman ou le premier noeud */
    if (taille > 1) 
        *arbre_huffman = arbre_huffman[first];
}

void creer_code(noeud* element, int code, int profondeur, noeud* alphabet[256]) {
    if (element == NULL) return;
    /* *Si on est sur une feuille */
    if (element->droit == NULL && element->gauche == NULL) {
        /* **Il y a autant de bits que la profondeur de la feuille */
        element->bits = profondeur;
        /* **Le code est "calculé" avant chaque appel à la fonction */
        element->code = code;
        /* **On affecte le noeud au caractère adéquat (indice = ASCII(caractère)) dans la table alphabet */
        alphabet[(int)element->initial] = element;
    } else {
        /* *Sinon on est sur un noeud */
        /* **On descend dans l'arbre pour "travailler" avec les fils */
        profondeur++;
        /* **On injecte un 1 dans le code pour le fils droit s'il y en a un */
        if (element->droit != NULL) {
            int new_code = (code << 1) + 1;
            creer_code(element->droit, new_code, profondeur, alphabet);
        }
        /* **On injecte un 0 dans le code pour le fils gauche s'il y en a un */
        if (element->gauche != NULL) {
            int new_code = (code << 1);
            creer_code(element->gauche, new_code, profondeur, alphabet);
        }
    }
}

void affichage_code(int nbr_bits, int codage) {
    /* *Déclaration des variables */
    /* **Tableau qui servira à stocker la "forme" binaire du codage décimal */
    int * binaire = (int*)allocation_mem_init0(nbr_bits, sizeof(int));
    int i, *p;
    /* *Algo de conversion de base */
    for (p = binaire; codage > 0; p++) {
        *p = codage % 2;
        codage = codage / 2;
    }
    /* *Affichage du codage */
    for (i = nbr_bits - 1; i >= 0; i--) printf("%d", binaire[i]);
    printf("\n");
    /* *Libération de la mémoire */
    free(binaire);
}

int calculer_profondeur_arbre(noeud* a) {
    int hauteur_max = 1;
    if (a == NULL) return 0;
    if (a->gauche != NULL) {
        int tmp = 1;
        tmp += calculer_profondeur_arbre(a->gauche);
        if (tmp > hauteur_max) hauteur_max = tmp;
    }
    if (a->droit != NULL) {
        int tmp = 1;
        tmp += calculer_profondeur_arbre(a->droit);
        if (tmp > hauteur_max) hauteur_max = tmp;
    }
    return hauteur_max;
}

void afficher_arbre_aux(noeud *a, int prof) {
    int i;
    for (i = 0; i < prof; printf("|"), i++ );
    printf( "%c -> ", a->initial);
    affichage_code(a->bits, a->code);
    if (a->gauche != NULL) afficher_arbre_aux(a->gauche, prof + 1);
    if (a->droit != NULL) afficher_arbre_aux( a->droit, prof + 1);
}

void afficher_arbre(noeud *a ) {
    afficher_arbre_aux(a, 0);
}

void dessiner_arbre(noeud *a, int x, int y, int ecart, int hauteur) {
    dessiner_noeud(a, x, y);
    if (a->gauche != NULL) {
        MLV_draw_line(x, y + 30, x - ecart, y + hauteur + 30, MLV_COLOR_WHITE);
        dessiner_arbre(a->gauche, x - ecart, y + hauteur + 30, ecart / 2, hauteur * 1.5);
    }
    if (a->droit != NULL) {
        MLV_draw_line(x + 30, y + 30, x + ecart + 30, y + hauteur + 30, MLV_COLOR_WHITE);
        dessiner_arbre(a->droit, x + ecart, y + hauteur + 30, ecart / 2, hauteur * 1.5);
    }
        
}

void dessiner_noeud(noeud *a, int x, int y) {
    char t[3];
    t[0] = '\'';
    t[2] = '\'';
    t[1] = a->initial;
    MLV_draw_text_box(x, y, 30, 30, t, 9, MLV_COLOR_WHITE, MLV_COLOR_WHITE, MLV_COLOR_BLACK,
                      MLV_TEXT_CENTER, MLV_HORIZONTAL_CENTER, MLV_VERTICAL_CENTER);
    MLV_actualise_window();
}

void affiche_arbre_graphique(noeud *a) {
    int profondeur = calculer_profondeur_arbre(a),
        largeur = profondeur * 250;
    MLV_create_window("Arbre", "Arbre", largeur, 1000);
    dessiner_arbre(a, largeur / 2, 50, 50 * profondeur, 15);
    MLV_wait_seconds(10);
    MLV_free_window();
}