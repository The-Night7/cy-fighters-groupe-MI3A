#include "util_combat.h"                // Inclusion du fichier d'en-tête des utilitaires de combat
#include "gest_combat.h"                // Inclusion du fichier d'en-tête pour afficher le résultat du combat
#include "gest_effets.h"                // Inclusion du fichier d'en-tête pour la gestion des effets
#include <stdlib.h>                     // Inclusion de la bibliothèque standard
#include <stdio.h>                      // Inclusion de la bibliothèque d'entrées/sorties
#include <math.h>                       // Inclusion de la bibliothèque mathématique
#include <string.h>                     // Inclusion de la bibliothèque pour les fonctions de chaînes

void utiliser_technique(EtatCombattant* attaquant, int tech_index, EtatCombattant* cible) {  // Définition de la fonction d'utilisation de technique
    /**
    * Utilise une technique spéciale d'un combattant sur une cible.
    * @param attaquant : le combattant qui utilise la technique
    * @param tech_index : l'indice de la technique à utiliser (0 à MAX_TECHNIQUES-1)
    * @param cible : le combattant ciblé par la technique
    */
    if (tech_index < 0 || tech_index >= MAX_TECHNIQUES) {    // Vérification de la validité de l'indice
        printf("Technique invalide.\n");                      // Affichage du message d'erreur
        return;                                              // Sortie de la fonction
    }

    Technique* tech = &attaquant->combattant->techniques[tech_index];   // Récupération de la technique

    if (attaquant->cooldowns[tech_index] > 0) {             // Vérification du temps de recharge
        printf("La technique %s n'est pas encore disponible (recharge : %d tours).\n", tech->nom, attaquant->cooldowns[tech_index]);  // Message d'erreur
        return;                                             // Sortie de la fonction
    }

    if (!tech->activable) {                                // Vérification de l'activation possible
        printf("La technique %s n'est pas activable.\n", tech->nom);  // Message d'erreur
        return;                                            // Sortie de la fonction
    }

    printf("\n%s utilise %s", attaquant->combattant->nom, tech->nom);  // Affichage du message d'utilisation
    if (attaquant != cible) {                             // Si la cible est différente de l'attaquant
        printf(" sur %s", cible->combattant->nom);        // Affichage de la cible
    }
    printf(" !\n");                                       // Fin du message

    switch(tech->type) {                                  // Traitement selon le type de technique
        case 1:                                          // Cas des dégâts
            {
                float degats = attaquant->combattant->attaque * tech->puissance - cible->combattant->defense * 0.2f;  // Calcul des dégâts
                if (degats < 0) degats = 0;              // Minimum de 0 dégâts
                
                cible->combattant->Vie.courrante -= degats;  // Application des dégâts
                printf("L'attaque inflige %.1f points de dégâts !\n", degats);  // Message de dégâts
            }
            break;
            
        case 2:                                         // Cas du soin
            {
                float soin = -tech->puissance * attaquant->combattant->Vie.max;  // Calcul du soin
                cible->combattant->Vie.courrante += soin;  // Application du soin
                if (cible->combattant->Vie.courrante > cible->combattant->Vie.max)  // Vérification du maximum
                    cible->combattant->Vie.courrante = cible->combattant->Vie.max;  // Limitation au maximum
                printf("Le soin restaure %.1f points de vie !\n", soin);  // Message de soin
            }
            break;
            
        case 3:                                        // Cas du bouclier
            printf("Un bouclier protecteur entoure %s !\n", cible->combattant->nom);  // Message de bouclier
            break;
            
        case 4:                                       // Cas de la brûlure
            printf("Des flammes commencent à consumer %s !\n", cible->combattant->nom);  // Message de brûlure
            break;
            
        case 5:                                      // Cas du boost
            {
                float soin = -tech->puissance * attaquant->combattant->Vie.max;  // Calcul du boost
                cible->combattant->Vie.courrante += soin;  // Application du boost
                if (cible->combattant->Vie.courrante > cible->combattant->Vie.max)  // Vérification du maximum
                    cible->combattant->Vie.courrante = cible->combattant->Vie.max;  // Limitation au maximum
                printf("Le boost restaure %.1f points de vie", soin);  // Message de boost
                
                if (tech->Effet.possede) {           // Si effet supplémentaire
                    printf(" et augmente les capacités !");  // Message d'augmentation
                }
                printf(" !\n");                      // Fin du message
            }
            break;
            
        default:                                    // Cas par défaut
            break;
    }

    if (tech->Effet.possede) {                     // Si la technique a un effet
        if (strcmp(tech->Effet.nom, "Brûlure") == 0) {  // Cas de la brûlure
            appliquer_effet(cible, EFFET_BRULURE, tech->Effet.nb_tour_actifs, attaquant->combattant->attaque * 0.15f);  // Application de l'effet
            printf("%s sera affecté par la brûlure pendant %d tours !\n", 
                   cible->combattant->nom, tech->Effet.nb_tour_actifs);  // Message d'effet
        }
        else if (strcmp(tech->Effet.nom, "Boost dégats") == 0) {  // Cas du boost de dégâts
            appliquer_effet(cible, EFFET_BOOST_ATTAQUE, tech->Effet.nb_tour_actifs, 0.3f);  // Application de l'effet
            printf("%s bénéficie d'un boost d'attaque de 30%% pendant %d tours !\n", 
                   cible->combattant->nom, tech->Effet.nb_tour_actifs);  // Message d'effet
        }
        else if (strcmp(tech->Effet.nom, "Bouclier Moyen") == 0) {  // Cas du bouclier moyen
            appliquer_effet(cible, EFFET_BOOST_DEFENSE, tech->Effet.nb_tour_actifs, 0.5f);  // Application de l'effet
            printf("%s bénéficie d'un boost de défense de 50%% pendant %d tours !\n", 
                   cible->combattant->nom, tech->Effet.nb_tour_actifs);  // Message d'effet
        }
        else if (strcmp(tech->Effet.nom, "Reconstitution") == 0) {  // Cas de la reconstitution
            printf("%s sera soigné progressivement pendant %d tours !\n", 
                   cible->combattant->nom, tech->Effet.nb_tour_actifs);  // Message d'effet
        }
        else {                                      // Cas par défaut
            printf("%s applique l'effet spécial '%s' à %s pour %d tours !\n",
                   attaquant->combattant->nom, tech->Effet.nom, cible->combattant->nom, tech->Effet.nb_tour_actifs);  // Message d'effet générique
        }
    }

    attaquant->cooldowns[tech_index] = tech->nb_tour_recharge;  // Mise à jour du temps de recharge
}

