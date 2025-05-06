#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "gestioncombattant.h"
#include "combat.h"

int main(int argc, char *argv[]) {
    // Initialisation du générateur de nombres aléatoires
    srand(time(NULL));
    
    // Création des combattants
    Combattant *musu = creer_combattant("Musu");
    Combattant *freettle = creer_combattant("Freettle");
    Combattant *marco = creer_combattant("Marco");
    Combattant *ronflex = creer_combattant("Ronflex");
    
    if (!musu || !freettle) {
        printf("Erreur lors de la création des combattants.\n");
        return EXIT_FAILURE;
    }
    
    // Choix du mode de jeu
    int choix_mode = 2;  // Par défaut: Joueur vs Ordinateur
    
    // Si un argument est fourni, l'utiliser comme choix de mode
    if (argc > 1) {
        choix_mode = atoi(argv[1]);
        if (choix_mode != 1 && choix_mode != 2) {
            choix_mode = 2;  // Valeur par défaut si argument invalide
        }
    } else {
        // Sinon demander à l'utilisateur
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
    }
    
    bool mode_jvj = (choix_mode == 1);
    
    // Création des équipes
    Equipe equipe1;
    strcpy(equipe1.name, "Equipe Joueur 1");
    equipe1.member_count = 1;
    equipe1.members[0] = *musu;
    equipe1.members[1] = *ronflex;
    
    Equipe equipe2;
    strcpy(equipe2.name, mode_jvj ? "Equipe Joueur 2" : "Equipe IA");
    equipe2.member_count = 1;
    equipe2.members[0] = *freettle;
    equipe2.members[1] = *marco;

    // Initialisation du combat avec le mode choisi
    Combat combat;
    initialiser_combat_mode(&combat, &equipe1, &equipe2, mode_jvj);

    printf("\n=== DÉBUT DU COMBAT ===\n");
    printf("Mode: %s\n\n", mode_jvj ? "Joueur vs Joueur" : "Joueur vs Ordinateur");

    // Boucle principale du combat
    while (!verifier_victoire(&combat)) {
        gerer_tour_combat(&combat);
    }
    
    printf("\n=== FIN DU COMBAT ===\n");
    
    // Déterminer le vainqueur
    bool eq1_vivant = false;
    for (int i = 0; i < combat.equipe1->member_count; i++) {
        if (!est_ko(&combat.equipe1->members[i])) {
            eq1_vivant = true;
            break;
        }
    }
    
    printf("\nL'équipe %s remporte la victoire!\n", eq1_vivant ? equipe1.name : equipe2.name);
    
    // Afficher l'état final
    afficher_statuts_combat(&combat);
    
    // Nettoyage
    nettoyer_combat(&combat);
    detruire_combattant(musu);
    detruire_combattant(freettle);
    detruire_combattant(marco);
    detruire_combattant(ronflex);
    
    return EXIT_SUCCESS;
}