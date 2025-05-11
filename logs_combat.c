#include "logs_combat.h"
#include <stdio.h>

LogsCombat logs_combat = {.debut = 0, .nombre = 0};

void ajouter_log(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    int pos = (logs_combat.debut + logs_combat.nombre) % MAX_LOGS;
    vsnprintf(logs_combat.messages[pos], 256, format, args);
    
    if (logs_combat.nombre < MAX_LOGS) {
        logs_combat.nombre++;
    } else {
        logs_combat.debut = (logs_combat.debut + 1) % MAX_LOGS;
    }
    
    va_end(args);
}
