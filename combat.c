#include "combat.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h> // pour strcmp

// Fonction interne pour calculer les dégâts infligés par une attaque ou une technique
static float calculer_degats(Combattant* attaquant, Technique* tech, Combattant* cible) {
    /* en gros je fais dégats = (attaque x puissance technique) - (défense x 0.2) */
    // Si la technique n'a pas de puissance, c'est probablement un soin
    if (tech && tech->puissance <= 0) return 0; // Retourne 0 si c'est un soin

    // Calcul des dégâts bruts
    float puissance = tech ? tech->puissance : 1.0f; // Attaque de base = puissance 1
    float base = attaquant->attaque * puissance; // Calcul de la base des dégâts
    // Correction : convertir le float en int pour le modulo
    float reduction = (cible->defense * 0.2f) + (rand() % (int)(cible->defense*0.8f)); // Calcul de la réduction de dégâts
    float degats = fmaxf(base - reduction, 0); // Calcul des dégâts finaux (minimum 0)

    // Calcul de la chance d'esquive en fonction de l'agilité de la cible
    float chance_esquive = cible->agility * 0.01f; // Conversion de l'agilité en pourcentage
    if ((rand() % 100) < (chance_esquive * 100)) { // Test d'esquive
        // L'attaque est esquivée
        return 0; // Retourne 0 si l'attaque est esquivée
    }
    return degats; // Retourne les dégâts calculés
}

// Initialise la structure Combat avec les deux équipes et prépare les états des combattants
void initialiser_combat(Combat* combat, Equipe* eq1, Equipe* eq2) {
    combat->equipe1 = eq1; // Assigne l'équipe 1
    combat->equipe2 = eq2; // Assigne l'équipe 2
    combat->tour = 0; // Initialise le compteur de tours à 0
    // Création des états de combat pour chaque combattant des deux équipes
    int total = eq1->member_count + eq2->member_count; // Calcul du nombre total de participants
    combat->participants = malloc(total * sizeof(EtatCombattant)); // Allocation de mémoire pour les états des combattants

    for (int i = 0; i < total; i++) { // Parcours de tous les participants
        // On récupère le bon combattant selon l'équipe
        Combattant* c = (i < eq1->member_count) ? &eq1->members[i] : &eq2->members[i - eq1->member_count]; // Sélection du combattant
        combat->participants[i] = (EtatCombattant){ // Initialisation de l'état du combattant
            .combattant = c, // Pointeur vers le combattant
            .turn_meter = 0.0f, // Jauge de tour à 0
            .controleur = (i < eq1->member_count) ? JOUEUR : ORDI, // Définition du contrôleur
            .nb_effets = 0  // Initialisation du nombre d'effets à 0
        };
        // Initialisation des cooldowns de techniques
        for (int j = 0; j < MAX_TECHNIQUES; j++) { // Parcours de toutes les techniques
            combat->participants[i].cooldowns[j] = 0; // Initialisation du cooldown à 0
        }
    }
    combat->nombre_participants = total; // Sauvegarde du nombre total de participants
}

