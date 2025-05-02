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
    float reduction = cible->defense * 0.2f;
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
    combat->participants = malloc(total * sizeof(CombatantState));

    for (int i = 0; i < total; i++) {
        // On récupère le bon combattant selon l'équipe
        Combattant* c = (i < eq1->member_count) ? &eq1->members[i] : &eq2->members[i - eq1->member_count];
        combat->participants[i] = (CombatantState){
            .combattant = c,
            .turn_meter = 0.0f,
            .controleur = (i < eq1->member_count) ? JOUEUR : ORDI
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
            printf("%s peut agir!\n", combat->participants[i].combattant->nom);

            // Ici, il faudra intégrer l'IHM pour le choix de l'action
            // Par défaut : attaque le premier adversaire encore en vie
            for (int j = 0; j < combat->nombre_participants; j++) {
                if (combat->participants[j].combattant->Vie.courrante > 0 &&
                    combat->participants[j].controleur != combat->participants[i].controleur) {

                    attaque_base(&combat->participants[i], &combat->participants[j]);
                    break;
                }
            }

            // Réinitialise la jauge de tour après l'action
            combat->participants[i].turn_meter = 0;
        }
    }
}

// Effectue une attaque de base d'un combattant sur une cible
void attaque_base(CombatantState* attaquant, CombatantState* cible) {
    float degats = calculer_degats(attaquant->combattant, NULL, cible->combattant);
    cible->combattant->Vie.courrante -= degats;
    printf("%s attaque %s et inflige %.1f dégâts!\n",
           attaquant->combattant->nom,
           cible->combattant->nom,
           degats);
}
/*  */
void utiliser_technique(CombatantState* attaquant, int tech_index, CombatantState* cible) {
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
            // Ici tu pourrais ajouter l'effet dans une liste d'effets temporaires sur la cible
            // Exemple d'affichage :
            printf("%s applique l'effet spécial '%s' à %s pour %d tours !\n",
                   attaquant->combattant->nom, tech->Effet.nom, cible->combattant->nom, tech->Effet.nb_tour_actifs);
            // À compléter selon ta structure d'effets temporaires
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

// Fonctions à implémenter :
// - Système d'effets temporaires
// - Interface utilisateur plus élaborée
// - Intelligence artificielle pour l'ordinateur
