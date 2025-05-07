#include "aff_combat.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h> // pour strcmp

// Affichage de l'état du combat
void afficher_combat(const Combat* combat) {
    printf("\n--- État du combat (Tour %d) ---\n", combat->tour); // Affiche le numéro du tour
    
    for (int i = 0; i < combat->nombre_participants; i++) { // Parcours de tous les participants
        const EtatCombattant* cs = &combat->participants[i]; // Récupère l'état du combattant
        printf("%s [%.0f/%.0f PV] - TM: %.1f - Contrôle: %s\n", // Affiche les informations du combattant
               cs->combattant->nom, // Nom
               cs->combattant->Vie.courrante, // PV actuels
               cs->combattant->Vie.max, // PV maximum
               cs->turn_meter, // Jauge de tour
               cs->controleur == JOUEUR ? "Joueur" : "IA"); // Contrôleur
               
        // Afficher les cooldowns
        for (int j = 0; j < MAX_TECHNIQUES; j++) { // Parcours des techniques
            if (cs->cooldowns[j] > 0) { // Si la technique est en cooldown
                printf("  %s: %d tours\n", 
                       cs->combattant->techniques[j].nom, // Nom de la technique
                       cs->cooldowns[j]); // Cooldown restant
            }
        }
        
        // Afficher les effets actifs
        for (int j = 0; j < cs->nb_effets; j++) { // Parcours des effets
            printf("  Effet: "); // Début de l'affichage de l'effet
            switch (cs->effets[j].type) { // Selon le type d'effet
                case EFFET_POISON: // Si poison
                    printf("Poison"); // Affiche "Poison"
                    break;
                case EFFET_ETOURDISSEMENT: // Si étourdissement
                    printf("Étourdissement"); // Affiche "Étourdissement"
                    break;
                case EFFET_BOOST_ATTAQUE: // Si boost d'attaque
                    printf("Boost d'attaque"); // Affiche "Boost d'attaque"
                    break;
                case EFFET_BOOST_DEFENSE: // Si boost de défense
                    printf("Boost de défense"); // Affiche "Boost de défense"
                    break;
                case EFFET_BRULURE: // Si brûlure
                    printf("Brûlure"); // Affiche "Brûlure"
                    break;
                default: // Autre effet
                    printf("Inconnu"); // Affiche "Inconnu"
                    break;
            }
            printf(" (%d tours restants)\n", cs->effets[j].tours_restants); // Affiche la durée restante
        }
    }
}

// Affiche les statuts des équipes
void afficher_statuts_combat(Combat* combat) {
    printf("\n=== TOUR %d ===\n", combat->tour); // Affiche le numéro du tour
    
    // Déterminer si nous sommes en mode JvJ
    bool mode_jvj = true;
    for (int i = 0; i < combat->equipe2->member_count; i++) {
        for (int j = 0; j < combat->nombre_participants; j++) {
            if (combat->participants[j].combattant == &combat->equipe2->members[i] && 
                combat->participants[j].controleur == ORDI) {
                mode_jvj = false;
                break;
            }
        }
        if (!mode_jvj) break;
    }
    
    // Équipe 1
    printf("\nÉquipe 1 (%s):\n", mode_jvj ? "JOUEUR 1" : "JOUEUR"); // Titre de l'équipe 1
    for (int i = 0; i < combat->equipe1->member_count; i++) { // Parcours des membres de l'équipe 1
        Combattant* c = &combat->equipe1->members[i]; // Récupère le combattant
        printf("- %s: %.0f/%.0f PV", c->nom, c->Vie.courrante, c->Vie.max); // Affiche les PV
        if (est_ko(c)) printf(" (KO)"); // Indique si le combattant est KO
        
        // Afficher les effets actifs
        for (int j = 0; j < combat->nombre_participants; j++) { // Parcours de tous les participants
            if (combat->participants[j].combattant == c && combat->participants[j].nb_effets > 0) { // Si c'est le bon combattant et qu'il a des effets
                printf(" ["); // Début de la liste d'effets
                for (int k = 0; k < combat->participants[j].nb_effets; k++) { // Parcours des effets
                    switch (combat->participants[j].effets[k].type) { // Selon le type d'effet
                        case EFFET_POISON: // Si poison
                            printf("Poison"); // Affiche "Poison"
                            break;
                        case EFFET_ETOURDISSEMENT: // Si étourdissement
                            printf("Étourdi"); // Affiche "Étourdi"
                            break;
                        case EFFET_BOOST_ATTAQUE: // Si boost d'attaque
                            printf("Att+"); // Affiche "Att+"
                            break;
                        case EFFET_BOOST_DEFENSE: // Si boost de défense
                            printf("Def+"); // Affiche "Def+"
                            break;
                        case EFFET_BRULURE: // Si brûlure
                            printf("Brûlure"); // Affiche "Brûlure"
                            break;
                        default: // Autre effet
                            break;
                    }
                    if (k < combat->participants[j].nb_effets - 1) printf(", "); // Ajoute une virgule si ce n'est pas le dernier effet
                }
                printf("]"); // Fin de la liste d'effets
            }
        }
        printf("\n"); // Nouvelle ligne
    }
    
    // Équipe 2
    printf("\nÉquipe 2 (%s):\n", mode_jvj ? "JOUEUR 2" : "ORDINATEUR"); // Titre de l'équipe 2
    for (int i = 0; i < combat->equipe2->member_count; i++) { // Parcours des membres de l'équipe 2
        Combattant* c = &combat->equipe2->members[i]; // Récupère le combattant
        printf("- %s: %.0f/%.0f PV", c->nom, c->Vie.courrante, c->Vie.max); // Affiche les PV
        if (est_ko(c)) printf(" (KO)"); // Indique si le combattant est KO
        
        // Afficher les effets actifs
        for (int j = 0; j < combat->nombre_participants; j++) { // Parcours de tous les participants
            if (combat->participants[j].combattant == c && combat->participants[j].nb_effets > 0) { // Si c'est le bon combattant et qu'il a des effets
                printf(" ["); // Début de la liste d'effets
                for (int k = 0; k < combat->participants[j].nb_effets; k++) { // Parcours des effets
                    switch (combat->participants[j].effets[k].type) { // Selon le type d'effet
                        case EFFET_POISON: // Si poison
                            printf("Poison"); // Affiche "Poison"
                            break;
                        case EFFET_ETOURDISSEMENT: // Si étourdissement
                            printf("Étourdi"); // Affiche "Étourdi"
                            break;
                        case EFFET_BOOST_ATTAQUE: // Si boost d'attaque
                            printf("Att+"); // Affiche "Att+"
                            break;
                        case EFFET_BOOST_DEFENSE: // Si boost de défense
                            printf("Def+"); // Affiche "Def+"
                            break;
                        case EFFET_BRULURE: // Si brûlure
                            printf("Brûlure"); // Affiche "Brûlure"
                            break;
                        default: // Autre effet
                            break;
                    }
                    if (k < combat->participants[j].nb_effets - 1) printf(", "); // Ajoute une virgule si ce n'est pas le dernier effet
                }
                printf("]"); // Fin de la liste d'effets
            }
        }
        printf("\n"); // Nouvelle ligne
    }
    printf("\n"); // Ligne vide
}

