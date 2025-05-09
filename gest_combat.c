#include "gest_combat.h"
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
    EtatCombattant* dernier_joueur_humain = NULL; // Pour suivre le dernier joueur humain
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
                    // Si c'est un joueur humain différent du précédent, faire la transition
                    if (dernier_joueur_humain != NULL) {
                        bool meme_equipe = false;
                        
                        // Vérifier si les deux joueurs sont dans la même équipe
                        for (int i = 0; i < combat->equipe1->member_count; i++) {
                            if (acteur->combattant == &combat->equipe1->members[i]) {
                                for (int j = 0; j < combat->equipe1->member_count; j++) {
                                    if (dernier_joueur_humain->combattant == &combat->equipe1->members[j]) {
                                        meme_equipe = true;
                                        break;
                                    }
                                }
                                break;
                            }
                        }
                        
                        // Si les joueurs sont de différentes équipes, faire la transition
                        if (!meme_equipe) {
                            transition_joueurs(combat, acteur);
                        }
                    }

                    // Tour du joueur humain
                    afficher_statuts_combat(combat);
                    
                    gerer_tour_joueur(combat, acteur);
                    dernier_joueur_humain = acteur;
                } else {
                    // Tour de l'IA
                    printf("\nC'est au tour de %s (IA) d'agir!\n", acteur->combattant->nom);
                    
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

// Permet de choisir une cible en fonction du type de technique et de ses cibles
int choisir_cible(Combat* combat, TypeJoueur controleur, int tech_index, EtatCombattant* attaquant) {
    // Si tech_index est -1, c'est une attaque de base (vise toujours un ennemi unique)
    bool vise_allie = false; // Par défaut, on vise un ennemi
    bool cibles_multiples = false; // Par défaut, on vise une seule cible
    bool soi_meme = false; // Par défaut, on ne se cible pas soi-même
    
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
        soi_meme = (tech->ncible == 3); // Si ncible = 3, se vise soi-même
    }
    
    // Si la technique se cible soi-même, retourner directement le code spécial
    if (soi_meme) {
        printf("\nLa technique cible automatiquement le lanceur!\n");
        return -3; // Code spécial pour indiquer "soi-même"
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
    
    // Vérifier si nous sommes en mode JvJ (tous les participants sont contrôlés par des joueurs)
    bool mode_jvj = true;
    for (int i = 0; i < combat->nombre_participants; i++) {
        if (combat->participants[i].controleur == ORDI) {
            mode_jvj = false;
            break;
        }
    }
    // Déterminer si le combattant est un allié ou un ennemi
    bool est_allie;
    for (int i = 0; i < combat->nombre_participants; i++) {
        EtatCombattant* c = &combat->participants[i];
        
        
        if (mode_jvj) {
            // En mode JvJ, on vérifie si les combattants sont dans la même équipe
            bool attaquant_equipe1 = false;
            bool cible_equipe1 = false;
            
            // Vérifier si l'attaquant est dans l'équipe 1
            for (int j = 0; j < combat->equipe1->member_count; j++) {
                if (attaquant->combattant == &combat->equipe1->members[j]) {
                    attaquant_equipe1 = true;
                    break;
                }
            }
            
            // Vérifier si la cible est dans l'équipe 1
            for (int j = 0; j < combat->equipe1->member_count; j++) {
                if (c->combattant == &combat->equipe1->members[j]) {
                    cible_equipe1 = true;
                    break;
                }
            }
            
            // Les combattants sont des alliés s'ils sont dans la même équipe
            est_allie = (attaquant_equipe1 == cible_equipe1);
        } else {
            // En mode JvO, on utilise le contrôleur comme avant
            est_allie = (c->controleur == controleur);
        }
        
        if (!est_ko(c->combattant) && (est_allie == vise_allie) && c != attaquant) {
            cibles_valides_existent = true;
            break;
        }
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
            bool est_allie;
            
            if (mode_jvj) {
                bool attaquant_equipe1 = false;
                bool cible_equipe1 = false;
                
                for (int j = 0; j < combat->equipe1->member_count; j++) {
                    if (attaquant->combattant == &combat->equipe1->members[j]) {
                        attaquant_equipe1 = true;
                        break;
                    }
                }
                
                for (int j = 0; j < combat->equipe1->member_count; j++) {
                    if (c->combattant == &combat->equipe1->members[j]) {
                        cible_equipe1 = true;
                        break;
                    }
                }
                
                est_allie = (attaquant_equipe1 == cible_equipe1);
            } else {
                est_allie = (c->controleur == controleur);
            }
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
    bool vise_allie;
    // Déterminer si le joueur est de l'équipe 1 ou 2
    bool est_equipe1 = false;
    for (int i = 0; i < combat->equipe1->member_count; i++) {
        if (joueur->combattant == &combat->equipe1->members[i]) {
            est_equipe1 = true;
            break;
        }
    }
    
    // Afficher clairement quel joueur doit jouer
    printf("\n=== TOUR DU %s ===\n", est_equipe1 ? "JOUEUR 1" : "JOUEUR 2");
    printf("C'est à %s de jouer!\n", joueur->combattant->nom);
    
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
                    
                    // Déterminer si c'est un allié en utilisant la même logique que dans choisir_cible
                    bool est_allie;
                    bool mode_jvj = true;
                    
                    for (int j = 0; j < combat->nombre_participants; j++) {
                        if (combat->participants[j].controleur == ORDI) {
                            mode_jvj = false;
                            break;
                        }
                    }
                    
                    if (mode_jvj) {
                        bool joueur_equipe1 = false;
                        bool cible_equipe1 = false;
                        
                        // Vérifier si le joueur est dans l'équipe 1
                        for (int j = 0; j < combat->equipe1->member_count; j++) {
                            if (joueur->combattant == &combat->equipe1->members[j]) {
                                joueur_equipe1 = true;
                                break;
                            }
                        }
                        
                        // Vérifier si la cible est dans l'équipe 1
                        for (int j = 0; j < combat->equipe1->member_count; j++) {
                            if (c->combattant == &combat->equipe1->members[j]) {
                                cible_equipe1 = true;
                                break;
                            }
                        }
                        
                        est_allie = (joueur_equipe1 == cible_equipe1);
                    } else {
                        est_allie = (c->controleur == joueur->controleur);
                    }
                    
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
                vise_allie = (joueur->combattant->techniques[tech_index].type == 0 ||  // boost
                    joueur->combattant->techniques[tech_index].type == 2 ||   // soin
                    joueur->combattant->techniques[tech_index].type == 3 ||   // bouclier
                    joueur->combattant->techniques[tech_index].type == 5);    // autre type de boost
                
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
                vise_allie = (joueur->combattant->techniques[tech_index].type == 0 ||  // boost
                    joueur->combattant->techniques[tech_index].type == 2 ||   // soin
                    joueur->combattant->techniques[tech_index].type == 3 ||   // bouclier
                    joueur->combattant->techniques[tech_index].type == 5);    // autre type de boost
                
                for (int i = 0; i < combat->nombre_participants; i++) {
                    EtatCombattant* c = &combat->participants[i];
                    bool est_allie;
                    bool mode_jvj = true;
                    
                    for (int j = 0; j < combat->nombre_participants; j++) {
                        if (combat->participants[j].controleur == ORDI) {
                            mode_jvj = false;
                            break;
                        }
                    }
                    
                    if (mode_jvj) {
                        bool joueur_equipe1 = false;
                        bool cible_equipe1 = false;
                        
                        for (int j = 0; j < combat->equipe1->member_count; j++) {
                            if (joueur->combattant == &combat->equipe1->members[j]) {
                                joueur_equipe1 = true;
                                break;
                            }
                        }
                        
                        for (int j = 0; j < combat->equipe1->member_count; j++) {
                            if (c->combattant == &combat->equipe1->members[j]) {
                                cible_equipe1 = true;
                                break;
                            }
                        }
                        
                        est_allie = (joueur_equipe1 == cible_equipe1);
                    } else {
                        est_allie = (c->controleur == joueur->controleur);
                    }
                    
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

// Fonction pour marquer la transition entre deux joueurs
void transition_joueurs(Combat* combat, EtatCombattant* joueur_suivant) {
    bool est_equipe1 = false;
    for (int i = 0; i < combat->equipe1->member_count; i++) {
        if (joueur_suivant->combattant == &combat->equipe1->members[i]) {
            est_equipe1 = true;
            break;
        }
    }
    
    printf("\n----------------------------------------------\n");
    printf("Le contrôle passe au %s (%s)\n", 
           est_equipe1 ? "JOUEUR 1" : "JOUEUR 2",
           joueur_suivant->combattant->nom);
    printf("Appuyez sur Entrée pour continuer...");
    
    // Attendre que le joueur appuie sur Entrée
    char buffer[10];
    fgets(buffer, sizeof(buffer), stdin);
}