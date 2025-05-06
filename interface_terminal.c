#include "combat.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    // Initialisation du générateur de nombres aléatoires
    srand(time(NULL));
    
    // Initialisation des combattants
    Combattant *musu = creer_combattant("Musu");
    Combattant *freettle = creer_combattant("Freettle");

    if (!musu || !freettle) {
        printf("Erreur lors de la création des combattants.\n");
        return EXIT_FAILURE;
    }

    // Choix du mode de jeu
    int choix_mode = 0;
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
    
    // Création des équipes
    Equipe equipe1 = {.name = "Equipe Joueur 1", .member_count = 1};
    equipe1.members[0] = *musu;
    Equipe equipe2 = {.name = mode_jvj ? "Equipe Joueur 2" : "Equipe IA", .member_count = 1};
    equipe2.members[0] = *freettle;

    // Initialisation du combat avec le mode choisi
    Combat combat;
    initialiser_combat_mode(&combat, &equipe1, &equipe2, mode_jvj);

    printf("\n=== DÉBUT DU COMBAT ===\n");
    printf("Mode: %s\n\n", mode_jvj ? "Joueur vs Joueur" : "Joueur vs Ordinateur");

    // Boucle principale du combat
    while (!verifier_victoire(&combat)) {
        gerer_tour_combat(&combat);
    }

    // Résultat final
    printf("\n=== FIN DU COMBAT ===\n");
    
    // Déterminer le gagnant
    bool eq1_vivant = false;
    for (int i = 0; i < combat.equipe1->member_count; i++) {
        if (!est_ko(&combat.equipe1->members[i])) {
            eq1_vivant = true;
            break;
        }
    }
    
    printf("Vainqueur: %s\n", eq1_vivant ? "Equipe Joueur 1" : (mode_jvj ? "Equipe Joueur 2" : "Equipe IA"));
    afficher_statuts_combat(&combat);

    // Nettoyage
    nettoyer_combat(&combat);
    detruire_combattant(musu);
    detruire_combattant(freettle);

    return EXIT_SUCCESS;
}
