#ifndef _GESTION_FICHIERS_H_
#define _GESTION_FICHIERS_H_

/* ***************Structure fichier*************** */

/* Structure qui permet de gérer les caractéristiques des
   fichiers/dossiers à compresser */
typedef struct struct_file {
    char * nom;                 /* nom du fichier */
    char type;                  /* Type du fichier 'd' pour dossier 'f' pour fichier */
    int taille;                 /* Taille du fichier : initialisée à zéro */
} file;

/* *********************************************** */

/* Vérifie si un fichier existe */
int verifier_dossier(char * nom_fichier);

/* Vérifie si un dossier existe */
int verifier_fichier(char * nom_fichier);

/* Renvoie le type d'un fichier */
char renvoyer_type(char *nom_fichier);

/* Rajoute le chemin du répertoire parent au chemin d'un sous-fichier */
char * creer_chemin_fichier(char *nom_dossier_parent, char *nom_fichier);

/* Renvoie le nombre de sous-fichiers directs d'un dossier */
int renvoyer_nombre_fichiers_dossier(char *nom_dossier);

/* Renvoie la liste des fichiers donnés en arguments lors de l'appel au programme */
file * lister_fichiers(int taille_ligne, char ** ligne_arguments);

/* Renvoie la liste des sous-fichiers d'un dossier */
file * lister_fichiers_dossier(char * nom_dossier);

/* Modifie le nom d'un fichier si le dernier est déjà attribué */
char * renvoyer_nom_modifie(char * nom, char type);

#endif