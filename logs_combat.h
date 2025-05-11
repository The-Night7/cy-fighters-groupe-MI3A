#ifndef LOGS_COMBAT_H
#define LOGS_COMBAT_H

#include <stdarg.h>

#define MAX_LOGS 10

typedef struct {
    char messages[MAX_LOGS][256];
    int debut;
    int nombre;
} LogsCombat;

extern LogsCombat logs_combat;
void ajouter_log(const char* format, ...);

#endif