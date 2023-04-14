#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "allocation.h"
#include "gestion_fichiers.h"
#include "gestion_erreurs.h"

void viderBuffer() {
    int c = 0;
    while (c != '\n' && c != EOF) {
        c = getchar();
    }
}

void usage(char* programme) {
    fprintf(stderr, "- Usage : %s <option> [...]\n+ Fonction : compression - décompression\n", programme);
}

void afficher_options() {
    fprintf(stderr, "- Options disponibles :\n\n\t[-c]\t- <archive_finale> <dossiers_ou_fichiers_a_compresser>\n\t\t- Cette option permet de compresser une liste de fichiers donnés en une archive.\n\t\t- Paramètre <archive_finale> : nom de l'archive\n\t\t- Paramètre <dossiers_ou_fichiers_a_compresser> : noms de fichiers ou dossiers qui existent.\n");
    fprintf(stderr, "\n\t[-d]\t- <archive_a_decompresser> {dossier_cible}\n\t\t- Cette option permet de décompresser une archive. Si {dossier_cible} spécifié, alors la décompression se fera dans ce dossier.\n\t\t- Paramètre <archive_a_decompresser> : nom d'une archive qui existe.\n\t\t- Paramètre optionnel {dossier_cible} : dossier_cible doit exister.\n");
    fprintf(stderr, "\n\t[-h]\t- Cette option permet d'afficher une page d'aide.\n\n");
}

char * concatener_ligne_arguments(int taille, char ** ligne_arguments) {
    /* *Déclaration de variables */
    int i = 0, taille_chaine = 0;
    char * ligne = NULL;
    /* *Calcule de la longueur totale de la ligne */
    for (i = 0; i < taille; i++) taille_chaine += strlen(ligne_arguments[i]) + 1;
    /* *Initialisation de la chaine */
    ligne = (char*)allocation_mem_init0(taille_chaine, sizeof(char));
    /* *Parcours de la ligne */
    for (i = 0; i < taille; i++) {
        /* *On ajoute d'abord l'argument */
        strcat(ligne, ligne_arguments[i]);
        /* *On ajoute ensuite un espace comme séparateur */
        strcat(ligne, " ");
    }
    /* Retour */
    return ligne;
}

void erreur_argument(int taille, char ** ligne_arguments) {
    /* *Déclaration de variables */
    char *ligne = NULL;
    /* *Récupération de la ligne d'arguments sous forme de chaine de caractère */
    ligne = concatener_ligne_arguments(taille, ligne_arguments);
    /* *Affichage du message d'erreur */
    fprintf(stderr, "- Erreur programme %s : le nombre de paramètres minimal est 1 !\n+ Ligne entrée : %s\n+ Tapez \"%s -h\" pour avoir plus d'aide.\n", ligne_arguments[0], ligne, ligne_arguments[0]);
    /* *Fermeture du programme */
    exit(EXIT_FAILURE);
}

void erreur_option(int taille, char ** ligne_arguments) {
    /* *Déclaration de variables */
    char *ligne = NULL;
    /* *Récupération de la ligne d'arguments sous forme de chaine de caractère */
    ligne = concatener_ligne_arguments(taille, ligne_arguments);
    /* *Affichage du message d'erreur */
    fprintf(stderr, "- Erreur programme %s : l'option %s n'est pas une option valable !\n+ Ligne entrée : %s\n+ Tapez \"%s -h\" pour avoir plus d'aide.\n", ligne_arguments[0], ligne_arguments[1], ligne, ligne_arguments[0]);
    /* *Fermeture du programme */
    exit(EXIT_FAILURE);
}

