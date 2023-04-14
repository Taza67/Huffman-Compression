#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "allocation.h"
#include "en_tete.h"
#include "compression.h"

/* ******************************Version 0****************************** */

/* ecrit le codage de huffman du fichier à compresser dans l'archive */
void ecrire_codage(FILE* fichier, FILE* archive, noeud* alphabet[256]) {
    /* *Déclaration des variables */
    int paquet = 0, /* **paquet = suite de bits => paquet de bits */
        taille_paquet = 0; /* **taille_paquet = nombre de bits dans le paquet */
    int caractere, /* **caractere dans le fichier */
        codage; /* **codage à écrire dans le fichier */
    /* *Vérification des fichiers */
    if (archive == NULL) {
        fprintf(stderr, "- Erreur -> fonction ecrire_codage(FILE *fichier, FILE *archive, noeud *alphabet[256]) : archive = NULL !\n");
        exit(EXIT_FAILURE);
    }
    if (fichier == NULL) {
        fprintf(stderr, "- Erreur -> fonction ecrire_codage(FILE *fichier, FILE *archive, noeud *alphabet[256]) : fichier = NULL !\n");
        exit(EXIT_FAILURE);
    }
    /* *Retour au début dans le fichier à compresser */
    fseek(fichier, 0, SEEK_SET);
    /* *Parcours du fichier */
    while((caractere = fgetc(fichier)) != EOF) {
        /* **Ajout d'autant de 0 (bits) dans le paquet que la taille du code associé au caractère */
        paquet = paquet << alphabet[caractere]->bits;
        /* **L'opérateur | permet d'assigner les 1 au bon endroit en fonction du code
           =>Remplacement des 0 (bits) ajoutés juste avant par les bits du code du caractère */
        paquet = paquet | alphabet[caractere]->code;
        /* **Ajout de bits => La taille du paquet a augmenté */
        taille_paquet += alphabet[caractere]->bits;
        /* **Une fois qu'on a plus de 8 bits (1 octet) => On peut écrire sur l'archive */
        while(taille_paquet >= 8) {
            /* ***On récupère les 8 bits de poids faible => taille_paquet - 8 */
            taille_paquet -= 8;
            codage = paquet >> taille_paquet;
            /* ***On ajoute le codage de 8 bits */
            fputc(codage, archive);
        }
    }
    /* **A la fin, il restera peut-être des bits qui ne forment pas forcément un octet */
    if (taille_paquet > 0) {
        /* ***On ajoute autant de bits (0) qu'il faut pour former un octet */
        paquet = paquet << (8 - taille_paquet);
        /* ***On ajoute le dernier paquet dans l'archive */
        fputc(paquet, archive);
    }
    fputc('\0', archive);
}

/* compresse un fichier dans une archive */
void compression_fichierV0(char *nom_archive, char *nom_fichier) {
    /* *Déclaration des variables */
    FILE *fichier = NULL, *archive = NULL;
    int tab_occurence[256], /* Stocke les nombres d'occurences dans le fichier par caractere */
        nbr_char = 0, /* Contient le nombre de caractères disctints du fichier */
        taille_fichier = 0, /* contient le nombre de caractères disctints ou pas du fichier */
        i = 0;
    noeud *alphabet[256], /* Stocke les pointeurs vers les noeuds associés à chaque caractère */
        *arbre_huffman[256]; /* Contient l'arbre de huffman associé au fichier */
    /* *Initialisations des variables */
    for (i = 0; i < 256; i++) {
        tab_occurence[i] = 0;
        alphabet[i] = NULL;
        arbre_huffman[i] = NULL;
    }
    /* *Ouverture des fichiers */
    if ((fichier = fopen(nom_fichier, "r")) == NULL) {
        fprintf(stderr, "- Erreur -> fonction compression_fichierV0(char* nom_archive, char* nom_fichier) : ouverture du fichier %s impossible !\n", nom_fichier);
        exit(EXIT_FAILURE);
    }
    if ((archive = fopen(nom_archive, "wb")) == NULL) {
        fprintf(stderr, "- Erreur -> fonction compression_fichierV0(char* nom_archive, char* nom_fichier) : ouverture de l'archive %s impossible !\n", nom_archive);
        exit(EXIT_FAILURE);
    }
    /* ***Nom du fichier */
    fwrite(nom_fichier, sizeof(char), strlen(nom_fichier), archive);
    /* ***Séparateur */
    fputc('\0', archive);
    /* *Récupération des occurences */
    occurence(nom_fichier, tab_occurence);
    /* *Création des noeuds */
    creer_tous_noeuds(arbre_huffman, tab_occurence, &nbr_char, &taille_fichier);
    /* *Création de l'arbre */
    creer_noeud(arbre_huffman, nbr_char);
    /* *Création des codes */
    creer_code(*arbre_huffman, 0, 0, alphabet);
    /* *Ecriture de l'entete */
    ecrire_entete_aux(archive, alphabet);
    /* *Ecriture du codage */
    ecrire_codage(fichier, archive, alphabet);
    /* *Libération de la mémoire */
    /* **Noeuds */
    for (i = 0; i < 256; i++)
        if (alphabet[i] != NULL) libere(alphabet[i]);
    /* **Fichiers */
    fclose(fichier);
    fclose(archive);
}

