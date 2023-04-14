#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "allocation.h"
#include "gestion_fichiers.h"
#include "en_tete.h"
#include "gestion_erreurs.h"
#include "decompression.h"

/* ******************************Version 0****************************** */

void recuperer_contenu(FILE* archive, FILE* fichier, noeud* arbre_huffman[], int taille_fichier) {
    /* *Déclaration de variables */
    noeud * feuille_caractere = NULL;
    int codage = 0;
    int paquet = 0, taille_paquet = 0,
        ajout = taille_fichier;
    /* *Vérification des fichiers */
    if (archive == NULL) {
        fprintf(stderr, "- Erreur -> fonction recuperer_contenu(FILE* archive, FILE* fichier, noeud* arbre_huffman[256], int taille_fichier) : archive = NULL !\n");
        exit(EXIT_FAILURE);
    }
    if (archive == NULL) {
        fprintf(stderr, "- Erreur -> fonction recuperer_contenu(FILE* archive, FILE* fichier, noeud* arbre_huffman[256], int taille_fichier) : fichier = NULL !\n");
        exit(EXIT_FAILURE);
    }
    /* *Parcours de l'archive octet par octet (8 bits) */
    while((codage = fgetc(archive)) != EOF && ajout > 0) {
        /* **Ajout de 8 bits (0) au paquet */
        paquet = paquet << 8;
        /* **Remplacement des 8 bits ajoutés par les bits du codage */
        paquet = paquet | codage;
        /* **Ajout de 8 bits au paquet => augmentation de sa taille */
        taille_paquet += 8;
        /* **On ajoute ensuite les caractères décodés tant qu'on a pas ajouté autant de caractères que la taille du fichier */
        while ((feuille_caractere = recuperer_feuille(*arbre_huffman, paquet, taille_paquet, 0)) != NULL && ajout > 0) {
            /* ***Ecriture du caractère dans le fichier */
            fputc(feuille_caractere->initial, fichier);
            /* ***On a décodé le code du caractère ajouté => on réduit donc la taille du paquet du nombre de bits décodés */
            taille_paquet -= feuille_caractere->bits;
            /* ***On a un caractère de moins à écrire */
            ajout--;
        }
    }
}

noeud * recuperer_feuille(noeud *racine, int paquet, int taille_paquet, int profondeur) {
    /* *Déclaration des variables */
    int bit = (paquet >> (taille_paquet - profondeur - 1)) & 1;
    /* *Si la profondeur */
    if (taille_paquet < profondeur) return NULL;
    if (racine->gauche == NULL && racine->droit == NULL) return racine;
    if (racine->gauche != NULL && bit == 0)
        return recuperer_feuille(racine->gauche, paquet, taille_paquet, profondeur + 1);
    if (racine->droit != NULL && bit == 1)
        return recuperer_feuille(racine->droit, paquet, taille_paquet, profondeur + 1);
    return NULL;
}

