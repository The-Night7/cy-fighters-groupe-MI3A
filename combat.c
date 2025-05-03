#include "combat.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h> // pour strcmp

// Fonction interne pour calculer les dégâts infligés par une attaque ou une technique
static float calculer_degats(Combattant* attaquant, Technique* tech, Combattant* cible) {
    /* en gros je fais dégats = (attaque x puissance technique) - (défense x 0.2) */
    // Si la technique n'a pas de puissance, c'est probablement un soin
    if (tech && tech->puissance <= 0) return 0;

    // Calcul des dégâts bruts
    float puissance = tech ? tech->puissance : 1.0f; // Attaque de base = puissance 1
    float base = attaquant->attaque * puissance;
    // Correction : convertir le float en int pour le modulo
    float reduction = (cible->defense * 0.2f) + (rand() % (int)(cible->defense*0.8f));
    float degats = fmaxf(base - reduction, 0);

    // Calcul de la chance d'esquive en fonction de l'agilité de la cible
    float chance_esquive = cible->agility * 0.01f;
    if ((rand() % 100) < (chance_esquive * 100)) {
        // L'attaque est esquivée
        return 0;
    }
    return degats;
}

// Initialise la structure Combat avec les deux équipes et prépare les états des combattants
void initialiser_combat(Combat* combat, Equipe* eq1, Equipe* eq2) {
    combat->equipe1 = eq1;
    combat->equipe2 = eq2;
    combat->tour = 0;

    // Création des états de combat pour chaque combattant des deux équipes
    int total = eq1->member_count + eq2->member_count;
    combat->participants = malloc(total * sizeof(EtatCombattant));

    for (int i = 0; i < total; i++) {
        // On récupère le bon combattant selon l'équipe
        Combattant* c = (i < eq1->member_count) ? &eq1->members[i] : &eq2->members[i - eq1->member_count];
        combat->participants[i] = (EtatCombattant){
            .combattant = c,
            .turn_meter = 0.0f,
            .controleur = (i < eq1->member_count) ? JOUEUR : ORDI,
            .nb_effets = 0  // Initialisation du nombre d'effets à 0
        };
        // Initialisation des cooldowns de techniques
        for (int j = 0; j < MAX_TECHNIQUES; j++) {
            combat->participants[i].cooldowns[j] = 0;
        }
    }
    combat->nombre_participants = total;
}

// Gère le déroulement d'un tour de combat (mise à jour des jauges, actions, etc.)
void gerer_tour_combat(Combat* combat) {
    combat->tour++;

    // Décrémenter les cooldowns de toutes les techniques pour chaque combattant
    for (int i = 0; i < combat->nombre_participants; i++) {
        for (int j = 0; j < MAX_TECHNIQUES; j++) {
            if (combat->participants[i].cooldowns[j] > 0)
                combat->participants[i].cooldowns[j]--;
        }
    }


    // Application des effets de statut (buffs/debuffs)
    appliquer_effets(combat);

    // Parcours de tous les participants pour gérer leur tour
    for (int i = 0; i < combat->nombre_participants; i++) {
        if (est_ko(combat->participants[i].combattant)) continue;

        // Incrémentation de la jauge de tour selon la vitesse
        combat->participants[i].turn_meter += combat->participants[i].combattant->speed;
        if (combat->participants[i].turn_meter >= 100.0f) {
            afficher_statuts_combat(combat);
            printf("%s peut agir!\n", combat->participants[i].combattant->nom);
            
            if (combat->participants[i].controleur == JOUEUR) {
                // Tour du joueur
                afficher_menu_actions(&combat->participants[i]);
                
                int choix;
                scanf("%d", &choix);
                
                // Trouver une cible valide
                int cible_index = choisir_cible(combat, combat->participants[i].controleur);
                EtatCombattant* cible = NULL;
                
                // Trouver la cible correspondante dans participants[]
                for (int j = 0, count = 0; j < combat->nombre_participants; j++) {
                    if (!est_ko(combat->participants[j].combattant) && 
                        combat->participants[j].controleur != combat->participants[i].controleur) {
                        if (count == cible_index) {
                            cible = &combat->participants[j];
                            break;
                        }
                        count++;
                    }
                }
                
                if (choix == 1) {
                    attaque_base(&combat->participants[i], cible);
                } else if (choix >= 2 && choix <= MAX_TECHNIQUES+1) {
                    utiliser_technique(&combat->participants[i], choix-2, cible);
                }
            } else {
                // Tour de l'IA (comportement simple)
                for (int j = 0; j < combat->nombre_participants; j++) {
                    if (!est_ko(combat->participants[j].combattant) && 
                        combat->participants[j].controleur != combat->participants[i].controleur) {
                        // L'IA utilise aléatoirement une attaque de base ou une technique
                        if (rand() % 2 == 0) {
                            attaque_base(&combat->participants[i], &combat->participants[j]);
                        } else {
                            // Choisir une technique aléatoire disponible
                            for (int k = 0; k < MAX_TECHNIQUES; k++) {
                                if (combat->participants[i].cooldowns[k] == 0 && 
                                    combat->participants[i].combattant->techniques[k].activable) {
                                    utiliser_technique(&combat->participants[i], k, &combat->participants[j]);
                                    break;
                                }
                            }
                        }
                        break;
                    }
                }
            }
            // Réinitialise la jauge de tour après l'action
            combat->participants[i].turn_meter = 0;
        }
        
    }
}

