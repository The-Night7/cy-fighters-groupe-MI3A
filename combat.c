#include "combat.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h> // pour strcmp

// Fonction interne pour calculer les dégâts infligés par une attaque ou une technique
float calculer_degats(Combattant* attaquant, Technique* tech, Combattant* cible) {
    /* en gros je fais dégats = (attaque attaquant x puissance technique) - (défense x 0.2) */
    
    // Si tech est NULL (attaque de base), on utilise une puissance par défaut
    float puissance = 0.5; // Puissance par défaut pour l'attaque de base
    
    if (tech) {
        if (tech->puissance <= 0) return 0; // si puissance négative alors bobye
        puissance = tech->puissance;
    }
    
    float base_damage = attaquant->attaque * puissance;
    float reduction = cible->defense / 100.0 * 20;  // abracadabra c'est un poucentage
    float damage = base_damage - reduction;
    
    // Variation aléatoire des dégâts (+/- 10%)
    float variation = (float)((rand() % 20) - 10) / 100.0;
    damage = damage * (1 + variation); // équivalent coup critique
    
    return damage > 0 ? damage : 0;
}

// Ajout de la fonction pour initialiser un combat en mode JvJ ou JvO
void initialiser_combat_mode(Combat* combat, Equipe* eq1, Equipe* eq2, bool mode_jvj) {
    combat->equipe1 = eq1;
    combat->equipe2 = eq2;
    combat->tour = 0;
    
    int total = eq1->member_count + eq2->member_count;
    combat->participants = malloc(total * sizeof(EtatCombattant));

    for (int i = 0; i < total; i++) {
        // On récupère le bon combattant selon l'équipe
        Combattant* c = (i < eq1->member_count) ? &eq1->members[i] : &eq2->members[i - eq1->member_count];
        
        // Si mode JvJ, les deux équipes sont contrôlées par des joueurs
        // Si mode JvO, seule l'équipe 1 est contrôlée par un joueur
        TypeJoueur controleur;
        if (mode_jvj) {
            // En mode JvJ, l'équipe 1 est contrôlée par le Joueur 1 et l'équipe 2 par le Joueur 2
            controleur = JOUEUR;
        } else {
            // En mode JvO, l'équipe 1 est contrôlée par le Joueur et l'équipe 2 par l'Ordinateur
            controleur = (i < eq1->member_count) ? JOUEUR : ORDI;
        }
        
        combat->participants[i] = (EtatCombattant){
            .combattant = c,
            .turn_meter = 0.0f,
            .controleur = controleur,
            .nb_effets = 0
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
    combat->tour++; // Incrémente le compteur de tours
    printf("\n=== DÉBUT DU TOUR %d ===\n", combat->tour); // Affiche le numéro du tour
    
    // Appliquer les effets en cours (poison, brûlure, etc.)
    appliquer_effets(combat);
    
    // Vérifier si une équipe a gagné après les effets
    if (verifier_victoire(combat)) {
        return;
    }
    
    // Mise à jour des jauges de tour pour tous les combattants
    for (int i = 0; i < combat->nombre_participants; i++) {
        EtatCombattant* cs = &combat->participants[i];
        if (!est_ko(cs->combattant)) {
            cs->turn_meter += cs->combattant->speed; // Augmente la jauge de tour
        }
    }
    
    // Traiter les actions des combattants dont la jauge est pleine
    bool action_effectuee = false;
    do {
        action_effectuee = false;
        
        // Trouver le combattant avec la jauge la plus élevée
        int index_max = -1;
        float tm_max = 0;
        
        for (int i = 0; i < combat->nombre_participants; i++) {
            EtatCombattant* cs = &combat->participants[i];
            if (!est_ko(cs->combattant) && cs->turn_meter >= 100 && cs->turn_meter > tm_max) {
                index_max = i;
                tm_max = cs->turn_meter;
            }
        }
        
        // Si un combattant peut agir
        if (index_max != -1) {
            EtatCombattant* acteur = &combat->participants[index_max];
            acteur->turn_meter -= 100; // Réinitialise la jauge (maintient l'excès)
            
            // Vérifier si le combattant est étourdi
            bool est_etourdi = false;
            for (int j = 0; j < acteur->nb_effets; j++) {
                if (acteur->effets[j].type == EFFET_ETOURDISSEMENT) {
                    est_etourdi = true;
                    printf("%s est étourdi et passe son tour!\n", acteur->combattant->nom);
                    break;
                }
            }
            
            if (!est_etourdi) {
                // Gérer l'action selon le type de contrôleur
                if (acteur->controleur == JOUEUR) {
                    // Tour du joueur humain
                    afficher_statuts_combat(combat);
                    
                    // Déterminer si le joueur est de l'équipe 1 ou 2
                    bool est_equipe1 = false;
                    for (int i = 0; i < combat->equipe1->member_count; i++) {
                        if (acteur->combattant == &combat->equipe1->members[i]) {
                            est_equipe1 = true;
                            break;
                        }
                    }
                    
                    printf("\nC'est au tour de %s (%s) d'agir!\n", 
                           acteur->combattant->nom, 
                           est_equipe1 ? "Joueur 1" : "Joueur 2");
                    
                    gerer_tour_joueur(combat, acteur);
                } else {
                    // Tour de l'IA
                    printf("\nC'est au tour de %s (IA) d'agir!\n", acteur->combattant->nom);
                    // Logique simple pour l'IA: attaque de base sur un ennemi aléatoire
                    
                    // Trouver les cibles valides
                    int nb_cibles = 0;
                    for (int i = 0; i < combat->nombre_participants; i++) {
                        EtatCombattant* c = &combat->participants[i];
                        if (!est_ko(c->combattant) && c->controleur != acteur->controleur) {
                            nb_cibles++;
                        }
                    }
                    
                    if (nb_cibles > 0) {
                        int cible_aleatoire = rand() % nb_cibles;
                        int index_cible = 0;
                        
                        for (int i = 0; i < combat->nombre_participants; i++) {
                            EtatCombattant* c = &combat->participants[i];
                            if (!est_ko(c->combattant) && c->controleur != acteur->controleur) {
                                if (index_cible == cible_aleatoire) {
                                    attaque_base(acteur, c);
                                    break;
                                }
                                index_cible++;
                            }
                        }
                    }
                }
            }
            
            action_effectuee = true;
            
            // Vérifier si une équipe a gagné après cette action
            if (verifier_victoire(combat)) {
                return;
            }
        }
    } while (action_effectuee);
    
    // Réduire les cooldowns à la fin du tour
    for (int i = 0; i < combat->nombre_participants; i++) {
        EtatCombattant* cs = &combat->participants[i];
        if (!est_ko(cs->combattant)) {
            for (int j = 0; j < MAX_TECHNIQUES; j++) {
                if (cs->cooldowns[j] > 0) {
                    cs->cooldowns[j]--;
                }
            }
        }
    }
    
    printf("\n=== FIN DU TOUR %d ===\n", combat->tour);
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
            
            // Afficher le type de technique
            switch(tech->type) {
                case 1:
                    printf(" (Dégâts");
                    if (tech->puissance > 0)
                        printf(" %.0f%%", tech->puissance * 100);
                    printf(")");
                    break;
                case 2:
                    printf(" (Soin");
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
                printf(" - Effet: %s (%d tours)", 
                       tech->Effet.nom, 
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

// Permet de choisir une cible en fonction du type de technique et de ses cibles
int choisir_cible(Combat* combat, TypeJoueur controleur, int tech_index, EtatCombattant* attaquant) {
    // Si tech_index est -1, c'est une attaque de base (vise toujours un ennemi unique)
    bool vise_allie = false; // Par défaut, on vise un ennemi
    bool cibles_multiples = false; // Par défaut, on vise une seule cible
    
    if (tech_index >= 0 && tech_index < MAX_TECHNIQUES) { // Si c'est une technique valide
        Technique* tech = &attaquant->combattant->techniques[tech_index]; // Récupère la technique
        
        // Déterminer si la technique vise un allié ou un ennemi
        switch (tech->type) { // Selon le type de technique
            case 2: // soin
            case 3: // bouclier
            case 5: // boost (comme dans la technique "Eau énergisante")
                vise_allie = true;
                break;
            case 1: // dégâts
            case 4: // brûlure
            default: // Autre type
                vise_allie = false; // Par défaut, vise un ennemi
                break;
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
        
        // Lecture sécurisée de l'entrée utilisateur
        char buffer[32];
        
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Erreur de lecture. Veuillez réessayer.\n");
            continue;
        }
        
        // Vérifier si l'entrée est un nombre valide
        if (sscanf(buffer, "%d", &choix) != 1) {
            printf("Entrée invalide. Veuillez entrer un nombre.\n");
            continue;
        }
        
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
            printf("Choix invalide. Veuillez choisir un nombre entre 0 et %d.\n", max_index);
        }
    }
    
    // Ajuster l'index si on vise des alliés (car on a ajouté l'option "soi-même")
    if (vise_allie) { // Si on vise un allié
        choix--; // Décrémente le choix
    }
    
    return choix; // Retourne l'index dans la liste des cibles valides
}

// Fonction pour lire un entier de manière sécurisée
int lire_entier_securise() {
    char buffer[32];
    int valeur;
    
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return -1; // Erreur de lecture
    }
    
    // Vérifier si l'entrée est un nombre valide
    if (sscanf(buffer, "%d", &valeur) != 1) {
        return -1; // Entrée non numérique
    }
    
    return valeur;
}

// Fonction pour gérer le tour d'un joueur humain
void gerer_tour_joueur(Combat* combat, EtatCombattant* joueur) {
    bool action_valide = false;
    
    while (!action_valide) {
        // Afficher le menu des actions
        afficher_menu_actions(joueur);
        
        // Lire le choix du joueur
        printf("Votre choix: ");
        int choix = lire_entier_securise();
        
        if (choix == -1) {
            printf("Entrée invalide. Veuillez entrer un nombre.\n");
            continue;
        }
        
        if (choix == 1) {
            // Attaque de base
            int cible_index = choisir_cible(combat, joueur->controleur, -1, joueur);
            if (cible_index >= 0) {
                // Trouver la cible réelle
                int index_reel = 0;
                for (int i = 0; i < combat->nombre_participants; i++) {
                    EtatCombattant* c = &combat->participants[i];
                    bool est_allie = (c->controleur == joueur->controleur);
                    if (!est_ko(c->combattant) && !est_allie && c != joueur) {
                        if (index_reel == cible_index) {
                            attaque_base(joueur, c);
                            action_valide = true;
                            break;
                        }
                        index_reel++;
                    }
                }
            } else if (cible_index == -1) {
                printf("Aucune cible disponible pour cette action.\n");
            }
        } else if (choix >= 2 && choix <= MAX_TECHNIQUES + 1) {
            // Technique spéciale
            int tech_index = choix - 2;
            
            if (tech_index < 0 || tech_index >= MAX_TECHNIQUES) {
                printf("Technique invalide.\n");
                continue;
            }
            
            if (!joueur->combattant->techniques[tech_index].activable) {
                printf("Cette technique n'est pas activable.\n");
                continue;
            }
            
            if (joueur->cooldowns[tech_index] > 0) {
                printf("Cette technique est en recharge pour %d tours.\n", joueur->cooldowns[tech_index]);
                continue;
            }
            
            int cible_index = choisir_cible(combat, joueur->controleur, tech_index, joueur);
            
            if (cible_index == -3) {
                // Se cibler soi-même
                utiliser_technique(joueur, tech_index, joueur);
                action_valide = true;
            } else if (cible_index == -2) {
                // Cibler tous les alliés ou ennemis
                bool vise_allie = (joueur->combattant->techniques[tech_index].type == 2 || 
                                  joueur->combattant->techniques[tech_index].type == 3 || 
                                  joueur->combattant->techniques[tech_index].type == 5);
                
                for (int i = 0; i < combat->nombre_participants; i++) {
                    EtatCombattant* c = &combat->participants[i];
                    bool est_allie = (c->controleur == joueur->controleur);
                    
                    if (!est_ko(c->combattant) && est_allie == vise_allie) {
                        utiliser_technique(joueur, tech_index, c);
                    }
                }
                action_valide = true;
            } else if (cible_index >= 0) {
                // Cibler un combattant spécifique
                int index_reel = 0;
                bool vise_allie = (joueur->combattant->techniques[tech_index].type == 2 || 
                                  joueur->combattant->techniques[tech_index].type == 3 || 
                                  joueur->combattant->techniques[tech_index].type == 5);
                
                for (int i = 0; i < combat->nombre_participants; i++) {
                    EtatCombattant* c = &combat->participants[i];
                    bool est_allie = (c->controleur == joueur->controleur);
                    
                    if (!est_ko(c->combattant) && est_allie == vise_allie && c != joueur) {
                        if (index_reel == cible_index) {
                            utiliser_technique(joueur, tech_index, c);
                            action_valide = true;
                            break;
                        }
                        index_reel++;
                    }
                }
            } else if (cible_index == -1) {
                printf("Aucune cible disponible pour cette technique.\n");
            }
        } else {
            printf("Choix invalide. Veuillez choisir une action entre 1 et %d.\n", MAX_TECHNIQUES + 1);
        }
    }
}