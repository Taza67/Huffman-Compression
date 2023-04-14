#include <stdio.h>
#include <stdlib.h>
#include "mon_erreur.h"
#include "allocation.h"

void * allocation_mem(size_t nobjets, size_t taille){
    void * pt;
    pt = malloc (nobjets * taille);
    if (pt == NULL)
        mon_erreur("Impossible d'allouer la memoire %d * %d\n", nobjets, taille);
    return pt;
}

void * allocation_mem_init0(size_t nobjets, size_t taille){
    void * pt;
    pt = calloc(nobjets, taille);
    if (pt == NULL)
        mon_erreur("Impossible d'allouer la mémoire %d * %d\n", nobjets, taille);
    return pt;
}

void * reallocation_mem(void *pt, size_t nobjets, size_t taille){
    void * pt_realloc;
    pt_realloc = realloc(pt, nobjets * taille);
    if (pt_realloc == NULL)
        mon_erreur("Impossible de réallouer la mémoire %d * %d à l'adresse %p\n",
                   nobjets, taille, pt);
    return pt_realloc;
}

void * reallocation_mem_peda(void **pt, size_t nobjets, size_t taille){
    void * pt_realloc;
    pt_realloc = realloc(*pt, nobjets * taille);
    if (pt_realloc != NULL)
        *pt = pt_realloc;
    else
        mon_erreur("Impossible de réallouer la mémoire %d * %d à l'adresse %p\n",
                   nobjets, taille, *pt);
    return pt_realloc;
}

void libere_mem(void * pt){
    void ** adr_pt = (void **) pt;
    if ((*adr_pt) != NULL)
        free(*adr_pt);
    *adr_pt = NULL;
}

void libere_mem_peda(void * *pt){
    if ((*pt) != NULL)
        free(*pt);
    *pt = NULL;
}

void libere(void * pt){
    free(pt);
}
