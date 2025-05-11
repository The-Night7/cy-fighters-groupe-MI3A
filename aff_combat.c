#include "aff_combat.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h> // pour strcmp

void afficher_barre_vie(float pv_courant, float pv_max, int largeur) {
    int nb_hash = (int)((pv_courant / pv_max) * largeur);
    int nb_space = largeur - nb_hash;
    printf("[");
    for (int i = 0; i < nb_hash; i++) printf("#");
    for (int i = 0; i < nb_space; i++) printf(" ");
    printf("]");
}

void afficher_combat_style(const Combat* combat) {
    // Efface l'écran
    system("clear");
    // Largeur totale de 65 caractères pour un meilleur alignement
    printf("╔═════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                          État du Combat (Tour %-3d)                      ║\n", combat->tour);
    printf("╠═════════════════════════════════════════════════════════════════════════╣\n");

    // Affichage pour chaque équipe
    for (int eq = 0; eq < 2; eq++) {
        Equipe* equipe = (eq == 0) ? combat->equipe1 : combat->equipe2;
        printf("║ [EQUIPE %d] %-58s ║\n", eq + 1, equipe->name);
        printf("╟─────────────────────────────────────────────────────────────────────────╢\n");
        
        for (int i = 0; i < equipe->member_count; i++) {
            EtatCombattant* cs = NULL;
            for (int j = 0; j < combat->nombre_participants; j++) {
                if (combat->participants[j].combattant == &equipe->members[i]) {
                    cs = &combat->participants[j];
                    break;
                }
            }
            if (!cs) continue;

            char buffer[70] = {0}; // Buffer pour construire la ligne
            int pos = 0;

            // Nom + statut KO (15 caractères fixes)
            pos += snprintf(buffer + pos, sizeof(buffer) - pos, "%-12s%s", 
                cs->combattant->nom,
                est_ko(cs->combattant) ? "(KO) " : "     ");
            // Barre de vie (20 caractères)
            pos += snprintf(buffer + pos, sizeof(buffer) - pos, " ");
            afficher_barre_vie(cs->combattant->Vie.courrante, cs->combattant->Vie.max, 20);
            pos += strlen(buffer);
            
            // PV numériques (15 caractères)
            pos += snprintf(buffer + pos, sizeof(buffer) - pos, " %4.0f/%-4.0f ", 
                cs->combattant->Vie.courrante, cs->combattant->Vie.max);

            // Effets spéciaux et TM
            pos += snprintf(buffer + pos, sizeof(buffer) - pos, "│ ");
            int effets_affiches = 0;
            for (int k = 0; k < cs->nb_effets && effets_affiches < 3; k++) {
                char effet_char = '?';
                // Dans la fonction afficher_combat_style, remplacer le switch par :
                switch(cs->effets[k].type) {
                    case EFFET_POISON: effet_char = 'P'; break;
                    case EFFET_ETOURDISSEMENT: effet_char = 'E'; break;
                    case EFFET_BOOST_ATTAQUE: effet_char = 'A'; break;
                    case EFFET_BOOST_DEFENSE: effet_char = 'D'; break;
                    case EFFET_BRULURE: effet_char = 'B'; break;
                    case EFFET_BOUCLIER: effet_char = 'S'; break;
                    case EFFET_BOOST_VITESSE: effet_char = 'V'; break;
                    case EFFET_RECONSTITUTION: effet_char = 'R'; break;
                    case EFFET_PROVOCATION: effet_char = 'T'; break;
                    case EFFET_VOL_DE_VIE: effet_char = 'L'; break;
                    case EFFET_AUCUN:
                    default: effet_char = '?'; break;
                }
                pos += snprintf(buffer + pos, sizeof(buffer) - pos, "%c%d ", 
                    effet_char, cs->effets[k].tours_restants);
                effets_affiches++;
            }

            // Remplir l'espace restant
            for (int k = effets_affiches; k < 3; k++) {
                pos += snprintf(buffer + pos, sizeof(buffer) - pos, "    ");
        }
            
            // Jauge de tour
            pos += snprintf(buffer + pos, sizeof(buffer) - pos, "│ TM:%3.0f%% ", cs->turn_meter);

            printf("║ %-69s ║\n", buffer);
    }

        if (eq == 0) {
            printf("╠═════════════════════════════════════════════════════════════════════════╣\n");
        }
    }


    // Affichage des techniques spéciales
    printf("╠═════════════════════════════════════════════════════════════════════════╣\n");
    EtatCombattant* joueur = &combat->participants[0];
    printf("║ TECHNIQUES DE %-52s ║\n", joueur->combattant->nom);
    printf("╟─────────────────────────────────────────────────────────────────────────╢\n");
    
    for (int t = 0; t < MAX_TECHNIQUES; t++) {
        Technique* tech = &joueur->combattant->techniques[t];
        if (!tech->activable) continue;
        
        char buffer[70];
        snprintf(buffer, sizeof(buffer), "[%d] %-20s │ %s │ Durée: %2d tours",
            t+1, tech->nom,
            joueur->cooldowns[t] > 0 ? "Recharge: %2d tours " : "    Disponible    ",
            tech->Effet.nb_tour_actifs);
        printf("║ %-69s ║\n", buffer);
        
        // Description de la technique sur une ligne séparée
        if (tech->description[0] != '\0') {
            printf("║     └─ %-56s ║\n", tech->description);
}
    }
    
    // Ajout d'une section pour les logs du combat
    printf("╠═════════════════════════════════════════════════════════════════════════╣\n");
    printf("║                           DERNIÈRES ACTIONS                             ║\n");
    printf("╟─────────────────────────────────────────────────────────────────────────╢\n");

    // Affichage des derniers logs
    for (int i = 0; i < logs_combat.nombre; i++) {
        int index = (logs_combat.debut + i) % MAX_LOGS;
        printf("║ %-69s ║\n", logs_combat.messages[index]);
}

    // Remplir l'espace si moins de MAX_LOGS messages
    for (int i = logs_combat.nombre; i < MAX_LOGS; i++) {
        printf("║ %-69s ║\n", "");
    }
    
    printf("╚═════════════════════════════════════════════════════════════════════════╝\n");
}

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
    afficher_combat_style(combat); // Affiche l'état du combat
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
    printf("\n%s, choisissez une action:\n", joueur->combattant->nom);
    printf("1. Attaque de base\n");
    
    // Affiche les techniques disponibles
    for (int i = 0; i < MAX_TECHNIQUES; i++) {
        Technique* tech = &joueur->combattant->techniques[i];
        if (tech->activable && joueur->cooldowns[i] == 0) {
            // Déclarer la variable avant le printf
            const char* effet_nom = tech->Effet.possede ? 
                obtenir_nom_effet(convertir_nom_effet(tech->Effet.nom)) : "";
            
            // Un seul printf avec tous les paramètres
            printf("%d. %s (Puissance: %.1f, Recharge: %d tours%s%s)\n",
            i+2,
            tech->nom,
            tech->puissance,
            tech->nb_tour_recharge,
                tech->Effet.possede ? ", Effet: " : "",
                effet_nom);
        } else if (tech->activable && joueur->cooldowns[i] > 0) {
            printf("  %s (en recharge: %d tours)\n",
                   tech->nom, joueur->cooldowns[i]);
        }
    }
}

// Affiche le résultat du combat
void afficher_resultat_combat(Combat* combat) {
    system("clear"); // Efface écran
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