void decompression_fichierV0(char * nom_archive, char *dossier_cible) {
    /* *Déclaration des variables */
    FILE *fichier = NULL, *archive = NULL;
    int taille_nom = 10 , j = 0;
    char *nom_fichier = (char*)allocation_mem_init0(10, sizeof(char)), caractere, *name = NULL;
    int occurence[256], /* Stocke les nombres d'occurences dans le fichier par caractere */
        nbr_char = 0, /* Contient le nombre de caractères disctints du fichier */
        taille_fichier = 0, /* contient le nombre de caractères disctints ou pas du fichier */
        i = 0;
    noeud *alphabet[256], /* Stocke les pointeurs vers les noeuds associés à chaque caractère */
        *arbre_huffman[256]; /* Contient l'arbre de huffman associé au fichier */
    /* *Initialisations des variables */
    for (i = 0; i < 256; i++) {
        occurence[i] = 0;
        alphabet[i] = NULL;
        arbre_huffman[i] = NULL;
    }
    /* *Ouverture de l'archive */
    if ((archive = fopen(nom_archive, "rb")) == NULL) {
        fprintf(stderr, "- Erreur -> fonction decompression_fichierV0(char* nom_archive, char* nom_fichier) : ouverture de l'archive %s impossible !\n", nom_archive);
        exit(EXIT_FAILURE);
    }

    /* **Récupération du nom */
    for (j = 0; (caractere = fgetc(archive)) != '\0' && caractere != EOF; j++) {
        if (j + 1 > taille_nom) {
            taille_nom += 10;
            nom_fichier = (char *)reallocation_mem(nom_fichier, taille_nom, sizeof(char));
        }
        nom_fichier[j] = caractere;
    }
    nom_fichier[j] = '\0';
    /* **Dossier cible */
    nom_fichier = creer_chemin_fichier(dossier_cible, nom_fichier);
    /* *Vérification de fichier déjà existant */
    name = renvoyer_nom_modifie(nom_fichier, 'f');
    /* *Ouverture du fichier */
    if ((fichier = fopen(name, "w")) == NULL) {
        fprintf(stderr, "- Erreur -> fonction decompression_fichierV0(char* nom_archive, char* nom_fichier) : ouverture du fichier %s impossible !\n", nom_fichier);
        exit(EXIT_FAILURE);
    }
    /* *Programme */
    /* *Récupération des occurences */
    recuperer_entete(archive, occurence);
    /* *Création des noeuds */
    creer_tous_noeuds(arbre_huffman, occurence, &nbr_char, &taille_fichier);
    /* *Création de l'arbre */
    creer_noeud(arbre_huffman, nbr_char);
    /* *Création des codes */
    creer_code(*arbre_huffman, 0, 0, alphabet);
    /* *Récupération du fichier d'origine */
    recuperer_contenu(archive, fichier, arbre_huffman, taille_fichier);
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

void decompression_fichier(FILE *archive, char * nom_fichier, noeud* arbre_huffman[256]) {
    /* *Déclaration des variables */
    FILE *fichier = NULL;
    int taille_fichier = 0;
    char * name = NULL;
    /* *Vérification de l'existence du fichier */
    name = renvoyer_nom_modifie(nom_fichier, 'f');
    /* *Ouverture des fichiers */
    if ((fichier = fopen(name, "w")) == NULL) {
        fprintf(stderr, "- Erreur -> fonction decompression_fichier(FILE *archive, char *nom_fichier) : ouverture du fichier %s impossible !\n", name);
        exit(EXIT_FAILURE);
    }
    if (archive == NULL) {
        fprintf(stderr, "- Erreur -> fonction decompression_fichier(FILE *archive, char *nom_fichier) : archive = NULL !\n");
        exit(EXIT_FAILURE);
    }
    /* *Récupération de la taille du fichierr */
    if (fread(&(taille_fichier), sizeof(int), 1, archive) != 1) {
        fprintf(stderr, "- Erreur -> fonction decompression_fichier(FILE *archive, char * nom_fichier, noeud* arbre_huffman[256]) : récupération du nombre de fichiers échouée !\n");
        exit(EXIT_FAILURE);
    }
    if (fgetc(archive) != '\0') {
        fprintf(stderr, "- Erreur -> fonction decompression_fichier(FILE *archive, char * nom_fichier, noeud* arbre_huffman[256]) : séparateur non trouvé, fichier erroné !\n");
        exit(EXIT_FAILURE);
    }
    /* *Récupération du fichier d'origine */
    recuperer_contenu(archive, fichier, arbre_huffman, taille_fichier);
    /* *Libération de la mémoire */
    /* **Fichiers */
    fclose(fichier);
}

void decompressionV1(char *nom_archive, char *dossier_cible) {
    /* *Déclaration des variables */
    FILE *archive = NULL;
    int nombre_fichiers = 0,
        type = 0, sep = 0, caractere = 0, i = 0,
        occurence[256], /* Stocke les nombres d'occurences dans le fichier par caractere */
        nbr_char = 0, /* Contient le nombre de caractères disctints du fichier */
        taille_fichier = 0; /* contient le nombre de caractères disctints ou pas du fichier */
    noeud *alphabet[256], /* Stocke les pointeurs vers les noeuds associés à chaque caractère */
        *arbre_huffman[256]; /* Contient l'arbre de huffman associé au fichier */
    /* *Initialisations des variables */
    for (i = 0; i < 256; i++) {
        occurence[i] = 0;
        alphabet[i] = NULL;
        arbre_huffman[i] = NULL;
    }
    /* *Ouverture de l'archive */
    if ((archive = fopen(nom_archive, "rb")) == NULL) {
        fprintf(stderr, "- Erreur -> fonction decompression(char *nom_archive) : ouverture de l'archive %s impossible !\n", nom_archive);
        exit(EXIT_FAILURE);
    }
    /* *Récupération des occurences */
    recuperer_entete(archive, occurence);
    /* *Création des noeuds */
    creer_tous_noeuds(arbre_huffman, occurence, &nbr_char, &taille_fichier);
    /* *Création de l'arbre */
    creer_noeud(arbre_huffman, nbr_char);
    /* *Création des codes */
    creer_code(*arbre_huffman, 0, 0, alphabet);
    /* *Récupération du nombre de fichiers */
    if (fread(&(nombre_fichiers), sizeof(int), 1, archive) != 1) {
        fprintf(stderr, "- Erreur -> fonction decompression(char *nom_archive) : récupération du nombre de fichiers échouée !\n");
        exit(EXIT_FAILURE);
    }
    /* *Récupération de chaque fichier */
    for (i = 0; i < nombre_fichiers; i++) {
        /* **Déclaration de variables */
        int taille_nom = 10 , j = 0;
        char *nom_fichier = (char*)allocation_mem_init0(10, sizeof(char));
        /* **Récupération du type */
        if (((type = fgetc(archive)) != 'f' && type != 'd') || type == EOF || (sep = fgetc(archive)) != '\0') {
            fprintf(stderr, "- Erreur -> fonction decompression(char *nom_archive) : récupération du type échouée à l'indice %d !\n", i);
            exit(EXIT_FAILURE);
        }
        /* **Récupération du nom */
        for (j = 0; (caractere = fgetc(archive)) != '\0' && caractere != EOF; j++) {
            if (j + 1 > taille_nom) {
                taille_nom += 10;
                nom_fichier = (char *)reallocation_mem(nom_fichier, taille_nom, sizeof(char));
            }
            nom_fichier[j] = caractere;
        }
        nom_fichier[j] = '\0';
        /* **Dossier cible */
        nom_fichier = creer_chemin_fichier(dossier_cible, nom_fichier);
        /* **Décompression du fichier */
        decompression_fichier(archive, nom_fichier, arbre_huffman);
        /* **Libération de la mémoire */
        /* ***Pointeurs */
        libere(nom_fichier);
    }
    /* *Libération de la mémoire */
    /* **Noeuds */
    for (i = 0; i < 256; i++)
        if (alphabet[i] != NULL) libere(alphabet[i]);
    /* **Fichiers */
    fclose(archive);
}

/* ********************************************************************* */

/* ******************************Version 2****************************** */

void decompressionV2(char *nom_archive, char *dossier_cible) {
    /* *Déclaration des variables */
    FILE *archive = NULL;
    int nombre_fichiers = 0,
        type = 0, sep = 0, caractere = 0, i = 0,
        occurence[256], /* Stocke les nombres d'occurences dans le fichier par caractere */
        nbr_char = 0, /* Contient le nombre de caractères disctints du fichier */
        taille_fichier = 0; /* contient le nombre de caractères disctints ou pas du fichier */
    noeud *alphabet[256], /* Stocke les pointeurs vers les noeuds associés à chaque caractère */
        *arbre_huffman[256]; /* Contient l'arbre de huffman associé au fichier */
    /* *Initialisations des variables */
    for (i = 0; i < 256; i++) {
        occurence[i] = 0;
        alphabet[i] = NULL;
        arbre_huffman[i] = NULL;
    }
    /* *Ouverture de l'archive */
    if ((archive = fopen(nom_archive, "rb")) == NULL) {
        fprintf(stderr, "- Erreur -> fonction decompression(char *nom_archive) : ouverture de l'archive %s impossible !\n", nom_archive);
        exit(EXIT_FAILURE);
    }
    /* *Récupération des occurences */
    recuperer_entete(archive, occurence);
    /* *Création des noeuds */
    creer_tous_noeuds(arbre_huffman, occurence, &nbr_char, &taille_fichier);
    /* *Création de l'arbre */
    creer_noeud(arbre_huffman, nbr_char);
    /* *Création des codes */
    creer_code(*arbre_huffman, 0, 0, alphabet);
    /* *Récupération du nombre de fichiers */
    if (fread(&(nombre_fichiers), sizeof(int), 1, archive) != 1) {
        fprintf(stderr, "- Erreur -> fonction decompression(char *nom_archive) : récupération du nombre de fichiers échouée !\n");
        exit(EXIT_FAILURE);
    }
    /* *Récupération de chaque fichier */
    for (i = 0; i < nombre_fichiers; i++) {
        /* **Déclaration de variables */
        int taille_nom = 10 , j = 0;
        char *nom_fichier = (char*)allocation_mem_init0(10, sizeof(char));
        /* **Récupération du type */
        if (((type = fgetc(archive)) != 'f' && type != 'd') || type == EOF || (sep = fgetc(archive)) != '\0') {
            fprintf(stderr, "- Erreur -> fonction decompression(char *nom_archive) : récupération du type échouée à l'indice %d !\n", i);
            exit(EXIT_FAILURE);
        }
        /* **Récupération du nom */
        for (j = 0; (caractere = fgetc(archive)) != '\0' && caractere != EOF; j++) {
            if (j + 1 > taille_nom) {
                taille_nom += 10;
                nom_fichier = (char *)reallocation_mem(nom_fichier, taille_nom, sizeof(char));
            }
            nom_fichier[j] = caractere;
        }
        nom_fichier[j] = '\0';
        /* **Dossier cible */
        nom_fichier = creer_chemin_fichier(dossier_cible, nom_fichier);
        if (verifier_dossier(nom_fichier) == 1) {
            int erreur = 0, patience = 5;
            char c = 0;
            do {
                /* **Demande de confirmation */
                fprintf(stdout, "- Avertissement : un dossier %s existe déjà !\n  Si le programme se poursuit, ce dossier fusionnera avec le dossier du même nom dans l'archive. Êtes-vous sûr de continuer ? (Entrez 'O' pour Oui et 'N' pour Non) -> ",
                        nom_fichier);
                c = getchar();
                if (c != 'O' && c != 'N') patience--;
                viderBuffer();
            } while (c != 'O' && c != 'N' && patience != 0);
            /* **Réponse */
            if (c == 'N' || patience == 0) {
                /* ***Arrêt du programme */
                erreur = 1;
                fprintf(stdout, "- Arrêt du programme : l'utilisateur ne souhaite pas continuer.\n");
            }
            /* *Fermeture du programme si erreur */
            if (erreur == 1) exit(EXIT_FAILURE);
        }
        mkdir(nom_fichier, S_IRWXU);
        decompression_dossier(archive, arbre_huffman, dossier_cible);
        /* **Libération de la mémoire */
        /* ***Pointeurs */
        libere(nom_fichier);
    }
    /* *Libération de la mémoire */
    /* **Noeuds */
    for (i = 0; i < 256; i++)
        if (alphabet[i] != NULL) libere(alphabet[i]);
    /* **Fichiers */
    fclose(archive);
}

void decompression_dossierV2(FILE *archive, noeud* arbre_huffman[256], char *dossier_cible) {
    /* *Déclaration des variables */
    int nombre_fichiers = 0,
        type = 0, sep = 0, caractere = 0, i = 0;
    /* *Vérification de l'archive */
    if (archive == NULL) {
        fprintf(stderr, "- Erreur -> fonction decompression_dossierV2(FILE *archive) : archive = NULL !\n");
        exit(EXIT_FAILURE);
    }
    /* *Récupération du nombre de fichiers */
    if (fread(&(nombre_fichiers), sizeof(int), 1, archive) != 1) {
        fprintf(stderr, "- Erreur -> fonction decompression_dossierV2(FILE *archive) : récupération du nombre de fichiers échouée !\n");
        exit(EXIT_FAILURE);
    }
    /* *Récupération de chaque fichier */
    for (i = 0; i < nombre_fichiers; i++) {
        /* **Déclaration de variables */
        int taille_nom = 10, j = 0;
        char * nom_fichier = (char*)allocation_mem_init0(taille_nom, sizeof(char));
        /* **Récupération du type */
        if (((type = fgetc(archive)) != 'f' && type != 'd') || type == EOF || (sep = fgetc(archive)) != '\0') {
            fprintf(stderr, "- Erreur -> fonction decompressionV2(char *nom_archive) : récupération du type échouée à l'indice %d !\n", i);
            exit(EXIT_FAILURE);
        }
        /* **Récupération du nom */
        for (j = 0; (caractere = fgetc(archive)) != '\0' && caractere != EOF; j++) {
            if (j + 1 > taille_nom) {
                taille_nom += 10;
                nom_fichier = (char *)reallocation_mem(nom_fichier, taille_nom, sizeof(char));
            }
            nom_fichier[j] = caractere;
        }
        nom_fichier[j] = '\0';
        /* **Dossier cible */
        nom_fichier = creer_chemin_fichier(dossier_cible, nom_fichier);
        /* **Décompression du fichier */
        decompression_fichier(archive, nom_fichier, arbre_huffman);
        /* **Libération de la mémoire */
        /* ***Pointeurs */
        libere(nom_fichier);
    }
}
    
/* ********************************************************************* */

/* ******************************Version 3-4**************************** */

void decompression(char *nom_archive, char *dossier_cible) {
    /* *Déclaration des variables */
    FILE *archive = NULL;
    int nombre_fichiers = 0,
        type = 0, sep = 0, caractere = 0, i = 0,
        occurence[256], /* Stocke les nombres d'occurences dans le fichier par caractere */
        nbr_char = 0, /* Contient le nombre de caractères disctints du fichier */
        taille_fichier = 0; /* contient le nombre de caractères disctints ou pas du fichier */
    noeud *alphabet[256], /* Stocke les pointeurs vers les noeuds associés à chaque caractère */
        *arbre_huffman[256]; /* Contient l'arbre de huffman associé au fichier */
    /* *Initialisations des variables */
    for (i = 0; i < 256; i++) {
        occurence[i] = 0;
        alphabet[i] = NULL;
        arbre_huffman[i] = NULL;
    }
    /* *Ouverture de l'archive */
    if ((archive = fopen(nom_archive, "rb")) == NULL) {
        fprintf(stderr, "- Erreur -> fonction decompression(char *nom_archive) : ouverture de l'archive %s impossible !\n", nom_archive);
        exit(EXIT_FAILURE);
    }
    /* *Récupération des occurences */
    recuperer_entete(archive, occurence);
    /* *Création des noeuds */
    creer_tous_noeuds(arbre_huffman, occurence, &nbr_char, &taille_fichier);
    /* *Création de l'arbre */
    creer_noeud(arbre_huffman, nbr_char);
    /* *Création des codes */
    creer_code(*arbre_huffman, 0, 0, alphabet);
    /* *Récupération du nombre de fichiers */
    if (fread(&(nombre_fichiers), sizeof(int), 1, archive) != 1) {
        fprintf(stderr, "- Erreur -> fonction decompression(char *nom_archive) : récupération du nombre de fichiers échouée !\n");
        exit(EXIT_FAILURE);
    }
    /* *Récupération de chaque fichier */
    for (i = 0; i < nombre_fichiers; i++) {
        /* **Déclaration de variables */
        int taille_nom = 10 , j = 0;
        char *nom_fichier = (char*)allocation_mem_init0(10, sizeof(char));
        /* **Récupération du type */
        if (((type = fgetc(archive)) != 'f' && type != 'd') || type == EOF || (sep = fgetc(archive)) != '\0') {
            fprintf(stderr, "- Erreur -> fonction decompression(char *nom_archive) : récupération du type échouée à l'indice %d !\n", i);
            exit(EXIT_FAILURE);
        }
        /* **Récupération du nom */
        for (j = 0; (caractere = fgetc(archive)) != '\0' && caractere != EOF; j++) {
            if (j + 1 > taille_nom) {
                taille_nom += 10;
                nom_fichier = (char *)reallocation_mem(nom_fichier, taille_nom, sizeof(char));
            }
            nom_fichier[j] = caractere;
        }
        nom_fichier[j] = '\0';
        /* **Dossier cible */
        nom_fichier = creer_chemin_fichier(dossier_cible, nom_fichier);
        if (type == 'd') {
            if (verifier_dossier(nom_fichier) == 1) {
                int erreur = 0, patience = 5;
                char c = 0;
                do {
                    /* **Demande de confirmation */
                    fprintf(stdout, "- Avertissement : un dossier %s existe déjà !\n  Si le programme se poursuit, ce dossier fusionnera avec le dossier du même nom dans l'archive. Êtes-vous sûr de continuer ? (Entrez 'O' pour Oui et 'N' pour Non) -> ",
                            nom_fichier);
                    c = getchar();
                    if (c != 'O' && c != 'N') patience--;
                    viderBuffer();
                } while (c != 'O' && c != 'N' && patience != 0);
                /* **Réponse */
                if (c == 'N' || patience == 0) {
                    /* ***Arrêt du programme */
                    erreur = 1;
                    fprintf(stdout, "- Arrêt du programme : l'utilisateur ne souhaite pas continuer.\n");
                }
                /* *Fermeture du programme si erreur */
                if (erreur == 1) exit(EXIT_FAILURE);
            }
            mkdir(nom_fichier, S_IRWXU);
            decompression_dossier(archive, arbre_huffman, dossier_cible);
        } else {
            /* **Décompression du fichier */
            decompression_fichier(archive, nom_fichier, arbre_huffman);
        }
        /* **Libération de la mémoire */
        /* ***Pointeurs */
        libere(nom_fichier);
    }
    /* *Libération de la mémoire */
    /* **Noeuds */
    for (i = 0; i < 256; i++)
        if (alphabet[i] != NULL) libere(alphabet[i]);
    /* **Fichiers */
    fclose(archive);
}

void decompression_dossier(FILE *archive, noeud* arbre_huffman[256], char *dossier_cible) {
    /* *Déclaration des variables */
    int nombre_fichiers = 0,
        type = 0, sep = 0, caractere = 0, i = 0;
    /* *Vérification de l'archive */
    if (archive == NULL) {
        fprintf(stderr, "- Erreur -> fonction decompression_dossier(FILE *archive) : archive = NULL !\n");
        exit(EXIT_FAILURE);
    }
    /* *Récupération du nombre de fichiers */
    if (fread(&(nombre_fichiers), sizeof(int), 1, archive) != 1) {
        fprintf(stderr, "- Erreur -> fonction decompression_dossier(FILE *archive) : récupération du nombre de fichiers échouée !\n");
        exit(EXIT_FAILURE);
    }
    /* *Récupération de chaque fichier */
    for (i = 0; i < nombre_fichiers; i++) {
        /* **Déclaration de variables */
        int taille_nom = 10, j = 0;
        char * nom_fichier = (char*)allocation_mem_init0(taille_nom, sizeof(char));
        /* **Récupération du type */
        if (((type = fgetc(archive)) != 'f' && type != 'd') || type == EOF || (sep = fgetc(archive)) != '\0') {
            fprintf(stderr, "- Erreur -> fonction decompression(char *nom_archive) : récupération du type échouée à l'indice %d !\n", i);
            exit(EXIT_FAILURE);
        }
        /* **Récupération du nom */
        for (j = 0; (caractere = fgetc(archive)) != '\0' && caractere != EOF; j++) {
            if (j + 1 > taille_nom) {
                taille_nom += 10;
                nom_fichier = (char *)reallocation_mem(nom_fichier, taille_nom, sizeof(char));
            }
            nom_fichier[j] = caractere;
        }
        nom_fichier[j] = '\0';
        /* **Dossier cible */
        nom_fichier = creer_chemin_fichier(dossier_cible, nom_fichier);
        if (type == 'd') {
            mkdir(nom_fichier, S_IRWXU);
            decompression_dossier(archive, arbre_huffman, dossier_cible);
        } else {
            /* **Décompression du fichier */
            decompression_fichier(archive, nom_fichier, arbre_huffman);
        }
        /* **Libération de la mémoire */
        /* ***Pointeurs */
        libere(nom_fichier);
    }
}

/* ********************************************************************* */