// Gère le déroulement d'un tour de combat (mise à jour des jauges, actions, etc.)
void gerer_tour_combat(Combat* combat) {
    combat->tour++; // Incrémentation du compteur de tours
    // Décrémenter les cooldowns de toutes les techniques pour chaque combattant
    for (int i = 0; i < combat->nombre_participants; i++) { // Parcours de tous les participants
        for (int j = 0; j < MAX_TECHNIQUES; j++) { // Parcours de toutes les techniques
            if (combat->participants[i].cooldowns[j] > 0) // Si le cooldown est actif
                combat->participants[i].cooldowns[j]--; // Décrémenter le cooldown
        }
    }


    // Application des effets de statut (buffs/debuffs)
    appliquer_effets(combat); // Appel à la fonction qui applique tous les effets actifs
    // Parcours de tous les participants pour gérer leur tour
    for (int i = 0; i < combat->nombre_participants; i++) { // Parcours de tous les participants
        if (est_ko(combat->participants[i].combattant)) continue; // Passe au suivant si le combattant est KO

        // Incrémentation de la jauge de tour selon la vitesse
        combat->participants[i].turn_meter += combat->participants[i].combattant->speed; // Augmente la jauge de tour
        if (combat->participants[i].turn_meter >= 100.0f) { // Si la jauge atteint 100%
            afficher_statuts_combat(combat); // Affiche l'état actuel du combat
            printf("%s peut agir!\n", combat->participants[i].combattant->nom); // Annonce que le combattant peut agir
            
            if (combat->participants[i].controleur == JOUEUR) { // Si c'est un combattant contrôlé par le joueur
                // Tour du joueur
                int choix = -1; // Variable pour stocker le choix du joueur
                int tech_index = -1; // Initialisation de l'indice de technique à -1 (attaque de base)
                int cible_index = -1; // Initialisation de l'indice de cible
                bool action_valide = false; // Pour vérifier si l'action est valide
                
                while (!action_valide) {
                    afficher_menu_actions(&combat->participants[i]); // Affiche le menu d'actions
                    scanf("%d", &choix); // Lecture du choix
                    
                    // Déterminer quelle technique est utilisée (ou attaque de base)
                    if (choix == 1) {
                        tech_index = -1; // Attaque de base
                    } else if (choix >= 2 && choix <= MAX_TECHNIQUES+1) {
                        tech_index = choix-2; // Calcul de l'indice de la technique
                        
                        // Vérifier si la technique est valide
                        if (tech_index >= 0 && tech_index < MAX_TECHNIQUES) {
                            Technique* tech = &combat->participants[i].combattant->techniques[tech_index];
                            if (!tech->activable) {
                                printf("La technique %s n'est pas activable. Choisissez une autre action.\n", tech->nom);
                                continue; // Redemander une action
                            }
                            if (combat->participants[i].cooldowns[tech_index] > 0) {
                                printf("La technique %s n'est pas encore disponible (recharge : %d tours). Choisissez une autre action.\n", 
                                      tech->nom, combat->participants[i].cooldowns[tech_index]);
                                continue; // Redemander une action
                            }
                        } else {
                            printf("Technique invalide. Choisissez une action valide.\n");
                            continue; // Redemander une action
}
                    } else {
                        printf("Choix invalide. Choisissez une action valide.\n");
                        continue; // Redemander une action
                    }
                    
                    // Trouver une cible valide selon le type de technique
                    cible_index = choisir_cible(combat, combat->participants[i].controleur, tech_index, &combat->participants[i]);
                    
                    // Traiter les cas spéciaux
                    if (cible_index == -1) {
                        printf("Action annulée, aucune cible valide. Choisissez une autre action.\n");
                        continue; // Redemander une action
                    } else {
                        // L'action est valide
                        action_valide = true;
                    }
                }
                
                // Exécuter l'action choisie
                if (cible_index == -2) { // Si cibles multiples
                    // Cibles multiples - appliquer à toutes les cibles valides
                    if (choix == 1) { // Si attaque de base
                        printf("L'attaque de base ne peut pas cibler plusieurs ennemis.\n"); // Message d'erreur
                    } else { // Si technique spéciale
                        // Pour chaque cible valide
                        for (int j = 0; j < combat->nombre_participants; j++) { // Parcours de tous les participants
                            bool est_allie = (combat->participants[j].controleur == combat->participants[i].controleur); // Vérifie si c'est un allié
                            bool vise_allie = (combat->participants[i].combattant->techniques[tech_index].type == 2 || 
                                              combat->participants[i].combattant->techniques[tech_index].type == 3 ||
                                              combat->participants[i].combattant->techniques[tech_index].type == 5); // Vérifie si la technique vise les alliés
                                              
                            if (!est_ko(combat->participants[j].combattant) && est_allie == vise_allie) { // Si la cible est valide
                                utiliser_technique(&combat->participants[i], tech_index, &combat->participants[j]); // Utilise la technique sur la cible
                            }
                        }
                    }
                }
                else if (cible_index == -3) { // Si ciblage de soi-même
                    // Ciblage de soi-même
                    utiliser_technique(&combat->participants[i], tech_index, &combat->participants[i]); // Utilise la technique sur soi-même
                }
                else { // Si ciblage normal
                    // Ciblage normal d'une cible unique
                    EtatCombattant* cible = NULL; // Initialisation du pointeur de cible
                    
                    // Trouver la cible correspondante dans participants[]
                    bool vise_allie = (tech_index >= 0 && (combat->participants[i].combattant->techniques[tech_index].type == 2 || 
                                       combat->participants[i].combattant->techniques[tech_index].type == 3 ||
                                       combat->participants[i].combattant->techniques[tech_index].type == 5)); // Vérifie si la technique vise les alliés
                                       
                    for (int j = 0, count = 0; j < combat->nombre_participants; j++) { // Parcours de tous les participants
                        bool est_allie = (combat->participants[j].controleur == combat->participants[i].controleur); // Vérifie si c'est un allié
                        
                        if (!est_ko(combat->participants[j].combattant) && 
                            est_allie == vise_allie && 
                            &combat->participants[j] != &combat->participants[i]) { // Si la cible est valide
                            if (count == cible_index) { // Si c'est la cible choisie
                                cible = &combat->participants[j]; // Assigne la cible
                                break; // Sort de la boucle
                            }
                            count++; // Incrémente le compteur
                        }
                    }
                    
                    if (cible) { // Si une cible a été trouvée
                        if (choix == 1) { // Si attaque de base
                            attaque_base(&combat->participants[i], cible); // Effectue l'attaque de base
                        } else if (choix >= 2 && choix <= MAX_TECHNIQUES+1) { // Si technique spéciale
                            utiliser_technique(&combat->participants[i], tech_index, cible); // Utilise la technique
                        }
                    }
                }
            } else { // Si c'est un combattant contrôlé par l'IA
                // Tour de l'IA (comportement simple)
                // ... [code existant pour l'IA]
                for (int j = 0; j < combat->nombre_participants; j++) { // Parcours de tous les participants
                    if (!est_ko(combat->participants[j].combattant) && 
                        combat->participants[j].controleur != combat->participants[i].controleur) { // Si c'est un ennemi non KO
                        // L'IA utilise aléatoirement une attaque de base ou une technique
                        if (rand() % 2 == 0) { // 50% de chance d'utiliser l'attaque de base
                            attaque_base(&combat->participants[i], &combat->participants[j]); // Effectue l'attaque de base
                        } else { // 50% de chance d'utiliser une technique
                            // Choisir une technique aléatoire disponible
                            for (int k = 0; k < MAX_TECHNIQUES; k++) { // Parcours de toutes les techniques
                                if (combat->participants[i].cooldowns[k] == 0 && 
                                    combat->participants[i].combattant->techniques[k].activable) { // Si la technique est disponible
                                    utiliser_technique(&combat->participants[i], k, &combat->participants[j]); // Utilise la technique
                                    break; // Sort de la boucle
                                }
                            }
                        }
                        break; // Sort de la boucle après avoir attaqué un ennemi
                    }
                }
            }
            // Réinitialise la jauge de tour après l'action
            combat->participants[i].turn_meter = 0; // Remet la jauge à 0
        }
    }
}