/* ********************************************************************* */

/* ******************************Version 1****************************** */

void compression_fichier(FILE *archive, file struct_fichier, noeud *alphabet[256]) {
    /* *Déclaration des variables */
    FILE *fichier = NULL;
    /* *Ouverture des fichiers */
    if ((fichier = fopen(struct_fichier.nom, "r")) == NULL) {
        fprintf(stderr, "- Erreur -> fonction compression_fichier(FILE* archive, char* nom_fichier) : ouverture du fichier %s impossible !\n", struct_fichier.nom);
        exit(EXIT_FAILURE);
    }
    /* *Vérification de l'archive */
    if (archive == NULL) {
        fprintf(stderr, "- Erreur -> fonction compression_fichier(FILE* archive, char* nom_fichier) : archive = NULL !\n");
        exit(EXIT_FAILURE);
    }
    /* *Ecriture du codage */
    ecrire_codage(fichier, archive, alphabet);
    /* *Libération de la mémoire */
    /* **Fichiers */
    fclose(fichier);
}

void compressionV1(file * liste_fichiers, int nombre_fichiers, char *nom_archive) {
    /* *Déclaration des variables */
    FILE *archive = NULL;
    int i = 0;
    noeud *alphabet[256]; /* **Stocke les pointeurs vers les noeuds associés à chaque caractère */
    /* *Ouverture de l'archive */
    if ((archive = fopen(nom_archive, "wb")) == NULL) {
        fprintf(stderr, "- Erreur -> fonction compressionV0(file * liste_fichiers, int nombre_fichiers, char * nom_archive) : ouverture de l'archive %s impossible !\n", nom_archive);
        exit(EXIT_FAILURE);
    }
    /* *Création + Ecriture de l'entete */
    ecrire_entete(liste_fichiers, archive, nombre_fichiers, alphabet);
    /* *Première valeur de l'archive après entete = nombre de fichiers */
    fwrite(&(nombre_fichiers), sizeof(int), 1, archive);
    /* *Parcours de la liste de fichiers */
    for (i = 0; i < nombre_fichiers; i++) {
        /* **Début de la compression d'un fichier */
        /* ***Caractère qui indique si c'est un fichier ou un dossier */
        fputc(liste_fichiers[i].type, archive);
        /* ***Séparateur */
        fputc('\0', archive);
        /* ***Nom du fichier ou du dossier */
        fwrite(liste_fichiers[i].nom, sizeof(char), strlen(liste_fichiers[i].nom), archive);
        /* ***Séparateur */
        fputc('\0', archive);
        /* ***Si c'est un dossier, on compresse d'abord ses fichiers */
        if (liste_fichiers[i].type == 'f') {
            /* ***Si c'est un fichier, alors il aura une taille (nombre de caractères) */
            liste_fichiers[i].taille = total_occurence_fichier(liste_fichiers[i].nom);
            /* ****On récupère la liste des fichiers/dossiers enfants */
            fwrite(&(liste_fichiers[i].taille), sizeof(int), 1, archive);
            /* ***Séparateur */
            fputc('\0', archive);
            /* ***Compression du fichier */
            compression_fichier(archive, liste_fichiers[i], alphabet);
        }
        /* **Fin de la compression du fichier ou dossier */
    }
    /* *Libération de la mémoire */
    /* **Tableau alphabet */
    for (i = 0; i < 256; i++)
        if (alphabet[i] != NULL) libere(alphabet[i]);
    /* **Fichiers */
    fclose(archive);
}

/* ********************************************************************* */

/* ******************************Version 2****************************** */

