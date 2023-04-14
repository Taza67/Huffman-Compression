#include <stdio.h>
#include <stdlib.h>
#include "gestion_erreurs.h"
#include "gestion_fichiers.h"
#include "compression.h"
#include "decompression.h"

int main(int argc, char ** argv) {
    /* *Déclaration de variables */
    char * dossier_cible;
    file *liste_fichiers;
    /* *Vérification des arguments */
    verifier_ligne_arguments(argc,argv, 4);
    /* *Programme */
    switch (argv[1][1]) {
    case 'c':
        liste_fichiers = lister_fichiers(argc, argv);
        compression(liste_fichiers, argc - 3, argv[2]);
        break;
    case 'd':
        dossier_cible = (argc == 4) ? argv[3] : ".";
        decompression(argv[2], dossier_cible);
        break;
    case 'h':
        usage(argv[0]);
        afficher_options();
        break;
    }
    /* *Exit */
    exit(EXIT_SUCCESS);
}
            
        