// Effectue une attaque de base d'un combattant sur une cible
void attaque_base(EtatCombattant* attaquant, EtatCombattant* cible) {
    float degats = calculer_degats(attaquant->combattant, NULL, cible->combattant); // Calcul des dégâts
    cible->combattant->Vie.courrante -= degats; // Soustrait les dégâts des points de vie de la cible
    printf("%s attaque %s et inflige %.1f dégâts!\n", // Affiche un message
           attaquant->combattant->nom, // Nom de l'attaquant
           cible->combattant->nom, // Nom de la cible
           degats); // Dégâts infligés
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
    if (tech_index < 0 || tech_index >= MAX_TECHNIQUES) { // Si l'indice est invalide
        printf("Technique invalide.\n"); // Message d'erreur
        return; // Sortie de la fonction
    }

    Technique* tech = &attaquant->combattant->techniques[tech_index]; // Récupère la technique

    // Vérification du cooldown
    if (attaquant->cooldowns[tech_index] > 0) { // Si la technique est en cooldown
        printf("La technique %s n'est pas encore disponible (recharge : %d tours).\n", tech->nom, attaquant->cooldowns[tech_index]); // Message d'erreur
        return; // Sortie de la fonction
    }

    // Vérification que la technique est activable
    if (!tech->activable) { // Si la technique n'est pas activable
        printf("La technique %s n'est pas activable.\n", tech->nom); // Message d'erreur
        return; // Sortie de la fonction
    }

    // Cas d'une technique de soin (puissance <= 0)
    if (tech->puissance <= 0) { // Si c'est une technique de soin
        float soin = -tech->puissance * attaquant->combattant->attaque; // exemple : puissance -0.3 soigne 30% de l'attaque
        cible->combattant->Vie.courrante += soin; // Ajoute le soin aux points de vie de la cible
        // On ne dépasse pas le maximum de vie
        if (cible->combattant->Vie.courrante > cible->combattant->Vie.max) // Si les PV dépassent le maximum
            cible->combattant->Vie.courrante = cible->combattant->Vie.max; // Limite les PV au maximum
        printf("%s soigne %s de %.1f points de vie !\n", attaquant->combattant->nom, cible->combattant->nom, soin); // Message de soin
    } else { // Si c'est une technique offensive
        // Cas d'une attaque spéciale
        float degats = attaquant->combattant->attaque * tech->puissance - cible->combattant->defense * 0.2f; // Calcul des dégâts
        if (degats < 0) degats = 0; // Minimum 0 dégâts

        // Gestion de l'esquive (similaire à attaque_base)
        float chance_esquive = cible->combattant->agility * 0.01f; // Calcul de la chance d'esquive
        if ((rand() % 100) < (chance_esquive * 100)) { // Test d'esquive
            printf("%s esquive la technique spéciale %s !\n", cible->combattant->nom, tech->nom); // Message d'esquive
            degats = 0; // Pas de dégâts si esquive
        } else { // Si l'attaque touche
            cible->combattant->Vie.courrante -= degats; // Soustrait les dégâts des points de vie
            printf("%s utilise %s sur %s et inflige %.1f dégâts !\n", attaquant->combattant->nom, tech->nom, cible->combattant->nom, degats); // Message d'attaque
        }

        // Application d'un effet spécial si la technique en possède un
        if (tech->Effet.possede) { // Si la technique a un effet
            // Si l'effet est de type brûlure, l'appliquer
            if (strcmp(tech->Effet.nom, "Brûlure") == 0) { // Si c'est une brûlure
                appliquer_effet(cible, EFFET_BRULURE, tech->Effet.nb_tour_actifs, attaquant->combattant->attaque * 0.15f); // Applique l'effet de brûlure
                printf("%s enflamme %s pour %d tours!\n", 
                       attaquant->combattant->nom, cible->combattant->nom, tech->Effet.nb_tour_actifs); // Message d'effet
            }
            // Autres effets existants...
            else { // Si c'est un autre effet
                printf("%s applique l'effet spécial '%s' à %s pour %d tours !\n",
                       attaquant->combattant->nom, tech->Effet.nom, cible->combattant->nom, tech->Effet.nb_tour_actifs); // Message d'effet
            }
        }
    }

    // Mise à jour du cooldown de la technique
    attaquant->cooldowns[tech_index] = tech->nb_tour_recharge; // Définit le cooldown
}

