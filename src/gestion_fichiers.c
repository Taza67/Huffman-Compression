#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "allocation.h"
#include "gestion_erreurs.h"
#include "gestion_fichiers.h"

int verifier_dossier(char * nom_fichier) {
    /* *Déclaration de variables */
    DIR *is_dossier = NULL;
    int retour = 0;
    /* *Si l'ouverture se fait, c'est un dossier */
    if ((is_dossier = opendir(nom_fichier)) != NULL) {
        closedir(is_dossier);
        retour = 1;
    }
    /* *Retour */
    return retour;
}

int verifier_fichier(char * nom_fichier) {
    /* *Déclaration de variables */
    FILE *is_fichier = NULL;
    int retour = 0;
    /* *Si l'ouverture se fait, c'est un fichier */
    if ((is_fichier = fopen(nom_fichier, "r")) != NULL) {
        fclose(is_fichier);
        retour = 1;
    }
    /* *Retour */
    return retour;
}

char renvoyer_type(char *nom_fichier) {
    /* *Déclaration de variables */
    char type = 'e'; /* **Par défaut, le fichier n'est ni un dossier, ni un fichier */
    /* *Si c'est un dossier, alors type = 'd' */
    if (verifier_dossier(nom_fichier) == 1) type = 'd';
    /* *Si c'est un fichier, alors type = 'f' */
    else if (verifier_fichier(nom_fichier) == 1) type = 'f';
    /* *Retour */
    return type;
}

char * creer_chemin_fichier(char *nom_dossier_parent, char *nom_fichier) {
    int longueur_chemin = strlen(nom_dossier_parent) + strlen(nom_fichier) + 1;
    char *chemin = (char*)allocation_mem_init0(longueur_chemin, sizeof(char));
    strcpy(chemin, nom_dossier_parent);
    strcat(chemin, "/");
    strcat(chemin, nom_fichier);
    return chemin;
}

int renvoyer_nombre_fichiers_dossier(char *nom_dossier) {
    /* *Déclaration de variables */
    DIR *dossier = NULL;
    int nombre_fichiers = 0;
    struct dirent *contenu;
    /* *Ouverture du dossier */
    if ((dossier = opendir(nom_dossier)) == NULL) {
        fprintf(stderr, "- Erreur fonction renvoyer_nombre_fichiers_dossier(char *nom_dossier) : ouverture du dossier %s échouée !\n", nom_dossier);
        exit(EXIT_FAILURE);
    }
    /* *Parcours du dossier + incrémentation nombre_fichiers */
    while ((contenu = readdir(dossier)) != NULL)
        if (strcmp(contenu->d_name, ".") != 0 && strcmp(contenu->d_name, "..") != 0)
            nombre_fichiers++;
    /* *Libération de la mémoire */
    /* **Dossiers */
    closedir(dossier);
    /* *Retour */
    return nombre_fichiers;
}

file * lister_fichiers(int taille_ligne, char ** ligne_arguments)  {
    /* *Déclaration de variables */
    int taille_liste_fichiers = taille_ligne - 3, i = 0;
    file * liste_fichiers = (file*)allocation_mem_init0(taille_liste_fichiers, sizeof(file));
    /* *Parcours de la ligne d'arguments */
    for (i = 3; i < taille_ligne; i++) {
        /* **Déclaration de variables */
        char type = 0;
        /* **On récupère chaque fichier dans le tableau liste_fichiers */
        /* ***On alloue de la mémoire à la chaine nom */
        liste_fichiers[i - 3].nom = (char*)allocation_mem_init0(strlen(ligne_arguments[i]), sizeof(char));
        /* ***On copie le nom dans le tableau */
        strcpy(liste_fichiers[i - 3].nom, ligne_arguments[i]);
        /* ***Recherche du type (fichier, dossier) */
        type = renvoyer_type(liste_fichiers[i - 3].nom);
        /* ***Vérification du type */
        if (type != 'f' && type != 'd') {
            fprintf(stderr, "- Erreur fonction lister_fichiers(int taille_ligne, char ** ligne_arguments) : le fichier %s n'est ni un fichier, ni un dossier !\n", liste_fichiers[i - 3].nom);
            exit(EXIT_FAILURE);
        }
        /* ***Affectation du type */
        liste_fichiers[i - 3].type = type;
        /* ***Initialisation de la taille */
        liste_fichiers[i - 3].taille = 0;
    }
    return liste_fichiers;
}

file * lister_fichiers_dossier(char * nom_dossier) {
    /* *Déclaration de variables */
    DIR *dossier = NULL;
    int i = 0,
        taille_liste_fichiers = renvoyer_nombre_fichiers_dossier(nom_dossier);
    file * liste_fichiers = (file*)allocation_mem_init0(taille_liste_fichiers, sizeof(file));
    struct dirent *contenu = NULL;
    /* *Ouverture du dossier */
    if ((dossier = opendir(nom_dossier)) == NULL) {
        fprintf(stderr, "- Erreur fonction lister_fichiers_dossier(char * nom_dossier) : ouverture du dossier %s échouée !\n", nom_dossier);
        exit(EXIT_FAILURE);
    }
    /* *Parcours du dossier */
    while ((contenu = readdir(dossier)) != NULL) {
        if (strcmp(contenu->d_name, ".") != 0 && strcmp(contenu->d_name, "..") != 0) {
            /* **Déclaration de variables */
            char type = 0, *chemin;
            int taille_chemin = strlen(nom_dossier) + strlen(contenu->d_name) + 1;
            /* **On récupère chaque fichier dans le tableau liste_fichiers */
            liste_fichiers[i].nom = (char*)allocation_mem_init0(taille_chemin, sizeof(char));
            /* ***On crée le chemin vers le fichier depuis le dossier */
            chemin = creer_chemin_fichier(nom_dossier, contenu->d_name);     
            strcpy(liste_fichiers[i].nom, chemin);
            /* ***Recherche du type (fichier, dossier) */
            type = renvoyer_type(liste_fichiers[i].nom);
            /* ***Vérification du type */
            if (type != 'f' && type != 'd') {
                fprintf(stderr, "- Erreur fonction lister_fichiers_dossier(char * nom_dossier) : le fichier %s n'est ni un fichier, ni un dossier", liste_fichiers[i].nom);
                exit(EXIT_FAILURE);
            }
            /* ***Affectation du type */
            liste_fichiers[i].type = type;
            /* ***Initialisation de la taille */
            liste_fichiers[i - 3].taille = 0;
            /* ***Prochain fichier dans la liste */
            i++;
        }
    }
    /* *Libération de la mémoire */
    /* **Dossiers */
    closedir(dossier);
    /* *Retour */
    return liste_fichiers;
}

char * renvoyer_nom_modifie(char * nom, char type) {
    /* *Déclaration de variables */
    int i = 0, taille = strlen(nom) + 2, stop = 0;
    char * new_name = (char*)allocation_mem_init0(taille, sizeof(char));
    strcpy(new_name, nom);
    for (i = 0; stop != 1; i++) {
        int verif = (type == 'd') ? verifier_dossier(new_name) : verifier_fichier(new_name);          
        if (verif == 1) {
            new_name[taille - 2] = i + '0';
            new_name[taille - 1] = '\0';
        } else {
            stop = 1;
        }
    }
    return new_name;
}
        