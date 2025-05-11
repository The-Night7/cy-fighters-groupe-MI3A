#include "aff_combat.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h> // pour strcmp

// Affichage de l'état du combat
void afficher_combat(const Combat* combat) {
    system("clear"); // Efface l'écran
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
                case EFFET_RECONSTITUTION: // Si reconstitution
                    printf("Reconstitution"); // Affiche "Reconstitution"
                    break;
                case EFFET_BOUCLIER: // Si bouclier
                    printf("Garde du Corps"); // Affiche "Garde du Corps"
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
    afficher_combat(combat); // Affiche l'état du combat
    printf("\n=== TOUR %d ===\n", combat->tour); // Affiche le numéro du tour
    
    // Déterminer si nous sommes en mode JvJ
    bool mode_jvj = true; // Initialise le mode JvJ à vrai
    for (int i = 0; i < combat->equipe2->member_count; i++) { // Parcours l'équipe 2
        for (int j = 0; j < combat->nombre_participants; j++) { // Parcours les participants
            if (combat->participants[j].combattant == &combat->equipe2->members[i] && 
                combat->participants[j].controleur == ORDI) { // Vérifie si un membre est contrôlé par l'IA
                mode_jvj = false; // Met le mode JvJ à faux
                break;
            }
        }
        if (!mode_jvj) break;
    }
    
    // Équipe 1
    printf("\nÉquipe 1 (%s):\n", mode_jvj ? "JOUEUR 1" : "JOUEUR"); // Affiche le titre de l'équipe 1
    for (int i = 0; i < combat->equipe1->member_count; i++) { // Parcours les membres de l'équipe 1
        Combattant* c = &combat->equipe1->members[i]; // Récupère le combattant
        printf("- %s: %.0f/%.0f PV", c->nom, c->Vie.courrante, c->Vie.max); // Affiche les PV
        if (est_ko(c)) printf(" (KO)"); // Indique si KO
        
        // Afficher les effets actifs
        for (int j = 0; j < combat->nombre_participants; j++) { // Parcours les participants
            if (combat->participants[j].combattant == c && combat->participants[j].nb_effets > 0) { // Vérifie si le combattant a des effets
                printf(" ["); // Début liste effets
                for (int k = 0; k < combat->participants[j].nb_effets; k++) { // Parcours les effets
                    switch (combat->participants[j].effets[k].type) { // Selon le type d'effet
                        case EFFET_POISON:
                            printf("Poison"); // Affiche poison
                            break;
                        case EFFET_ETOURDISSEMENT:
                            printf("Étourdi"); // Affiche étourdi
                            break;
                        case EFFET_BOOST_ATTAQUE:
                            printf("Att+"); // Affiche boost attaque
                            break;
                        case EFFET_BOOST_DEFENSE:
                            printf("Def+"); // Affiche boost défense
                            break;
                        case EFFET_BRULURE:
                            printf("Brûlure"); // Affiche brûlure
                            break;
                        case EFFET_RECONSTITUTION:
                            printf("Reconst."); // Affiche reconstitution
                            break;
                        case EFFET_BOUCLIER:
                            printf("Garde"); // Affiche garde
                            break;
                        default:
                            break;
                    }
                    if (k < combat->participants[j].nb_effets - 1) printf(", "); // Ajoute virgule si pas dernier
                }
                printf("]"); // Fin liste effets
            }
        }
        printf("\n"); // Nouvelle ligne
    }
    
    // Équipe 2
    printf("\nÉquipe 2 (%s):\n", mode_jvj ? "JOUEUR 2" : "ORDINATEUR"); // Affiche titre équipe 2
    for (int i = 0; i < combat->equipe2->member_count; i++) { // Parcours membres équipe 2
        Combattant* c = &combat->equipe2->members[i]; // Récupère combattant
        printf("- %s: %.0f/%.0f PV", c->nom, c->Vie.courrante, c->Vie.max); // Affiche PV
        if (est_ko(c)) printf(" (KO)"); // Indique si KO
        
        // Afficher les effets actifs
        for (int j = 0; j < combat->nombre_participants; j++) { // Parcours participants
            if (combat->participants[j].combattant == c && combat->participants[j].nb_effets > 0) { // Vérifie effets
                printf(" ["); // Début liste
                for (int k = 0; k < combat->participants[j].nb_effets; k++) { // Parcours effets
                    switch (combat->participants[j].effets[k].type) { // Selon type
                        case EFFET_POISON:
                            printf("Poison"); // Affiche poison
                            break;
                        case EFFET_ETOURDISSEMENT:
                            printf("Étourdi"); // Affiche étourdi
                            break;
                        case EFFET_BOOST_ATTAQUE:
                            printf("Att+"); // Affiche boost attaque
                            break;
                        case EFFET_BOOST_DEFENSE:
                            printf("Def+"); // Affiche boost défense
                            break;
                        case EFFET_BRULURE:
                            printf("Brûlure"); // Affiche brûlure
                            break;
                        case EFFET_RECONSTITUTION:
                            printf("Reconst."); // Affiche reconstitution
                            break;
                        case EFFET_BOUCLIER:
                            printf("Garde"); // Affiche garde
                            break;
                        default:
                            break;
                    }
                    if (k < combat->participants[j].nb_effets - 1) printf(", "); // Ajoute virgule si pas dernier
                }
                printf("]"); // Fin liste
            }
        }
        printf("\n"); // Nouvelle ligne
    }
    printf("\n"); // Ligne vide
}