void erreur_option_argument(int taille, char **ligne_arguments, char option, int version, int taille_min, int taille_max) {
    /* *Déclaration de variables */
    char *ligne = NULL;
    char *fich_doss = "";
    /* *Récupération de la ligne d'arguments sous forme de chaine de caractère */
    ligne = concatener_ligne_arguments(taille, ligne_arguments);
    /* *Modification de fich_doss en fonction de la version */
    if (version < 2) fich_doss = "fichier(s)";
    else if (version > 1 && version < 4) fich_doss = "dossier(s)";
    else if (version == 4) fich_doss = "dossier(s) et/ou fichiers(s)";
    /* *Comparaison du paramètre option */
    switch(option) {
    /* **Option aide */
    case 'h': 
        fprintf(stderr, "- Erreur programme %s version %d : l'option %s ne nécessite pas de paramètres !\n+ Ligne entrée : %s\n+ Tapez \"%s -h\" pour avoir plus d'aide.\n", ligne_arguments[0], version, ligne_arguments[1], ligne, ligne_arguments[0]);
        break;
    /* **Option compression */
    case 'c':
        fprintf(stderr, "- Erreur programme %s version %d : l'option %s nécessite au moins un nom d'archive suivi d'au moins %d %s et au plus %d %s !\n+ Ligne entrée : %s\n+ Tapez \"%s -h\" pour avoir plus d'aide.\n", ligne_arguments[0], version, ligne_arguments[1], taille_min, fich_doss, taille_max, fich_doss, ligne, ligne_arguments[0]);
        break;
    /* **Option décompression */
    case 'd':
        fprintf(stderr, "- Erreur programme %s version %d : l'option %s nécessite au moins un paramètre et au plus deux (un nom d'archive suivi optionnellement d'un dossier cible) !\n+ Ligne entrée : %s\n+ Tapez \"%s -h\" pour avoir plus d'aide.\n", ligne_arguments[0], version, ligne_arguments[1], ligne, ligne_arguments[0]);
    }
    /* *Fermeture du programme */
    exit(EXIT_FAILURE);
}

void erreur_dossier_cible(int taille, char ** ligne_arguments) {
    /* *Déclaration de variables */
    char *ligne = NULL;
    /* *Récupération de la ligne d'arguments sous forme de chaine de caractère */
    ligne = concatener_ligne_arguments(taille, ligne_arguments);
    /* *Affichage du message d'erreur */
    fprintf(stderr, "- Erreur programme %s : le dossier %s n'existe pas !\n+ Ligne entrée : %s\n+ Tapez \"%s -h\" pour avoir plus d'aide.\n", ligne_arguments[0], ligne_arguments[3], ligne, ligne_arguments[0]);
    /* *Fermeture du programme */
    exit(EXIT_FAILURE);
}

void erreur_decompression_archive(int taille, char ** ligne_arguments) {
     /* *Déclaration de variables */
    char *ligne = NULL;
    /* *Récupération de la ligne d'arguments sous forme de chaine de caractère */
    ligne = concatener_ligne_arguments(taille, ligne_arguments);
    /* *Affichage du message d'erreur */
    fprintf(stderr, "- Erreur programme %s : l'archive %s n'existe pas !\n+ Ligne entrée : %s\n+ Tapez \"%s -h\" pour avoir plus d'aide.\n", ligne_arguments[0], ligne_arguments[2], ligne, ligne_arguments[0]);
    /* *Fermeture du programme */
    exit(EXIT_FAILURE);
}


void verifier_fichier_compresser(int taille, char ** ligne_arguments, int fichier_accepte, int dossier_accepte, int version) {
    /* *Déclaration de variables */
    int i = 0, erreur = 0;
    /* *Vérification par fichier */
    for (i = 3; i < taille; i++) {
        /* **Déclaration de variables */
        int verif_fic = 0, verif_dos = 0;
        /* **Vérification si c'est un fichier et si les fichiers sont acceptés */
        if (version != 4) {
            verif_dos = verifier_dossier(ligne_arguments[i]);
            verif_fic = verifier_fichier(ligne_arguments[i]);
            
            if ((verif_fic != 1 && verif_dos != 1) && fichier_accepte == 1) {
                fprintf(stderr, "- Erreur programme %s version %d : le fichier %s n'existe pas !\n", ligne_arguments[0], version, ligne_arguments[i]);
                erreur = 1;
            } else if ((verif_fic == 1 && verif_dos != 1) && fichier_accepte != 1 ) {
                fprintf(stderr, "- Erreur programme %s version %d : cette version n'accepte pas les fichiers et %s en est un !\n", ligne_arguments[0], version, ligne_arguments[i]);
                erreur = 1;
            } else if (verif_dos != 1 && dossier_accepte == 1) {
                fprintf(stderr, "- Erreur programme %s version %d : le dossier %s n'existe pas !\n", ligne_arguments[0], version, ligne_arguments[i]);
                erreur = 1;
            } else if (verif_dos == 1 && dossier_accepte != 1 ) {
                fprintf(stderr, "- Erreur programme %s version %d : cette version n'accepte pas les dossiers et %s en est un !\n", ligne_arguments[0], version, ligne_arguments[i]);
                erreur = 1;
            }
        } else {
            if (verifier_fichier(ligne_arguments[i]) != 1 && verifier_fichier(ligne_arguments[i]) != 1) {
                fprintf(stderr, "- Erreur programme %s version %d : le dossier ou fichier %s n'existe pas !\n", ligne_arguments[0], version, ligne_arguments[i]);
                erreur = 1;
            }
        }
        
    }
    /* *Fermeture du programme si erreur */
    if (erreur == 1) exit(EXIT_FAILURE);
}