// Effectue une attaque de base d'un combattant sur une cible
void attaque_base(EtatCombattant* attaquant, EtatCombattant* cible) {
    float degats = calculer_degats(attaquant->combattant, NULL, cible->combattant);
    cible->combattant->Vie.courrante -= degats;
    printf("%s attaque %s et inflige %.1f dégâts!\n",
           attaquant->combattant->nom,
           cible->combattant->nom,
           degats);
}
/*  */
void utiliser_technique(EtatCombattant* attaquant, int tech_index, EtatCombattant* cible) {
    /**
    * Utilise une technique spéciale d'un combattant sur une cible.
    * @param attaquant : le combattant qui utilise la technique
    * @param tech_index : l'indice de la technique à utiliser (0 à MAX_TECHNIQUES-1)
    * @param cible : le combattant ciblé par la technique
    */
    // Vérification de la validité de l'indice de technique
    if (tech_index < 0 || tech_index >= MAX_TECHNIQUES) {
        printf("Technique invalide.\n");
        return;
    }

    Technique* tech = &attaquant->combattant->techniques[tech_index];

    // Vérification du cooldown
    if (attaquant->cooldowns[tech_index] > 0) {
        printf("La technique %s n'est pas encore disponible (recharge : %d tours).\n", tech->nom, attaquant->cooldowns[tech_index]);
        return;
    }

    // Vérification que la technique est activable
    if (!tech->activable) {
        printf("La technique %s n'est pas activable.\n", tech->nom);
        return;
    }

    // Cas d'une technique de soin (puissance <= 0)
    if (tech->puissance <= 0) {
        float soin = -tech->puissance * attaquant->combattant->attaque; // exemple : puissance -0.3 soigne 30% de l'attaque
        cible->combattant->Vie.courrante += soin;
        // On ne dépasse pas le maximum de vie
        if (cible->combattant->Vie.courrante > cible->combattant->Vie.max)
            cible->combattant->Vie.courrante = cible->combattant->Vie.max;
        printf("%s soigne %s de %.1f points de vie !\n", attaquant->combattant->nom, cible->combattant->nom, soin);
    } else {
        // Cas d'une attaque spéciale
        float degats = attaquant->combattant->attaque * tech->puissance - cible->combattant->defense * 0.2f;
        if (degats < 0) degats = 0;

        // Gestion de l'esquive (similaire à attaque_base)
        float chance_esquive = cible->combattant->agility * 0.01f;
        if ((rand() % 100) < (chance_esquive * 100)) {
            printf("%s esquive la technique spéciale %s !\n", cible->combattant->nom, tech->nom);
            degats = 0;
        } else {
            cible->combattant->Vie.courrante -= degats;
            printf("%s utilise %s sur %s et inflige %.1f dégâts !\n", attaquant->combattant->nom, tech->nom, cible->combattant->nom, degats);
        }

        // Application d'un effet spécial si la technique en possède un
        if (tech->Effet.possede) {
            // Si l'effet est de type brûlure, l'appliquer
            if (strcmp(tech->Effet.nom, "Brûlure") == 0) {
                appliquer_effet(cible, EFFET_BRULURE, tech->Effet.nb_tour_actifs, attaquant->combattant->attaque * 0.15f);
                printf("%s enflamme %s pour %d tours!\n", 
                       attaquant->combattant->nom, cible->combattant->nom, tech->Effet.nb_tour_actifs);
            }
            // Autres effets existants...
            else {
                printf("%s applique l'effet spécial '%s' à %s pour %d tours !\n",
                       attaquant->combattant->nom, tech->Effet.nom, cible->combattant->nom, tech->Effet.nb_tour_actifs);
            }
        }
    }

    // Mise à jour du cooldown de la technique
    attaquant->cooldowns[tech_index] = tech->nb_tour_recharge;
}