// Affiche le menu des actions disponibles
void afficher_menu_actions(EtatCombattant* joueur) {
    printf("\n%s, choisissez une action:\n", joueur->combattant->nom); // Affiche nom joueur
    printf("1. Attaque de base\n"); // Option attaque base
    
    // Affiche les techniques disponibles
    for (int i = 0; i < MAX_TECHNIQUES; i++) { // Parcours techniques
        Technique* tech = &joueur->combattant->techniques[i]; // Récupère technique
        if (tech->activable && joueur->cooldowns[i] == 0) { // Si technique disponible
            printf("%d. %s (Puissance: %.1f, Recharge: %d tours%s%s)\n", // Affiche détails technique
                i+2,
                tech->nom,
                tech->puissance,
                tech->nb_tour_recharge,
                tech->Effet.possede ? ", Effet: " : "",
                tech->Effet.possede ? obtenir_nom_effet(convertir_nom_effet(tech->Effet.nom)) : "");
        } else if (tech->activable && joueur->cooldowns[i] > 0) {
            printf("  %s (en recharge: %d tours)\n", // Affiche technique en recharge
                   tech->nom, joueur->cooldowns[i]);
        }
    }
}

// Affiche le résultat du combat
void afficher_resultat_combat(Combat* combat) {
    //system("clear"); // Efface écran
    bool eq1_vivant = false; // Initialise état équipe 1
    for (int i = 0; i < combat->equipe1->member_count; i++) { // Vérifie membres équipe 1
        if (!est_ko(&combat->equipe1->members[i])) { // Si un membre vivant
            eq1_vivant = true; // Équipe 1 vivante
            break;
        }
    }
    
    bool mode_jvj = true; // Initialise mode JvJ
    for (int i = 0; i < combat->equipe2->member_count; i++) { // Parcours équipe 2
        for (int j = 0; j < combat->nombre_participants; j++) { // Parcours participants
            if (combat->participants[j].combattant == &combat->equipe2->members[i] && 
                combat->participants[j].controleur == ORDI) { // Vérifie si IA
                mode_jvj = false; // Pas mode JvJ
                break;
            }
        }
        if (!mode_jvj) break;
    }
    
    printf("\n=== FIN DU COMBAT ===\n"); // Affiche fin combat
    if (eq1_vivant) { // Si équipe 1 gagne
        printf("L'équipe 1 (%s) remporte la victoire!\n", mode_jvj ? "JOUEUR 1" : "JOUEUR"); // Affiche victoire équipe 1
    } else {
        printf("L'équipe 2 (%s) remporte la victoire!\n", mode_jvj ? "JOUEUR 2" : "ORDINATEUR"); // Affiche victoire équipe 2
    }
    
    printf("\nStatistiques finales:\n"); // Titre stats
    afficher_statuts_combat(combat); // Affiche stats finales
}