// Affiche le menu des actions disponibles
void afficher_menu_actions(EtatCombattant* joueur) {
    printf("\n%s, choisissez une action:\n", joueur->combattant->nom); // Affiche le nom du joueur
    printf("1. Attaque de base\n"); // Option d'attaque de base
    
    // Affiche les techniques disponibles
    for (int i = 0; i < MAX_TECHNIQUES; i++) { // Parcours des techniques
        Technique* tech = &joueur->combattant->techniques[i]; // Récupère la technique
        if (tech->activable && joueur->cooldowns[i] == 0) { // Si la technique est disponible
            printf("%d. %s", i+2, tech->nom); // Affiche l'option de technique
            
            // Afficher le type de technique
            switch(tech->type) {
                case 1:
                    printf(" (Dégâts");
                    if (tech->puissance > 0)
                        printf(" %.0f%%", tech->puissance * 100);
                    printf(")");
                    break;
                case 2:
                    printf(" (Soin ");
                    if (tech->puissance < 0)
                        printf(" %.0f%%", -tech->puissance * 100);
                    printf(")");
                    break;
                case 3:
                    printf(" (Bouclier)");
                    break;
                case 4:
                    printf(" (Brûlure)");
                    break;
                case 5:
                    printf(" (Boost)");
                    break;
                default:
                    break;
            }
            
            // Afficher la cible
            printf(" - Cible: %s", tech->cible);
            
            // Afficher l'effet si présent
            if (tech->Effet.possede) {
                // Selection du nom de l'effet
                char *effet_nom;

                if (strcmp(tech->Effet.nom, "EFFET_BOOST_ATTAQUE"))
                    effet_nom = "Boost d'Attaque";
                if (strcmp(tech->Effet.nom, "EFFET_BOOST_DEFENSE"))
                    effet_nom = "Boost de Défense";
                if (strcmp(tech->Effet.nom, "EFFET_BOUCLIER"))
                    effet_nom = "Bouclier";
                if (strcmp(tech->Effet.nom, "EFFET_BRULURE"))
                    effet_nom = "Brûlure";
                if (strcmp(tech->Effet.nom, "EFFET_ETOURDISSEMENT"))
                    effet_nom = "Etourdissement";
                if (strcmp(tech->Effet.nom, "EFFET_POISON"))
                    effet_nom = "Poison";
                if (strcmp(tech->Effet.nom, "EFFET_RECONSTITUTION"))
                    effet_nom = "Reconstitution";
                printf(" - Effet: %s (%d tours)", 
                       effet_nom, 
                       tech->Effet.nb_tour_actifs);
            }
            
            printf("\n"); // Nouvelle ligne
        } else if (tech->activable && joueur->cooldowns[i] > 0) {
            // Afficher aussi les techniques en cooldown mais de façon différente
            printf("  %s (en recharge: %d tours)\n", 
                   tech->nom, joueur->cooldowns[i]);
        }
    }
}

// Affiche le résultat du combat
void afficher_resultat_combat(Combat* combat) {
    bool eq1_vivant = false;
    for (int i = 0; i < combat->equipe1->member_count; i++) {
        if (!est_ko(&combat->equipe1->members[i])) {
            eq1_vivant = true;
            break;
        }
    }
    
    bool mode_jvj = true;
    for (int i = 0; i < combat->equipe2->member_count; i++) {
        for (int j = 0; j < combat->nombre_participants; j++) {
            if (combat->participants[j].combattant == &combat->equipe2->members[i] && 
                combat->participants[j].controleur == ORDI) {
                mode_jvj = false;
                break;
            }
        }
        if (!mode_jvj) break;
    }
    
    printf("\n=== FIN DU COMBAT ===\n");
    if (eq1_vivant) {
        printf("L'équipe 1 (%s) remporte la victoire!\n", mode_jvj ? "JOUEUR 1" : "JOUEUR");
    } else {
        printf("L'équipe 2 (%s) remporte la victoire!\n", mode_jvj ? "JOUEUR 2" : "ORDINATEUR");
    }
    
    // Afficher les statistiques finales
    printf("\nStatistiques finales:\n");
    afficher_statuts_combat(combat);
}