// Vérifie si une équipe a gagné (si l'autre équipe n'a plus de combattants vivants)
bool verifier_victoire(Combat* combat) {
    bool eq1_vivant = false; // Initialisation du statut de l'équipe 1
    bool eq2_vivant = false; // Initialisation du statut de l'équipe 2

    for (int i = 0; i < combat->equipe1->member_count; i++) { // Parcours des membres de l'équipe 1
        if (!est_ko(&combat->equipe1->members[i])) { // Si un membre est vivant
            eq1_vivant = true; // L'équipe 1 est vivante
            break; // Sort de la boucle
        }
    }

    for (int i = 0; i < combat->equipe2->member_count; i++) { // Parcours des membres de l'équipe 2
        if (!est_ko(&combat->equipe2->members[i])) { // Si un membre est vivant
            eq2_vivant = true; // L'équipe 2 est vivante
            break; // Sort de la boucle
        }
    }
    // Retourne vrai si une seule équipe a encore des combattants vivants
    return !(eq1_vivant && eq2_vivant); // Vrai si une équipe a perdu
}

// Vérifie si le combattant est K.O. (points de vie <= 0)
bool est_ko(Combattant* c) {
    return c->Vie.courrante <= 0; // Vrai si les PV sont à 0 ou moins
}

// Fonction pour appliquer un effet à un combattant
void appliquer_effet(EtatCombattant* cible, TypeEffet effet, int duree, float puissance) {
    // Vérifier si l'effet existe déjà
    for (int i = 0; i < cible->nb_effets; i++) { // Parcours des effets existants
        if (cible->effets[i].type == effet) { // Si l'effet existe déjà
            // Mettre à jour la durée et la puissance si l'effet est déjà présent
            cible->effets[i].tours_restants = duree; // Met à jour la durée
            cible->effets[i].puissance = puissance; // Met à jour la puissance
            return; // Sortie de la fonction
        }
    }
    
    // Ajouter le nouvel effet s'il n'existe pas déjà et s'il y a de la place
    if (cible->nb_effets < MAX_EFFECTS) { // S'il reste de la place
        cible->effets[cible->nb_effets] = (EffetTemporaire){ // Crée un nouvel effet
            .type = effet, // Type d'effet
            .tours_restants = duree, // Durée
            .puissance = puissance // Puissance
        };
        cible->nb_effets++; // Incrémente le nombre d'effets
        // Appliquer les effets immédiats si nécessaire
        switch (effet) { // Selon le type d'effet
            case EFFET_BOOST_ATTAQUE: // Si boost d'attaque
                cible->combattant->attaque *= (1 + puissance); // Augmente l'attaque
                break;
            case EFFET_BOOST_DEFENSE: // Si boost de défense
                cible->combattant->defense *= (1 + puissance); // Augmente la défense
                break;
            default: // Autres effets
                // Pas d'effet immédiat pour les autres types
                break;
        }
    }
}