void compression_dossierV2(file * liste_fichiers, int nombre_fichiers, FILE *archive, noeud *alphabet[256]) {
    /* *Déclaration des variables */
    int i = 0;
    /* *Vérification de l'archive */
    if (archive == NULL) {
        fprintf(stderr, "- Erreur -> fonction compression_dossier(file * liste_fichiers, int nombre_fichiers, FILE *archive) : archive = NULL !\n");
        exit(EXIT_FAILURE);
    }
    /* *Première valeur de la partie du dossier = nombre de fichiers */
    fwrite(&(nombre_fichiers), sizeof(int), 1, archive);
    /* *Parcours de la liste de fichiers */
    for (i = 0; i < nombre_fichiers; i++) {
        /* **Début de la compression d'un fichier */
        /* ***Caractère qui indique si c'est un fichier ou un dossier */
        fputc(liste_fichiers[i].type, archive);
        /* ***Séparateur */
        fputc('\0', archive);
        /* ***Nom du fichier */
        fwrite(liste_fichiers[i].nom, sizeof(char), strlen(liste_fichiers[i].nom), archive);
        /* ***Séparateur */
        fputc('\0', archive);
        /* ***Si c'est un fichier, alors il aura une taille (nombre de caractères) */
        liste_fichiers[i].taille = total_occurence_fichier(liste_fichiers[i].nom);
        fwrite(&(liste_fichiers[i].taille), sizeof(int), 1, archive);
        /* ***Séparateur */
        fputc('\0', archive);
        /* ***Compression du fichier */
        compression_fichier(archive, liste_fichiers[i], alphabet);
    }
}

void compressionV2(file * liste_fichiers, int nombre_fichiers, char *nom_archive) {
    /* *Déclaration des variables */
    FILE *archive = NULL;
    int i = 0;
    noeud *alphabet[256]; /* **Stocke les pointeurs vers les noeuds associés à chaque caractère */
    /* *Ouverture de l'archive */
    if ((archive = fopen(nom_archive, "wb")) == NULL) {
        fprintf(stderr, "- Erreur -> fonction compression(file * liste_fichiers, int nombre_fichiers, char * nom_archive) : ouverture de l'archive %s impossible !\n", nom_archive);
        exit(EXIT_FAILURE);
    }
    /* *Création + Ecriture de l'entete */
    ecrire_entete(liste_fichiers, archive, nombre_fichiers, alphabet);
    /* *Première valeur de l'archive après entete = nombre de fichiers */
    fwrite(&(nombre_fichiers), sizeof(int), 1, archive);
    /* *Parcours de la liste de fichiers */
    for (i = 0; i < nombre_fichiers; i++) {
        int nombre_fichiers_dossier = 0;
        file * liste_fichiers_dossier = NULL;
        /* **Début de la compression d'un fichier */
        /* ***Caractère qui indique si c'est un fichier ou un dossier */
        fputc(liste_fichiers[i].type, archive);
        /* ***Séparateur */
        fputc('\0', archive);
        /* ***Nom du fichier ou du dossier */
        fwrite(liste_fichiers[i].nom, sizeof(char), strlen(liste_fichiers[i].nom), archive);
        /* ***Séparateur */
        fputc('\0', archive);
        /* ****On récupère le nombre de fichiers dans le dossier */
        nombre_fichiers_dossier = renvoyer_nombre_fichiers_dossier(liste_fichiers[i].nom);
        liste_fichiers_dossier = lister_fichiers_dossier(liste_fichiers[i].nom);
        compression_dossier(liste_fichiers_dossier, nombre_fichiers_dossier, archive, alphabet);
    }
    /* *Libération de la mémoire */
    /* **Tableau alphabet */
    for (i = 0; i < 256; i++)
        if (alphabet[i] != NULL) libere(alphabet[i]);
    /* **Fichiers */
    fclose(archive);
}


/* ********************************************************************* */

/* ******************************Version 3****************************** */




