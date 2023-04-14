#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "mon_erreur.h"

void mon_erreur(char * format,...){
    va_list arg;
    va_start(arg, format);
    fprintf(stderr, "Erreur - ");
    vfprintf(stderr, format, arg);
    va_end(arg);
    exit(EXIT_FAILURE);
}