void appliquer_effets(Combat* combat) {
    for (int i = 0; i < combat->nombre_participants; i++) { // Parcours de tous les participants
        EtatCombattant* cs = &combat->participants[i]; // Récupère l'état du combattant
        
        // Parcours des effets en cours
        for (int j = 0; j < cs->nb_effets; j++) { // Parcours des effets du combattant
            EffetTemporaire* eff = &cs->effets[j]; // Récupère l'effet
            
            switch (eff->type) { // Selon le type d'effet
                case EFFET_AUCUN: // Si aucun effet
                    // Ne rien faire pour EFFET_AUCUN
                    break;
                    
                case EFFET_POISON: // Si poison
                    cs->combattant->Vie.courrante -= eff->puissance; // Inflige des dégâts de poison
                    printf("%s subit %.1f dégâts de poison!\n", 
                           cs->combattant->nom, eff->puissance); // Message de poison
                    break;
                
                case EFFET_BOOST_ATTAQUE: // Si boost d'attaque
                    // Boost déjà appliqué lors de l'initialisation
                    break;
                    
                case EFFET_ETOURDISSEMENT: // Si étourdissement
                    // Implémenter l'effet d'étourdissement ici
                    // Par exemple, empêcher le combattant d'agir pendant ce tour
                    printf("%s est étourdi et ne peut pas agir!\n", cs->combattant->nom); // Message d'étourdissement
                    break;
                    
                case EFFET_BOOST_DEFENSE: // Si boost de défense
                    // Boost déjà appliqué lors de l'initialisation
                    break;
                    
                case EFFET_BRULURE: // Si brûlure
                    // La brûlure inflige des dégâts qui augmentent à chaque tour
                    float degats_brulure = eff->puissance * (4 - eff->tours_restants); // Calcul des dégâts de brûlure
                    cs->combattant->Vie.courrante -= degats_brulure; // Inflige les dégâts
                    printf("%s subit %.1f dégâts de brûlure! Les flammes s'intensifient!\n", 
                           cs->combattant->nom, degats_brulure); // Message de brûlure
                    break;
            }
            
            // Décrémenter le compteur
            eff->tours_restants--; // Réduit la durée restante
            
            // Si effet terminé
            if (eff->tours_restants <= 0) { // Si l'effet est terminé
                retirer_effet(cs, eff->type); // Retire l'effet
                j--; // On revient en arrière car l'effet a été retiré
            }
        }
    }
}

