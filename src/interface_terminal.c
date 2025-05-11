#include <stdio.h>    // Pour printf, scanf, fgets, etc.  // Inclut les fonctions d'entrée/sortie standard
#include <stdlib.h>   // Pour EXIT_SUCCESS, atoi  // Inclut les fonctions utilitaires standard
#include <stdbool.h>  // Pour le type bool  // Inclut le type booléen
#include <time.h>     // Pour time()  // Inclut les fonctions de gestion du temps
#include "combat.h"   // Pour les structures Combat, NiveauDifficulte et les fonctions associées  // Inclut les définitions du combat

#include <pthread.h> // AJOUTÉ
#include "combat.h"

void* executer_terminal(void* arg) {
    // Ce code est ton ancien main()
    srand(time(NULL));
    int choix_mode = 2;

    printf("=== CY-FIGHTERS ===\n\n");
    printf("Choisissez un mode de jeu:\n");
    printf("1. Joueur vs Joueur\n");
    printf("2. Joueur vs Ordinateur\n");
    printf("Votre choix: ");
    
    char buffer[32];
    if (fgets(buffer, sizeof(buffer), stdin) == NULL || sscanf(buffer, "%d", &choix_mode) != 1) {
        printf("Erreur de saisie. Mode Joueur vs Ordinateur sélectionné par défaut.\n");
        choix_mode = 2;
    }

    bool mode_jvj = (choix_mode == 1);

    Combat combat;
    NiveauDifficulte difficulte = DIFFICULTE_MOYENNE;

    configurer_combat(&combat, mode_jvj, &difficulte);

    printf("\n=== DÉBUT DU COMBAT ===\n");
    printf("Mode: %s\n\n", mode_jvj ? "Joueur vs Joueur" : "Joueur vs Ordinateur");

    while (!verifier_victoire(&combat)) {
        gerer_tour_combat(&combat);
    }

    nettoyer_combat(&combat);
    return NULL;
}