// Vérifie si une équipe a gagné (si l'autre équipe n'a plus de combattants vivants)
bool verifier_victoire(Combat* combat) {
    bool eq1_vivant = false;
    bool eq2_vivant = false;

    for (int i = 0; i < combat->equipe1->member_count; i++) {
        if (!est_ko(&combat->equipe1->members[i])) {
            eq1_vivant = true;
            break;
        }
    }

    for (int i = 0; i < combat->equipe2->member_count; i++) {
        if (!est_ko(&combat->equipe2->members[i])) {
            eq2_vivant = true;
            break;
        }
    }

    // Retourne vrai si une seule équipe a encore des combattants vivants
    return !(eq1_vivant && eq2_vivant);
}

// Vérifie si le combattant est K.O. (points de vie <= 0)
bool est_ko(Combattant* c) {
    return c->Vie.courrante <= 0;
}

// Fonction pour appliquer un effet à un combattant
void appliquer_effet(EtatCombattant* cible, TypeEffet effet, int duree, float puissance) {
    // Vérifier si l'effet existe déjà
    for (int i = 0; i < cible->nb_effets; i++) {
        if (cible->effets[i].type == effet) {
            // Mettre à jour la durée et la puissance si l'effet est déjà présent
            cible->effets[i].tours_restants = duree;
            cible->effets[i].puissance = puissance;
            return;
        }
    }
    
    // Ajouter le nouvel effet s'il n'existe pas déjà et s'il y a de la place
    if (cible->nb_effets < MAX_EFFECTS) {
        cible->effets[cible->nb_effets] = (EffetTemporaire){
            .type = effet,
            .tours_restants = duree,
            .puissance = puissance
        };
        cible->nb_effets++;
        
        // Appliquer les effets immédiats si nécessaire
        switch (effet) {
            case EFFET_BOOST_ATTAQUE:
                cible->combattant->attaque *= (1 + puissance);
                break;
            case EFFET_BOOST_DEFENSE:
                cible->combattant->defense *= (1 + puissance);
                break;
            default:
                // Pas d'effet immédiat pour les autres types
                break;
        }
    }
}

void appliquer_effets(Combat* combat) {
    for (int i = 0; i < combat->nombre_participants; i++) {
        EtatCombattant* cs = &combat->participants[i];
        
        // Parcours des effets en cours
        for (int j = 0; j < cs->nb_effets; j++) {
            EffetTemporaire* eff = &cs->effets[j];
            
            switch (eff->type) {
                case EFFET_AUCUN:
                    // Ne rien faire pour EFFET_AUCUN
                    break;
                    
                case EFFET_POISON:
                    cs->combattant->Vie.courrante -= eff->puissance;
                    printf("%s subit %.1f dégâts de poison!\n", 
                           cs->combattant->nom, eff->puissance);
                    break;
                
                case EFFET_BOOST_ATTAQUE:
                    // Boost déjà appliqué lors de l'initialisation
                    break;
                    
                case EFFET_ETOURDISSEMENT:
                    // Implémenter l'effet d'étourdissement ici
                    // Par exemple, empêcher le combattant d'agir pendant ce tour
                    printf("%s est étourdi et ne peut pas agir!\n", cs->combattant->nom);
                    break;
                    
                case EFFET_BOOST_DEFENSE:
                    // Boost déjà appliqué lors de l'initialisation
                    break;
                    
                case EFFET_BRULURE:
                    // La brûlure inflige des dégâts qui augmentent à chaque tour
                    float degats_brulure = eff->puissance * (4 - eff->tours_restants);
                    cs->combattant->Vie.courrante -= degats_brulure;
                    printf("%s subit %.1f dégâts de brûlure! Les flammes s'intensifient!\n", 
                           cs->combattant->nom, degats_brulure);
                    break;
            }
            
            // Décrémenter le compteur
            eff->tours_restants--;
            
            // Si effet terminé
            if (eff->tours_restants <= 0) {
                retirer_effet(cs, eff->type);
                j--; // On revient en arrière car l'effet a été retiré
            }
        }
    }
}