bool verifier_victoire(Combat* combat) {            // Fonction de vérification de victoire
    bool eq1_vivant = false;                       // Initialisation du statut de l'équipe 1
    bool eq2_vivant = false;                       // Initialisation du statut de l'équipe 2

    for (int i = 0; i < combat->equipe1->member_count; i++) {  // Parcours de l'équipe 1
        if (!est_ko(&combat->equipe1->members[i])) {  // Vérification si membre vivant
            eq1_vivant = true;                     // Marquage équipe 1 comme vivante
            break;                                 // Sortie de la boucle
        }
    }

    for (int i = 0; i < combat->equipe2->member_count; i++) {  // Parcours de l'équipe 2
        if (!est_ko(&combat->equipe2->members[i])) {  // Vérification si membre vivant
            eq2_vivant = true;                     // Marquage équipe 2 comme vivante
            break;                                 // Sortie de la boucle
        }
    }
    
    if (!(eq1_vivant && eq2_vivant)) {           // Si une équipe a perdu
        afficher_resultat_combat(combat);         // Affichage du résultat
        return true;                              // Retour victoire
    }
    
    return false;                                // Retour combat en cours
}

bool est_ko(Combattant* c) {                    // Fonction de vérification KO
    return c->Vie.courrante <= 0;               // Retourne vrai si plus de vie
}

void nettoyer_combat(Combat* combat) {          // Fonction de nettoyage du combat
    if (combat->participants) {                 // Si des participants existent
        free(combat->participants);             // Libération de la mémoire
        combat->participants = NULL;            // Mise à NULL du pointeur
    }
    combat->nombre_participants = 0;            // Réinitialisation du compteur
}
