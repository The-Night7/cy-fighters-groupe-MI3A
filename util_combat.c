#include "util_combat.h"
#include "gest_combat.h"    // Pour afficher_resultat_combat
#include "gest_effets.h"    // Pour appliquer_effet et TypeEffet
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h> // pour strcmp

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

    // Message d'utilisation de la technique
    printf("\n%s utilise %s", attaquant->combattant->nom, tech->nom);
    if (attaquant != cible) {
        printf(" sur %s", cible->combattant->nom);
    }
    printf(" !\n");

    // Traitement selon le type de technique
    switch(tech->type) {
        case 1: // Dégâts
            {
                float degats = attaquant->combattant->attaque * tech->puissance - cible->combattant->defense * 0.2f;
                if (degats < 0) degats = 0;
                
                // Gestion de l'esquive
                float chance_esquive = cible->combattant->agility * 0.01f;
                if ((rand() % 100) < (chance_esquive * 100)) {
                    printf("%s esquive l'attaque !\n", cible->combattant->nom);
                } else {
                    cible->combattant->Vie.courrante -= degats;
                    printf("L'attaque inflige %.1f points de dégâts !\n", degats);
                }
            }
            break;
            
        case 2: // Soin
            {
                float soin = -tech->puissance * attaquant->combattant->Vie.max;
                cible->combattant->Vie.courrante += soin;
                if (cible->combattant->Vie.courrante > cible->combattant->Vie.max)
                    cible->combattant->Vie.courrante = cible->combattant->Vie.max;
                printf("Le soin restaure %.1f points de vie !\n", soin);
            }
            break;
            
        case 3: // Bouclier
            printf("Un bouclier protecteur entoure %s !\n", cible->combattant->nom);
            break;
            
        case 4: // Brûlure
            printf("Des flammes commencent à consumer %s !\n", cible->combattant->nom);
            break;
            
        case 5: // Boost
            {
                float soin = -tech->puissance * attaquant->combattant->Vie.max;
                cible->combattant->Vie.courrante += soin;
                if (cible->combattant->Vie.courrante > cible->combattant->Vie.max)
                    cible->combattant->Vie.courrante = cible->combattant->Vie.max;
                printf("Le boost restaure %.1f points de vie", soin);
                
                if (tech->Effet.possede) {
                    printf(" et augmente les capacités !");
                }
                printf(" !\n");
            }
            break;
            
        default:
            break;
    }

    // Application d'un effet spécial si la technique en possède un
    if (tech->Effet.possede) {
        // Si l'effet est de type brûlure, l'appliquer
        if (strcmp(tech->Effet.nom, "Brûlure") == 0) {
            appliquer_effet(cible, EFFET_BRULURE, tech->Effet.nb_tour_actifs, attaquant->combattant->attaque * 0.15f);
            printf("%s sera affecté par la brûlure pendant %d tours !\n", 
                   cible->combattant->nom, tech->Effet.nb_tour_actifs);
        }
        else if (strcmp(tech->Effet.nom, "Boost dégats") == 0) {
            appliquer_effet(cible, EFFET_BOOST_ATTAQUE, tech->Effet.nb_tour_actifs, 0.3f); // 30% boost d'attaque
            printf("%s bénéficie d'un boost d'attaque de 30%% pendant %d tours !\n", 
                   cible->combattant->nom, tech->Effet.nb_tour_actifs);
        }
        else if (strcmp(tech->Effet.nom, "Bouclier Moyen") == 0) {
            appliquer_effet(cible, EFFET_BOOST_DEFENSE, tech->Effet.nb_tour_actifs, 0.5f); // 50% boost de défense
            printf("%s bénéficie d'un boost de défense de 50%% pendant %d tours !\n", 
                   cible->combattant->nom, tech->Effet.nb_tour_actifs);
        }
        else if (strcmp(tech->Effet.nom, "Reconstitution") == 0) {
            // Effet de soin sur la durée déjà géré par la fonction appliquer_effets
            printf("%s sera soigné progressivement pendant %d tours !\n", 
                   cible->combattant->nom, tech->Effet.nb_tour_actifs);
        }
        else {
            printf("%s applique l'effet spécial '%s' à %s pour %d tours !\n",
                   attaquant->combattant->nom, tech->Effet.nom, cible->combattant->nom, tech->Effet.nb_tour_actifs);
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
    
    // Si une équipe a gagné, afficher le résultat
    if (!(eq1_vivant && eq2_vivant)) {
        afficher_resultat_combat(combat);
        return true;
    }
    
    return false;
}

// Vérifie si le combattant est K.O. (points de vie <= 0)
bool est_ko(Combattant* c) {
    return c->Vie.courrante <= 0; // Vrai si les PV sont à 0 ou moins
}

// Nettoyage de la mémoire
void nettoyer_combat(Combat* combat) {
    if (combat->participants) { // Si les participants existent
        free(combat->participants); // Libère la mémoire
        combat->participants = NULL; // Met le pointeur à NULL
    }
    combat->nombre_participants = 0; // Réinitialise le nombre de participants
}
