#include "combat.h"

int main(void) {
    // Initialisation des combattants
    Combattant *musu = creer_combattant("Musu");
    Combattant *freettle = creer_combattant("Freettle");

    if (!musu || !freettle) {
        printf("Erreur lors de la création des combattants.\n");
        return EXIT_FAILURE;
    }

    // Création des équipes
    Equipe equipe1 = {.name = "Equipe Joueur", .member_count = 2};
    equipe1.members[0] = *musu;
    equipe1.members[1] = *freettle;

    Equipe equipe2 = {.name = "Equipe IA", .member_count = 1};
    equipe2.members[0] = *freettle;

    // Initialisation du combat
    Combat combat;
    initialiser_combat(&combat, &equipe1, &equipe2);

    // Boucle principale du combat
    while (!verifier_victoire(&combat)) {
        afficher_combat(&combat);
        gerer_tour_combat(&combat);
    }

    // Résultat final
    printf("Le combat est terminé !\n");
    afficher_statuts_combat(&combat);

    // Nettoyage
    nettoyer_combat(&combat);
    detruire_combattant(musu);
    detruire_combattant(freettle);

    return EXIT_SUCCESS;
}