void verifier_archive_finale(char ** ligne_arguments) {
    /* *Déclaration de variables */
    int erreur = 0, patience = 5;
    char c = 0;
    /* *Vérification */
    if (verifier_fichier(ligne_arguments[2]) == 1) {
        do {
            /* **Demande de confirmation */
            fprintf(stdout, "- Avertissement programme %s : un fichier du nom de l'archive %s existe déjà !\n  Si le programme se poursuit, ce fichier sera écrasé. Êtes-vous sûr de continuer ? (Entrez 'O' pour Oui et 'N' pour Non) -> ",
                    ligne_arguments[0], ligne_arguments[2]);
            c = getchar();
            if (c != 'O' && c != 'N') patience--;
            viderBuffer();
        } while (c != 'O' && c != 'N' && patience != 0);
        /* **Réponse */
        if (c == 'N' || patience == 0) {
            /* ***Arrêt du programme */
            erreur = 1;
            fprintf(stdout, "- Arrêt du programme : l'utilisateur ne souhaite pas écraser le fichier existant.\n");
        }
    }
    /* *Fermeture du programme si erreur */
    if (erreur == 1) exit(EXIT_FAILURE);
}

void verifier_ligne_arguments(int taille, char ** ligne_arguments, int version) {
    /* *Déclaration de variables */
    char option = 0;
    int taille_max = 0,
        taille_min = 0,
        dossier_accepte = 0,
        fichier_accepte = 0;
    /* *Initialisation des variables en fonction de la version */
    switch(version) {
    case 0:
        taille_max = taille_min = fichier_accepte = 1;
        break;
    case 1:
        taille_min = fichier_accepte = 1;
        taille_max = INT_MAX - 3;
        break;
    case 2:
    case 3:
        taille_min = taille_max = 1;
        dossier_accepte = 1;
        break;
    case 4:
        taille_min = 1;
        taille_max = INT_MAX - 3;
        fichier_accepte = dossier_accepte = 1;
        break;
    }
    /* *Vérification du nombre d'arguments minimal */
    if (taille < 2)
        erreur_argument(taille, ligne_arguments);
    /* *Vérification de la syntaxe du paramètre option */
    else if (strlen(ligne_arguments[1]) != 2 || ligne_arguments[1][0] != '-')
        erreur_option(taille, ligne_arguments);
    /* *Vérification de la cohérence du paramètre option */
    else if ((option = ligne_arguments[1][1]) != 'c' && option != 'd' && option != 'h')
        erreur_option(taille, ligne_arguments);
    /* *Vérification des arguments par option */
    if (option == 'h') {
        /* **Option manuel d'aide => "huffman-compressor -h" uniquement */
        if (taille != 2) erreur_option_argument(taille, ligne_arguments, option, version, taille_min, taille_max);
    } else if (option == 'c') {    
        /* **Option compression => "huffman-compressor -c <archive_finale> <fichier_a_compresser> */
        if (taille < taille_min + 3 || taille > taille_max + 3) erreur_option_argument(taille, ligne_arguments, option, version, taille_min, taille_max);
        /* **Vérification du fichier à compresser */
        verifier_fichier_compresser(taille, ligne_arguments, fichier_accepte, dossier_accepte, version);
        /* **Vérification de l'archive finale */
        verifier_archive_finale(ligne_arguments);
    } else if (option == 'd') {
        /* **Option décompression => "huffman-compressor -d <archive> [dossier_cible]" */
        if (taille < 3 || taille > 4) erreur_option_argument(taille, ligne_arguments, option, version, taille_min, taille_max);
        /* **Vérification du dossier cible */
        else if (taille == 4 && verifier_dossier(ligne_arguments[3]) == 0) erreur_dossier_cible(taille, ligne_arguments);
        /* **Vérification de l'archive à décompresser */
        if (verifier_fichier(ligne_arguments[2]) == 0) erreur_decompression_archive(taille, ligne_arguments);
    }
        
        
}