void compression_dossier(file * liste_fichiers, int nombre_fichiers, FILE *archive, noeud *alphabet[256]) {
    /* *Déclaration des variables */
    int i = 0;
    /* *Vérification de l'archive */
    if (archive == NULL) {
        fprintf(stderr, "- Erreur -> fonction compression_dossier(file * liste_fichiers, int nombre_fichiers, FILE *archive) : archive = NULL !\n");
        exit(EXIT_FAILURE);
    }
    /* *Première valeur de la partie du dossier = nombre de fichiers */
    fwrite(&(nombre_fichiers), sizeof(int), 1, archive);
    /* *Parcours de la liste de fichiers */
    for (i = 0; i < nombre_fichiers; i++) {
        /* **Début de la compression d'un fichier */
        /* ***Caractère qui indique si c'est un fichier ou un dossier */
        fputc(liste_fichiers[i].type, archive);
        /* ***Séparateur */
        fputc('\0', archive);
        /* ***Nom du fichier ou du dossier */
        fwrite(liste_fichiers[i].nom, sizeof(char), strlen(liste_fichiers[i].nom), archive);
        /* ***Séparateur */
        fputc('\0', archive);
        /* ***Si c'est un dossier, on compresse d'abord ses fichiers */
        if (liste_fichiers[i].type == 'd') {
            /* ****On récupère le nombre de fichiers dans le dossier */
            int nombre_fichiers_dossier = renvoyer_nombre_fichiers_dossier(liste_fichiers[i].nom);
            /* ****On récupère la liste des fichiers/dossiers enfants */
            file * liste_fichiers_dossier = lister_fichiers_dossier(liste_fichiers[i].nom);
            compression_dossier(liste_fichiers_dossier, nombre_fichiers_dossier, archive, alphabet);
        } else {
            /* ***Si c'est un fichier, alors il aura une taille (nombre de caractères) */
            liste_fichiers[i].taille = total_occurence_fichier(liste_fichiers[i].nom);
            fwrite(&(liste_fichiers[i].taille), sizeof(int), 1, archive);
            /* ***Séparateur */
            fputc('\0', archive);
            /* ***Compression du fichier */
            compression_fichier(archive, liste_fichiers[i], alphabet);
        }
        /* **Fin de la compression du fichier ou dossier */
    }
}

void compression(file * liste_fichiers, int nombre_fichiers, char *nom_archive) {
    /* *Déclaration des variables */
    FILE *archive = NULL;
    int i = 0;
    noeud *alphabet[256]; /* **Stocke les pointeurs vers les noeuds associés à chaque caractère */
    /* *Ouverture de l'archive */
    if ((archive = fopen(nom_archive, "wb")) == NULL) {
        fprintf(stderr, "- Erreur -> fonction compression(file * liste_fichiers, int nombre_fichiers, char * nom_archive) : ouverture de l'archive %s impossible !\n", nom_archive);
        exit(EXIT_FAILURE);
    }
    /* *Création + Ecriture de l'entete */
    ecrire_entete(liste_fichiers, archive, nombre_fichiers, alphabet);
    /* *Première valeur de l'archive après entete = nombre de fichiers */
    fwrite(&(nombre_fichiers), sizeof(int), 1, archive);
    /* *Parcours de la liste de fichiers */
    for (i = 0; i < nombre_fichiers; i++) {
        /* **Début de la compression d'un fichier */
        /* ***Caractère qui indique si c'est un fichier ou un dossier */
        fputc(liste_fichiers[i].type, archive);
        /* ***Séparateur */
        fputc('\0', archive);
        /* ***Nom du fichier ou du dossier */
        fwrite(liste_fichiers[i].nom, sizeof(char), strlen(liste_fichiers[i].nom), archive);
        /* ***Séparateur */
        fputc('\0', archive);
        /* ***Si c'est un dossier, on compresse d'abord ses fichiers */
        if (liste_fichiers[i].type == 'd') {
            /* ****On récupère le nombre de fichiers dans le dossier */
            int nombre_fichiers_dossier = renvoyer_nombre_fichiers_dossier(liste_fichiers[i].nom);
            file * liste_fichiers_dossier = lister_fichiers_dossier(liste_fichiers[i].nom);
            compression_dossier(liste_fichiers_dossier, nombre_fichiers_dossier, archive, alphabet);
        } else {
            /* ***Si c'est un fichier, alors il aura une taille (nombre de caractères) */
            liste_fichiers[i].taille = total_occurence_fichier(liste_fichiers[i].nom);
            /* ****On récupère la liste des fichiers/dossiers enfants */
            fwrite(&(liste_fichiers[i].taille), sizeof(int), 1, archive);
            /* ***Séparateur */
            fputc('\0', archive);
            /* ***Compression du fichier */
            compression_fichier(archive, liste_fichiers[i], alphabet);
        }
        /* **Fin de la compression du fichier ou dossier */
    }
    /* *Libération de la mémoire */
    /* **Tableau alphabet */
    for (i = 0; i < 256; i++)
        if (alphabet[i] != NULL) libere(alphabet[i]);
    /* **Fichiers */
    fclose(archive);
}

