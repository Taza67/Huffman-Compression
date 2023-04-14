#ifndef _GESTION_ERREURS_H_
#define _GESTION_ERREURS_H_

void viderBuffer();

/* Affiche l'aide du programme */
void usage(char *programme);

/* Affiche les options et leurs caractéristiques */
void afficher_options();

/* Renvoie la ligne d'arguments sous forme de chaine de caracteres */
char * concatener_ligne_arguments(int taille, char ** ligne_arguments);

/* Affiche un message d'erreur sur le nombre d'arguments */
void erreur_argument(int taille, char ** ligne_arguments);

/* Affiche un message d'erreur sur l'option */
void erreur_option(int taille, char ** ligne_arguments);

/* Affiche un message d'erreur sur le nombre d'arguments par option */
void erreur_option_argument(int taille, char **ligne_arguments, char option, int version, int taille_min, int taille_max);

/* Affiche un message d'erreur sur le dossier cible donné lors de la décompression */
void erreur_dossier_cible(int taille, char ** ligne_arguments);

/* Affiche un message d'erreur sur l'archive à décompresser */
void erreur_decompression_archive(int taille, char ** ligne_arguments);    

/* Vérifie les fichiers à compresser */
void verifier_fichier_compresser(int taille, char ** ligne_arguments, int fichier_accepte, int dossier_accepte, int version);

/* Vérifie l'archive finale d'une compression */
void verifier_archive_finale(char ** ligne_arguments);

/* Vérifie l'intégralité des contraintes du programme en fonction de la version */
void verifier_ligne_arguments(int taille, char ** ligne_arguments, int version);

#endif