#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "allocation.h"
#include "arbre_huffman.h"
#include "gestion_fichiers.h"
#include "compression.h"
#include "en_tete.h"

/* ******************************Version 0****************************** */

void ecrire_entete_aux(FILE* archive, noeud* alphabet[256]) {
    /* *Déclaration des variables */
    int i;
    /* *Vérification de l'archive */
    if (archive == NULL) {
        fprintf(stderr, "- Erreur -> fonction ecrire_entete(FILE *archive, noeud *alphabet[256]) : archive = NULL !\n");
        exit(EXIT_FAILURE);
    }
    /* *Parcours du tableau alphabet */
    for(i = 0; i < 256; i++) {
        /* **On ne prend en compte que les noeuds associés à des caractères du fichier */
        if (alphabet[i] != NULL) {
            /* ***On écrit d'abord le caractère => i = ASCII(caractere) */
            fwrite(&i, sizeof(char), 1, archive);
            /* ***On écrit ensuite le nombre d'occurences */
            fwrite(&(alphabet[i]->occurence), sizeof(int), 1, archive);
        }
    }
    /* *Le caractère '\0' marque la fin de l'entete et le début du contenu codé */
    fputc('\0', archive);
} 

void recuperer_entete(FILE* archive, int occurence[256]) {
    /* *Déclaration des variables */
    int caractere;
    do {
        /* *On récupère le premier caractère */
        caractere = fgetc(archive);
        /* *Si ce n'est pas la fin de l'entete ('\0') ou la fin du fichier (EOF) */
        if (caractere != '\0' && caractere != EOF) {
            /* **On récupère le nombre d'occurences du caractères */
            if (fread(&(occurence[caractere]), sizeof(int), 1, archive) != 1)
                fprintf(stderr, "- Erreur recuperer_entete(FILE* archive, int occurence[256]) : recupération de l'occurence échouée !\n");
        }
    } while (caractere != '\0' && caractere != EOF);
}

/* ********************************************************************* */

void ecrire_entete(file* liste_fichiers, FILE* archive, int nombre_fichiers, noeud* alphabet[256]) {
    /* *Déclaration de variables */
    int tab_occurence[256], /* Stocke les nombres d'occurences dans le fichier par caractere */
        nbr_char = 0, /* Contient le nombre de caractères disctints des fichiers */
        taille_fichier = 0, /* contient le nombre de caractères disctints ou pas du fichier */
        i = 0;
    noeud* arbre_huffman[256];
    /* *Initialisation des variables */
    for(i = 0;i < 256; i++) {
        alphabet[i] = NULL;
        tab_occurence[i] = 0;
        arbre_huffman[i] = NULL;
    }
    /* *Calcul des occurences totales dans l'intégralité des fichiers, des sous-fichiers de dossiers, etc */
    calculer_occurences_totales(liste_fichiers, nombre_fichiers, tab_occurence);
    /* *Création des noeuds */
    creer_tous_noeuds(arbre_huffman, tab_occurence, &nbr_char, &taille_fichier);
    /* *Création de l'arbre */
    creer_noeud(arbre_huffman, nbr_char);
    /* *Création des codes */
    creer_code(*arbre_huffman, 0, 0, alphabet);
    /* *Ecriture de l'entete */
    ecrire_entete_aux(archive, alphabet);  
}

void calculer_occurences_totales(file* liste_fichiers, int nombre_fichiers, int tab_occurence[256]) {
    /* *Déclaration de variables */
    int i = 0;
    /* *Parcours de la liste des fichiers */
    for (i = 0; i < nombre_fichiers; i++) {
        /* **Si c'est un fichier */
        if (liste_fichiers[i].type == 'f')
            /* ***Comptage des occurences du fichier */
            occurence(liste_fichiers[i].nom, tab_occurence);
        /* **Si c'est un dossier */
        else if (liste_fichiers[i].type == 'd') {
            /* ****On récupère le nombre de fichiers dans le dossier */
            int nombre_fichiers_dossier = renvoyer_nombre_fichiers_dossier(liste_fichiers[i].nom);
            /* ****On récupère la liste des fichiers/dossiers enfants */
            file * liste_fichiers_dossier = lister_fichiers_dossier(liste_fichiers[i].nom);
            /* ****On rappelle la fonction sur ces fichiers là */
            calculer_occurences_totales(liste_fichiers_dossier, nombre_fichiers_dossier, tab_occurence);
        }
    }
}