// Fonction pour retirer un effet
void retirer_effet(EtatCombattant* cs, TypeEffet type) {
    for (int i = 0; i < cs->nb_effets; i++) { // Parcours des effets
        if (cs->effets[i].type == type) { // Si c'est l'effet recherché
            // Annuler l'effet si nécessaire
            switch (type) { // Selon le type d'effet
                case EFFET_AUCUN: // Si aucun effet
                    // Ne rien faire pour EFFET_AUCUN
                    break;
                    
                case EFFET_POISON: // Si poison
                    // Le poison n'a pas besoin d'être annulé, il suffit de ne plus l'appliquer
                    break;
                    
                case EFFET_BOOST_ATTAQUE: // Si boost d'attaque
                    cs->combattant->attaque /= (1 + cs->effets[i].puissance); // Annule le boost d'attaque
                    break;
                    
                case EFFET_ETOURDISSEMENT: // Si étourdissement
                    // L'étourdissement n'a pas besoin d'être annulé
                    printf("%s n'est plus étourdi.\n", cs->combattant->nom); // Message de fin d'étourdissement
                    break;
                    
                case EFFET_BOOST_DEFENSE: // Si boost de défense
                    cs->combattant->defense /= (1 + cs->effets[i].puissance); // Annule le boost de défense
                    break;
                    
                case EFFET_BRULURE: // Si brûlure
                    // La brûlure n'a pas besoin d'être annulée, elle s'éteint simplement
                    printf("Les flammes sur %s s'éteignent.\n", cs->combattant->nom); // Message de fin de brûlure
                    break;
            }
            
            // Déplacer les effets suivants
            for (int j = i; j < cs->nb_effets - 1; j++) { // Décale les effets suivants
                cs->effets[j] = cs->effets[j+1]; // Copie l'effet suivant
            }
            cs->nb_effets--; // Réduit le nombre d'effets
            printf("L'effet %d se dissipe sur %s\n", type, cs->combattant->nom); // Message de fin d'effet
            return; // Sortie de la fonction
        }
    }
}

// Nettoyage de la mémoire
void nettoyer_combat(Combat* combat) {
    if (combat->participants) { // Si les participants existent
        free(combat->participants); // Libère la mémoire
        combat->participants = NULL; // Met le pointeur à NULL
    }
    combat->nombre_participants = 0; // Réinitialise le nombre de participants
}

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
    
    // Équipe 1
    printf("\nÉquipe 1:\n"); // Titre de l'équipe 1
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
    printf("\nÉquipe 2:\n"); // Titre de l'équipe 2
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
            if (tech->puissance <= 0) printf(" (Soin)"); // Indique si c'est un soin
            printf("\n"); // Nouvelle ligne
        }
    }
}