// Fonction pour retirer un effet
void retirer_effet(EtatCombattant* cs, TypeEffet type) {
    for (int i = 0; i < cs->nb_effets; i++) {
        if (cs->effets[i].type == type) {
            // Annuler l'effet si nécessaire
            switch (type) {
                case EFFET_AUCUN:
                    // Ne rien faire pour EFFET_AUCUN
                    break;
                    
                case EFFET_POISON:
                    // Le poison n'a pas besoin d'être annulé, il suffit de ne plus l'appliquer
                    break;
                    
                case EFFET_BOOST_ATTAQUE:
                    cs->combattant->attaque /= (1 + cs->effets[i].puissance);
                    break;
                    
                case EFFET_ETOURDISSEMENT:
                    // L'étourdissement n'a pas besoin d'être annulé
                    printf("%s n'est plus étourdi.\n", cs->combattant->nom);
                    break;
                    
                case EFFET_BOOST_DEFENSE:
                    cs->combattant->defense /= (1 + cs->effets[i].puissance);
                    break;
                    
                case EFFET_BRULURE:
                    // La brûlure n'a pas besoin d'être annulée, elle s'éteint simplement
                    printf("Les flammes sur %s s'éteignent.\n", cs->combattant->nom);
                    break;
            }
            
            // Déplacer les effets suivants
            for (int j = i; j < cs->nb_effets - 1; j++) {
                cs->effets[j] = cs->effets[j+1];
            }
            cs->nb_effets--;
            
            printf("L'effet %d se dissipe sur %s\n", type, cs->combattant->nom);
            return;
        }
    }
}

// Nettoyage de la mémoire
void nettoyer_combat(Combat* combat) {
    if (combat->participants) {
        free(combat->participants);
        combat->participants = NULL;
    }
    combat->nombre_participants = 0;
}

// Affichage de l'état du combat
void afficher_combat(const Combat* combat) {
    printf("\n--- État du combat (Tour %d) ---\n", combat->tour);
    
    for (int i = 0; i < combat->nombre_participants; i++) {
        const EtatCombattant* cs = &combat->participants[i];
        printf("%s [%.0f/%.0f PV] - TM: %.1f - Contrôle: %s\n",
               cs->combattant->nom,
               cs->combattant->Vie.courrante,
               cs->combattant->Vie.max,
               cs->turn_meter,
               cs->controleur == JOUEUR ? "Joueur" : "IA");
               
        // Afficher les cooldowns
        for (int j = 0; j < MAX_TECHNIQUES; j++) {
            if (cs->cooldowns[j] > 0) {
                printf("  %s: %d tours\n", 
                       cs->combattant->techniques[j].nom,
                       cs->cooldowns[j]);
            }
        }
        
        // Afficher les effets actifs
        for (int j = 0; j < cs->nb_effets; j++) {
            printf("  Effet: ");
            switch (cs->effets[j].type) {
                case EFFET_POISON:
                    printf("Poison");
                    break;
                case EFFET_ETOURDISSEMENT:
                    printf("Étourdissement");
                    break;
                case EFFET_BOOST_ATTAQUE:
                    printf("Boost d'attaque");
                    break;
                case EFFET_BOOST_DEFENSE:
                    printf("Boost de défense");
                    break;
                case EFFET_BRULURE:
                    printf("Brûlure");
                    break;
                default:
                    printf("Inconnu");
                    break;
            }
            printf(" (%d tours restants)\n", cs->effets[j].tours_restants);
        }
    }
}

