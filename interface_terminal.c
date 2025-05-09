#include <stdio.h>    // Pour printf, scanf, fgets, etc.  // Inclut les fonctions d'entrée/sortie standard
#include <stdlib.h>   // Pour EXIT_SUCCESS, atoi  // Inclut les fonctions utilitaires standard
#include <stdbool.h>  // Pour le type bool  // Inclut le type booléen
#include <time.h>     // Pour time()  // Inclut les fonctions de gestion du temps
#include "combat.h"   // Pour les structures Combat, NiveauDifficulte et les fonctions associées  // Inclut les définitions du combat

int main(int argc, char *argv[]) {  // Point d'entrée du programme avec arguments
    // Initialisation du générateur de nombres aléatoires
    srand(time(NULL));  // Initialise le générateur avec l'heure actuelle
    
    // Choix du mode de jeu
    int choix_mode = 2;  // Par défaut: Joueur vs Ordinateur  // Initialise le mode par défaut
    
    // Si un argument est fourni, l'utiliser comme choix de mode
    if (argc > 1) {  // Vérifie si un argument est passé en ligne de commande
        choix_mode = atoi(argv[1]);  // Convertit l'argument en entier
        if (choix_mode != 1 && choix_mode != 2) {  // Vérifie si le choix est valide
            choix_mode = 2;  // Valeur par défaut si argument invalide  // Réinitialise au mode par défaut
        }
    } else {  // Si aucun argument n'est fourni
        // Sinon demander à l'utilisateur
        printf("=== CY-FIGHTERS ===\n\n");  // Affiche le titre du jeu
        printf("Choisissez un mode de jeu:\n");  // Affiche le menu de sélection
        printf("1. Joueur vs Joueur\n");  // Affiche l'option 1
        printf("2. Joueur vs Ordinateur\n");  // Affiche l'option 2
        printf("Votre choix: ");  // Demande le choix à l'utilisateur
        
        char buffer[32];  // Déclare un buffer pour la saisie
        if (fgets(buffer, sizeof(buffer), stdin) == NULL || sscanf(buffer, "%d", &choix_mode) != 1) {  // Lit et vérifie la saisie
            printf("Erreur de saisie. Mode Joueur vs Ordinateur sélectionné par défaut.\n");  // Affiche message d'erreur
            choix_mode = 2;  // Réinitialise au mode par défaut
        }
    }
    
    bool mode_jvj = (choix_mode == 1);  // Convertit le choix en booléen
    
    // Initialisation du combat
    Combat combat;  // Déclare la structure de combat
    NiveauDifficulte difficulte = DIFFICULTE_MOYENNE; // Difficulté par défaut  // Initialise la difficulté
    
    // Utilisation de la nouvelle fonction de configuration
    configurer_combat(&combat, mode_jvj, &difficulte);  // Configure le combat

    printf("\n=== DÉBUT DU COMBAT ===\n");  // Affiche le début du combat
    printf("Mode: %s\n\n", mode_jvj ? "Joueur vs Joueur" : "Joueur vs Ordinateur");  // Affiche le mode choisi

    // Boucle principale du combat
    while (!verifier_victoire(&combat)) {  // Continue tant qu'il n'y a pas de vainqueur
        gerer_tour_combat(&combat);  // Gère un tour de combat
    }
    
    // Nettoyage
    nettoyer_combat(&combat);  // Libère les ressources
    
    return EXIT_SUCCESS;  // Termine le programme avec succès
}