// Permet de choisir une cible en fonction du type de technique et de ses cibles
int choisir_cible(Combat* combat, TypeJoueur controleur, int tech_index, EtatCombattant* attaquant) {
    // Si tech_index est -1, c'est une attaque de base (vise toujours un ennemi unique)
    bool vise_allie = false; // Par défaut, on vise un ennemi
    bool cibles_multiples = false; // Par défaut, on vise une seule cible
    
    if (tech_index >= 0 && tech_index < MAX_TECHNIQUES) { // Si c'est une technique valide
        Technique* tech = &attaquant->combattant->techniques[tech_index]; // Récupère la technique
        
        // Déterminer si la technique vise un allié ou un ennemi
        switch (tech->type) { // Selon le type de technique
            case 1: // dégâts
            case 4: // brûlure
                vise_allie = false; // Vise un ennemi
                break;
            case 2: // soin
            case 3: // bouclier
            case 5: // autre effet positif
                vise_allie = true; // Vise un allié
                break;
            default: // Autre type
                vise_allie = false; // Par défaut, vise un ennemi
        }
        
        // Déterminer si la technique vise une ou plusieurs cibles
        cibles_multiples = (tech->ncible == 2); // Si ncible = 2, vise plusieurs cibles
    }
    
    // Si la technique vise plusieurs cibles, pas besoin de choisir
    if (cibles_multiples) { // Si cibles multiples
        printf("\nLa technique cible toutes les %s!\n", vise_allie ? "alliés" : "ennemis"); // Message d'information
        return -2; // Code spécial pour indiquer que toutes les cibles sont sélectionnées
    }
    
    // Vérifier s'il y a des cibles valides
    bool cibles_valides_existent = false;
    if (vise_allie && attaquant->combattant->Vie.courrante > 0) {
        cibles_valides_existent = true; // Le joueur lui-même est une cible valide
    }
    
    for (int i = 0; i < combat->nombre_participants; i++) {
        EtatCombattant* c = &combat->participants[i];
        bool est_allie = (c->controleur == controleur);
        
        if (!est_ko(c->combattant) && (est_allie == vise_allie) && c != attaquant) {
            cibles_valides_existent = true;
            break;
        }
    }
    
    if (!cibles_valides_existent) {
        printf("Aucune cible valide disponible!\n");
        return -1;
    }
    
    int choix = -1;
    bool choix_valide = false;
    
    while (!choix_valide) {
        printf("\nChoisissez une cible:\n"); // Demande de choisir une cible
        int index = 0; // Initialisation de l'index
        // Pour les techniques qui visent des alliés, inclure le lanceur lui-même comme option
        if (vise_allie) { // Si on vise un allié
            printf("0. %s (soi-même) (%.0f PV)\n", attaquant->combattant->nom, attaquant->combattant->Vie.courrante); // Option de se cibler soi-même
            index = 1; // L'index commence à 1
        }
        
        for (int i = 0; i < combat->nombre_participants; i++) { // Parcours de tous les participants
            EtatCombattant* c = &combat->participants[i]; // Récupère l'état du combattant
            
            // Filtrer les cibles selon qu'on vise un allié ou un ennemi
            bool est_allie = (c->controleur == controleur); // Vérifie si c'est un allié
            if (!est_ko(c->combattant) && (est_allie == vise_allie) && c != attaquant) { // Si la cible est valide
                printf("%d. %s (%.0f PV)", index, c->combattant->nom, c->combattant->Vie.courrante); // Affiche l'option de cible
                
                // Afficher les effets actifs sur la cible
                if (c->nb_effets > 0) { // Si la cible a des effets
                    printf(" ["); // Début de la liste d'effets
                    for (int j = 0; j < c->nb_effets; j++) { // Parcours des effets
                        switch (c->effets[j].type) { // Selon le type d'effet
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
                        if (j < c->nb_effets - 1) printf(", "); // Ajoute une virgule si ce n'est pas le dernier effet
                    }
                    printf("]"); // Fin de la liste d'effets
                }
                printf("\n"); // Nouvelle ligne
                index++; // Incrémente l'index
            }
        }
        
        scanf("%d", &choix); // Lecture du choix
        
        // Si on vise soi-même
        if (vise_allie && choix == 0) { // Si on se cible soi-même
            choix_valide = true;
            return -3; // Code spécial pour indiquer "soi-même"
        }
        
        // Vérifier si le choix est valide
        int max_index = index - 1;
        if (vise_allie) {
            max_index = index;
        }
        
        if (choix >= 0 && choix <= max_index) {
            choix_valide = true;
        } else {
            printf("Choix invalide. Veuillez choisir une cible valide.\n");
        }
    }
    
    // Ajuster l'index si on vise des alliés (car on a ajouté l'option "soi-même")
    if (vise_allie) { // Si on vise un allié
        choix--; // Décrémente le choix
    }
    
    return choix; // Retourne l'index dans la liste des cibles valides
}