// Affiche les statuts des équipes
void afficher_statuts_combat(Combat* combat) {
    printf("\n=== TOUR %d ===\n", combat->tour);
    
    // Équipe 1
    printf("\nÉquipe 1:\n");
    for (int i = 0; i < combat->equipe1->member_count; i++) {
        Combattant* c = &combat->equipe1->members[i];
        printf("- %s: %.0f/%.0f PV", c->nom, c->Vie.courrante, c->Vie.max);
        if (est_ko(c)) printf(" (KO)");
        
        // Afficher les effets actifs
        for (int j = 0; j < combat->nombre_participants; j++) {
            if (combat->participants[j].combattant == c && combat->participants[j].nb_effets > 0) {
                printf(" [");
                for (int k = 0; k < combat->participants[j].nb_effets; k++) {
                    switch (combat->participants[j].effets[k].type) {
                        case EFFET_POISON:
                            printf("Poison");
                            break;
                        case EFFET_ETOURDISSEMENT:
                            printf("Étourdi");
                            break;
                        case EFFET_BOOST_ATTAQUE:
                            printf("Att+");
                            break;
                        case EFFET_BOOST_DEFENSE:
                            printf("Def+");
                            break;
                        case EFFET_BRULURE:
                            printf("Brûlure");
                            break;
                        default:
                            break;
                    }
                    if (k < combat->participants[j].nb_effets - 1) printf(", ");
                }
                printf("]");
            }
        }
        printf("\n");
    }
    
    // Équipe 2
    printf("\nÉquipe 2:\n");
    for (int i = 0; i < combat->equipe2->member_count; i++) {
        Combattant* c = &combat->equipe2->members[i];
        printf("- %s: %.0f/%.0f PV", c->nom, c->Vie.courrante, c->Vie.max);
        if (est_ko(c)) printf(" (KO)");
        
        // Afficher les effets actifs
        for (int j = 0; j < combat->nombre_participants; j++) {
            if (combat->participants[j].combattant == c && combat->participants[j].nb_effets > 0) {
                printf(" [");
                for (int k = 0; k < combat->participants[j].nb_effets; k++) {
                    switch (combat->participants[j].effets[k].type) {
                        case EFFET_POISON:
                            printf("Poison");
                            break;
                        case EFFET_ETOURDISSEMENT:
                            printf("Étourdi");
                            break;
                        case EFFET_BOOST_ATTAQUE:
                            printf("Att+");
                            break;
                        case EFFET_BOOST_DEFENSE:
                            printf("Def+");
                            break;
                        case EFFET_BRULURE:
                            printf("Brûlure");
                            break;
                        default:
                            break;
                    }
                    if (k < combat->participants[j].nb_effets - 1) printf(", ");
                }
                printf("]");
            }
        }
        printf("\n");
    }
    printf("\n");
}

// Affiche le menu des actions disponibles
void afficher_menu_actions(EtatCombattant* joueur) {
    printf("\n%s, choisissez une action:\n", joueur->combattant->nom);
    printf("1. Attaque de base\n");
    
    // Affiche les techniques disponibles
    for (int i = 0; i < MAX_TECHNIQUES; i++) {
        Technique* tech = &joueur->combattant->techniques[i];
        if (tech->activable && joueur->cooldowns[i] == 0) {
            printf("%d. %s", i+2, tech->nom);
            if (tech->puissance <= 0) printf(" (Soin)");
            printf("\n");
        }
    }
}

// Permet de choisir une cible
int choisir_cible(Combat* combat, TypeJoueur controleur) {
    printf("\nChoisissez une cible:\n");
    int index = 0;
    
    for (int i = 0; i < combat->nombre_participants; i++) {
        EtatCombattant* c = &combat->participants[i];
        if (!est_ko(c->combattant) && c->controleur != controleur) {
            printf("%d. %s (%.0f PV)", index+1, c->combattant->nom, c->combattant->Vie.courrante);
            
            // Afficher les effets actifs sur la cible
            if (c->nb_effets > 0) {
                printf(" [");
                for (int j = 0; j < c->nb_effets; j++) {
                    switch (c->effets[j].type) {
                        case EFFET_POISON:
                            printf("Poison");
                            break;
                        case EFFET_ETOURDISSEMENT:
                            printf("Étourdi");
                            break;
                        case EFFET_BOOST_ATTAQUE:
                            printf("Att+");
                            break;
                        case EFFET_BOOST_DEFENSE:
                            printf("Def+");
                            break;
                        case EFFET_BRULURE:
                            printf("Brûlure");
                            break;
                        default:
                            break;
                    }
                    if (j < c->nb_effets - 1) printf(", ");
                }
                printf("]");
            }
            printf("\n");
            index++;
        }
    }
    
    int choix;
    scanf("%d", &choix);
    return choix-1; // Retourne l'index dans la liste des cibles valides
}