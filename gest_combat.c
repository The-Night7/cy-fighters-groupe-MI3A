#include "gest_combat.h" // Inclusion du fichier d'en-tête pour la gestion du combat
#include <stdlib.h> // Inclusion de la bibliothèque standard
#include <stdio.h> // Inclusion des fonctions d'entrée/sortie
#include <math.h> // Inclusion des fonctions mathématiques  
#include <string.h> // Inclusion des fonctions de manipulation de chaînes
// Fonction interne pour calculer les dégâts infligés par une attaque ou une technique
float calculer_degats(Combattant* attaquant, Technique* tech, Combattant* cible) { // Déclaration de la fonction avec ses paramètres
    /* en gros je fais dégats = (attaque attaquant x puissance technique) - (défense x 0.2) */ // Commentaire explicatif de la formule
    
    // Si tech est NULL (attaque de base), on utilise une puissance par défaut
    float puissance = 0.5; // Initialisation de la puissance par défaut
    
    if (tech) { // Si une technique est spécifiée
        if (tech->puissance <= 0) return 0; // Vérification de la validité de la puissance
        puissance = tech->puissance; // Attribution de la puissance de la technique
    }
    
    float base_damage = attaquant->attaque * puissance; // Calcul des dégâts de base
    float reduction = cible->defense / 100.0 * 20;  // Calcul de la réduction de dégâts
    float damage = base_damage - reduction; // Calcul des dégâts finaux
    
    // Variation aléatoire des dégâts (+/- 10%)
    float variation = (float)((rand() % 20) - 10) / 100.0; // Calcul de la variation aléatoire
    damage = damage * (1 + variation); // Application de la variation aux dégâts
    
    return damage > 0 ? damage : 0; // Retour des dégâts (minimum 0)
}

// Fonction pour gérer l'IA
void gerer_tour_ia(Combat* combat, EtatCombattant* ia, NiveauDifficulte difficulte) { // Déclaration de la fonction
    printf("\nC'est au tour de %s (IA) d'agir!\n", ia->combattant->nom); // Affichage du tour de l'IA
    
    // Variables pour le choix de l'action
    EtatCombattant* meilleure_cible = NULL; // Initialisation de la meilleure cible
    int meilleure_technique = -1; // Initialisation de la meilleure technique
    float meilleur_score = -1; // Initialisation du meilleur score
    
    switch (difficulte) { // Début du switch sur la difficulté
        case DIFFICULTE_FACILE: // Cas de la difficulté facile
            // En facile : choix aléatoire de la cible et attaque de base uniquement
            {
                int nb_cibles = 0; // Compteur de cibles possibles
                for (int i = 0; i < combat->nombre_participants; i++) { // Parcours des participants
                    EtatCombattant* c = &combat->participants[i]; // Récupération du participant courant
                    if (!est_ko(c->combattant) && c->controleur != ia->controleur) { // Vérification de la validité de la cible
                        nb_cibles++; // Incrémentation du compteur
                    }
                }
                
                if (nb_cibles > 0) { // S'il y a des cibles disponibles
                    int cible_aleatoire = rand() % nb_cibles; // Choix aléatoire d'une cible
                    int index_cible = 0; // Index de la cible courante
                    
                    for (int i = 0; i < combat->nombre_participants; i++) { // Parcours des participants
                        EtatCombattant* c = &combat->participants[i]; // Récupération du participant courant
                        if (!est_ko(c->combattant) && c->controleur != ia->controleur) { // Vérification de la validité de la cible
                            if (index_cible == cible_aleatoire) { // Si c'est la cible choisie
                                attaque_base(ia, c); // Effectue l'attaque
                                break; // Sort de la boucle
                            }
                            index_cible++; // Incrémentation de l'index
                        }
                    }
                }
            }
            break; // Fin du cas facile
            
        case DIFFICULTE_MOYENNE: // Cas de la difficulté moyenne
            {
                bool va_utiliser_technique = (rand() % 100) < 70; // Décision d'utiliser une technique (70% de chances)
                
                if (va_utiliser_technique) { // Si on utilise une technique
                    for (int i = 0; i < MAX_TECHNIQUES; i++) { // Parcours des techniques
                        if (ia->cooldowns[i] == 0 && ia->combattant->techniques[i].activable) { // Vérification de la disponibilité
                            for (int j = 0; j < combat->nombre_participants; j++) { // Parcours des cibles potentielles
                                EtatCombattant* cible = &combat->participants[j]; // Récupération de la cible courante
                                if (!est_ko(cible->combattant)) { // Si la cible n'est pas KO
                                    bool est_allie = (cible->controleur == ia->controleur); // Vérifie si c'est un allié
                                    bool technique_soin = (ia->combattant->techniques[i].puissance < 0); // Vérifie si c'est un soin
                                    
                                    if (technique_soin && est_allie && // Si c'est un soin pour un allié
                                        cible->combattant->Vie.courrante < cible->combattant->Vie.max * 0.5) { // Et qu'il est blessé
                                        meilleure_cible = cible; // Mémorisation de la cible
                                        meilleure_technique = i; // Mémorisation de la technique
                                        break; // Sort de la boucle
                                    }
                                    else if (!technique_soin && !est_allie) { // Si c'est une attaque sur un ennemi
                                        meilleure_cible = cible; // Mémorisation de la cible
                                        meilleure_technique = i; // Mémorisation de la technique
                                        break; // Sort de la boucle
                                    }
                                }
                            }
                        }
                    }
                }
                
                if (meilleure_technique >= 0 && meilleure_cible != NULL) { // Si une technique et une cible ont été trouvées
                    utiliser_technique(ia, meilleure_technique, meilleure_cible); // Utilisation de la technique
                } else { // Sinon
                    // Attaque de base sur un ennemi aléatoire
                    for (int i = 0; i < combat->nombre_participants; i++) { // Parcours des participants
                        EtatCombattant* cible = &combat->participants[i]; // Récupération du participant courant
                        if (!est_ko(cible->combattant) && cible->controleur != ia->controleur) { // Si c'est une cible valide
                            attaque_base(ia, cible); // Effectue l'attaque
                            break; // Sort de la boucle
                        }
                    }
                }
            }
            break; // Fin du cas moyen
            
        case DIFFICULTE_DIFFICILE: // Cas de la difficulté difficile
            // En difficile : choix optimal des techniques et des cibles
            {
                for (int i = -1; i < MAX_TECHNIQUES; i++) { // Parcours des techniques (-1 pour attaque de base)
                    if (i >= 0 && (ia->cooldowns[i] > 0 || !ia->combattant->techniques[i].activable)) { // Vérifie disponibilité
                        continue; // Passe à la suivante si non disponible
                    }
                    
                    for (int j = 0; j < combat->nombre_participants; j++) { // Parcours des cibles potentielles
                        EtatCombattant* cible = &combat->participants[j]; // Récupération de la cible courante
                        if (est_ko(cible->combattant)) continue; // Passe si KO
                        
                        bool est_allie = (cible->controleur == ia->controleur); // Vérifie si c'est un allié
                        float score = 0; // Initialisation du score
                        
                        if (i == -1) { // Si c'est une attaque de base
                            if (!est_allie) { // Si c'est un ennemi
                                score = calculer_degats(ia->combattant, NULL, cible->combattant); // Calcul du score
                            }
                        } else { // Si c'est une technique
                            Technique* tech = &ia->combattant->techniques[i]; // Récupération de la technique
                            if (tech->puissance < 0) { // Si c'est un soin
                                if (est_allie) { // Si c'est un allié
                                    score = (cible->combattant->Vie.max - cible->combattant->Vie.courrante) // Calcul du score
                                           * (-tech->puissance); // Multiplié par la puissance de soin
                                }
                            } else { // Si c'est une attaque
                                if (!est_allie) { // Si c'est un ennemi
                                    score = calculer_degats(ia->combattant, tech, cible->combattant); // Calcul du score
                                    if (tech->Effet.possede) { // Si la technique a un effet
                                        score *= 1.5; // Bonus au score
                                    }
                                }
                            }
                        }
                        
                        if (score > meilleur_score) { // Si le score est meilleur
                            meilleur_score = score; // Mise à jour du meilleur score
                            meilleure_technique = i; // Mise à jour de la meilleure technique
                            meilleure_cible = cible; // Mise à jour de la meilleure cible
                        }
                    }
                }
                
                if (meilleure_technique >= 0 && meilleure_cible != NULL) { // Si une technique et une cible ont été trouvées
                    utiliser_technique(ia, meilleure_technique, meilleure_cible); // Utilisation de la technique
                } else { // Sinon
                    attaque_base(ia, meilleure_cible); // Attaque de base
                }
            }
            break; // Fin du cas difficile
            
        case DIFFICULTE_MAX: // Cas de la difficulté maximale
            printf("Niveau de difficulté non valide\n"); // Message d'erreur
            break; // Fin du cas max
    }
}

void configurer_combat(Combat* combat, bool mode_jvj, NiveauDifficulte* difficulte) { // Déclaration de la fonction
    Equipe* equipe1 = malloc(sizeof(Equipe)); // Allocation de l'équipe 1
    Equipe* equipe2 = malloc(sizeof(Equipe)); // Allocation de l'équipe 2
    equipe1->member_count = 0; // Initialisation du compteur de membres équipe 1
    equipe2->member_count = 0; // Initialisation du compteur de membres équipe 2

    if (!mode_jvj) { // Si mode joueur contre ordinateur
        printf("\nChoisissez le niveau de difficulté :\n"); // Affichage du menu de difficulté
        printf("0 - Facile\n"); // Option facile
        printf("1 - Moyen\n"); // Option moyenne
        printf("2 - Difficile\n"); // Option difficile
        
        do { // Boucle de saisie
            printf("Votre choix : "); // Demande de choix
            *difficulte = lire_entier_securise(); // Lecture du choix
            if (*difficulte < 0 || *difficulte >= DIFFICULTE_MAX) { // Vérification de la validité
                printf("Niveau de difficulté invalide. Veuillez réessayer.\n"); // Message d'erreur
            }
        } while (*difficulte < 0 || *difficulte >= DIFFICULTE_MAX); // Continue tant que le choix est invalide
    }

    printf("\nEntrez le nom de l'équipe 1 : "); // Demande du nom de l'équipe 1
    char buffer[50]; // Buffer pour la lecture
    fgets(buffer, sizeof(buffer), stdin); // Lecture du nom
    buffer[strcspn(buffer, "\n")] = 0; // Suppression du retour à la ligne
    strncpy(equipe1->name, buffer, sizeof(equipe1->name) - 1); // Copie du nom

    if (mode_jvj) { // Si mode joueur contre joueur
        printf("Entrez le nom de l'équipe 2 : "); // Demande du nom de l'équipe 2
        fgets(buffer, sizeof(buffer), stdin); // Lecture du nom
        buffer[strcspn(buffer, "\n")] = 0; // Suppression du retour à la ligne
        strncpy(equipe2->name, buffer, sizeof(equipe2->name) - 1); // Copie du nom
    } else { // Si mode joueur contre ordinateur
        strncpy(equipe2->name, "Équipe IA", sizeof(equipe2->name) - 1); // Nom par défaut pour l'IA
    }

    printf("\nSélection des personnages pour %s (3 personnages) :\n", equipe1->name); // Début de la sélection
    printf("Personnages disponibles :\n"); // Liste des personnages
    printf("1 - Musu\n2 - Freettle\n3 - Ronflex\n4 - Kirishima\n5 - Marco\n6 - Furina\n"); // Options

    const char* noms_persos[] = {"Musu", "Freettle", "Ronflex", "Kirishima", "Marco", "Furina"}; // Tableau des noms
    bool persos_pris[6] = {false}; // Tableau de disponibilité

    for (int i = 0; i < 3; i++) { // Pour chaque personnage à sélectionner
        int choix; // Variable de choix
        do { // Boucle de sélection
            printf("Choisissez le personnage %d : ", i + 1); // Demande de choix
            choix = lire_entier_securise(); // Lecture du choix
            if (choix < 1 || choix > 6) { // Vérification de la validité
                printf("Choix invalide. Veuillez choisir entre 1 et 6.\n"); // Message d'erreur
                continue; // Recommence
            }
            if (persos_pris[choix - 1]) { // Si déjà pris
                printf("Ce personnage est déjà pris. Veuillez en choisir un autre.\n"); // Message d'erreur
                continue; // Recommence
            }
            break; // Sort de la boucle si valide
        } while (1); // Continue jusqu'à un choix valide

        persos_pris[choix - 1] = true; // Marque le personnage comme pris
        equipe1->members[i] = *creer_combattant(noms_persos[choix - 1]); // Crée le combattant
        equipe1->member_count++; // Incrémente le compteur
    }

    if (mode_jvj) { // Si mode joueur contre joueur
        printf("\nSélection des personnages pour %s (3 personnages) :\n", equipe2->name); // Sélection équipe 2
        for (int i = 0; i < 3; i++) { // Pour chaque personnage
            printf("Personnages disponibles :\n"); // Liste des disponibles
            for (int j = 0; j < 6; j++) { // Parcours des personnages
                if (!persos_pris[j]) { // Si disponible
                    printf("%d - %s\n", j + 1, noms_persos[j]); // Affiche l'option
                }
            }

            int choix; // Variable de choix
            do { // Boucle de sélection
                printf("Choisissez le personnage %d : ", i + 1); // Demande de choix
                choix = lire_entier_securise(); // Lecture du choix
                if (choix < 1 || choix > 6) { // Vérification de la validité
                    printf("Choix invalide. Veuillez choisir entre 1 et 6.\n"); // Message d'erreur
                    continue; // Recommence
                }
                if (persos_pris[choix - 1]) { // Si déjà pris
                    printf("Ce personnage est déjà pris. Veuillez en choisir un autre.\n"); // Message d'erreur
                    continue; // Recommence
                }
                break; // Sort si valide
            } while (1); // Continue jusqu'à un choix valide
            persos_pris[choix - 1] = true; // Marque comme pris
            equipe2->members[i] = *creer_combattant(noms_persos[choix - 1]); // Crée le combattant
            equipe2->member_count++; // Incrémente le compteur
        }
    } else { // Si mode joueur contre ordinateur
        int count = 0; // Compteur
        for (int i = 0; count < 3; i = (i + 1) % 6) { // Pour chaque personnage restant
            if (!persos_pris[i]) { // Si disponible
                equipe2->members[count] = *creer_combattant(noms_persos[i]); // Crée le combattant
                equipe2->member_count++; // Incrémente le compteur
                count++; // Incrémente le compteur général
            }
        }
    }

    initialiser_combat_mode(combat, equipe1, equipe2, mode_jvj); // Initialise le combat
}

void initialiser_combat_mode(Combat* combat, Equipe* eq1, Equipe* eq2, bool mode_jvj) { // Déclaration de la fonction
    combat->equipe1 = eq1; // Assignation de l'équipe 1
    combat->equipe2 = eq2; // Assignation de l'équipe 2
    combat->tour = 0; // Initialisation du compteur de tours
    
    int total = eq1->member_count + eq2->member_count; // Calcul du nombre total de participants
    combat->participants = malloc(total * sizeof(EtatCombattant)); // Allocation de la mémoire

    for (int i = 0; i < total; i++) { // Pour chaque participant
        Combattant* c = (i < eq1->member_count) ? &eq1->members[i] : &eq2->members[i - eq1->member_count]; // Récupère le combattant
        
        TypeJoueur controleur; // Type de contrôleur
        if (mode_jvj) { // Si mode joueur contre joueur
            controleur = JOUEUR; // Tous sont des joueurs
        } else { // Si mode joueur contre ordinateur
            controleur = (i < eq1->member_count) ? JOUEUR : ORDI; // Joueur pour équipe 1, IA pour équipe 2
        }
        
        combat->participants[i] = (EtatCombattant){ // Initialisation du participant
            .combattant = c, // Assigne le combattant
            .turn_meter = 0.0f, // Initialise la jauge de tour
            .controleur = controleur, // Assigne le contrôleur
            .nb_effets = 0 // Initialise le nombre d'effets
        };
        
        for (int j = 0; j < MAX_TECHNIQUES; j++) { // Pour chaque technique
            combat->participants[i].cooldowns[j] = 0; // Initialise le cooldown
        }
    }
    combat->nombre_participants = total; // Assigne le nombre total de participants
}

void gerer_tour_combat(Combat* combat) { // Déclaration de la fonction
    combat->tour++; // Incrémente le compteur de tours
    printf("\n=== DÉBUT DU TOUR %d ===\n", combat->tour); // Affiche le début du tour
    
    appliquer_effets(combat); // Applique les effets actifs
    
    if (verifier_victoire(combat)) { // Vérifie s'il y a un vainqueur
        return; // Sort si oui
    }
    
    for (int i = 0; i < combat->nombre_participants; i++) { // Pour chaque participant
        EtatCombattant* cs = &combat->participants[i]; // Récupère l'état du combattant
        if (!est_ko(cs->combattant)) { // Si pas KO
            cs->turn_meter += cs->combattant->vitesse; // Augmente la jauge de tour
        }
    }
    
    bool action_effectuee = false; // Flag d'action effectuée
    EtatCombattant* dernier_joueur_humain = NULL; // Dernier joueur humain
    do { // Boucle principale du tour
        action_effectuee = false; // Réinitialise le flag
        
        int index_max = -1; // Index du prochain à jouer
        float tm_max = 0; // Jauge maximale
        
        for (int i = 0; i < combat->nombre_participants; i++) { // Pour chaque participant
            EtatCombattant* cs = &combat->participants[i]; // Récupère l'état
            if (!est_ko(cs->combattant) && cs->turn_meter >= 100 && cs->turn_meter > tm_max) { // Si peut jouer
                index_max = i; // Mémorise l'index
                tm_max = cs->turn_meter; // Mémorise la jauge
            }
        }
        
        if (index_max != -1) { // Si quelqu'un peut jouer
            EtatCombattant* acteur = &combat->participants[index_max]; // Récupère l'acteur
            acteur->turn_meter -= 100; // Réduit sa jauge
            
            bool est_etourdi = false; // Flag d'étourdissement
            for (int j = 0; j < acteur->nb_effets; j++) { // Vérifie les effets
                if (acteur->effets[j].type == EFFET_ETOURDISSEMENT) { // Si étourdi
                    est_etourdi = true; // Active le flag
                    printf("%s est étourdi et passe son tour!\n", acteur->combattant->nom); // Message
                    break; // Sort de la boucle
                }
            }
            
            if (!est_etourdi) { // Si pas étourdi
                if (acteur->controleur == JOUEUR) { // Si c'est un joueur
                    if (dernier_joueur_humain != NULL) { // Si ce n'est pas le premier
                        bool meme_equipe = false; // Flag même équipe
                        
                        for (int i = 0; i < combat->equipe1->member_count; i++) { // Vérifie l'équipe
                            if (acteur->combattant == &combat->equipe1->members[i]) { // Si dans équipe 1
                                for (int j = 0; j < combat->equipe1->member_count; j++) { // Vérifie le dernier
                                    if (dernier_joueur_humain->combattant == &combat->equipe1->members[j]) { // Si même équipe
                                        meme_equipe = true; // Active le flag
                                        break; // Sort de la boucle
                                    }
                                }
                                break; // Sort de la boucle
                            }
                        }
                        
                        if (!meme_equipe) { // Si équipes différentes
                            transition_joueurs(combat, acteur); // Fait la transition
                        }
                    }

                    afficher_statuts_combat(combat); // Affiche les statuts
                    
                    gerer_tour_joueur(combat, acteur); // Gère le tour du joueur
                    dernier_joueur_humain = acteur; // Mémorise le dernier joueur
                } else { // Si c'est l'IA
                    NiveauDifficulte difficulte = DIFFICULTE_MOYENNE; // Définit la difficulté
                    gerer_tour_ia(combat, acteur, difficulte); // Gère le tour de l'IA
                }
            }
            
            action_effectuee = true; // Marque l'action comme effectuée
            
            if (verifier_victoire(combat)) { // Vérifie la victoire
                return; // Sort si oui
            }
        }
    } while (action_effectuee); // Continue tant qu'il y a des actions
    
    for (int i = 0; i < combat->nombre_participants; i++) { // Pour chaque participant
        EtatCombattant* cs = &combat->participants[i]; // Récupère l'état
        if (!est_ko(cs->combattant)) { // Si pas KO
            for (int j = 0; j < MAX_TECHNIQUES; j++) { // Pour chaque technique
                if (cs->cooldowns[j] > 0) { // Si en cooldown
                    cs->cooldowns[j]--; // Réduit le cooldown
                }
            }
        }
    }
    
    printf("\n=== FIN DU TOUR %d ===\n", combat->tour); // Affiche la fin du tour
}

void attaque_base(EtatCombattant* attaquant, EtatCombattant* cible) { // Déclaration de la fonction
    float degats = calculer_degats(attaquant->combattant, NULL, cible->combattant); // Calcule les dégâts
    cible->combattant->Vie.courrante -= degats; // Applique les dégâts
    printf("%s attaque %s et inflige %.1f dégâts!\n", // Affiche le message
           attaquant->combattant->nom, // Nom de l'attaquant
           cible->combattant->nom, // Nom de la cible
           degats); // Dégâts infligés
}

int choisir_cible(Combat* combat, TypeJoueur controleur, int tech_index, EtatCombattant* attaquant) {
    if (!combat || !attaquant) return -1;  // Validation des paramètres

    bool cibles_valides_existent = false;
    bool est_allie = false;
    
    // Déterminer si l'attaquant est dans l'équipe 1
    bool attaquant_equipe1 = false;
    for (int i = 0; i < combat->equipe1->member_count; i++) {
        if (attaquant->combattant == &combat->equipe1->members[i]) {
            attaquant_equipe1 = true;
            break;
        }
    }

    // Si c'est une technique spéciale, vérifier la cible appropriée
    if (tech_index >= 0) {
        Technique* tech = &attaquant->combattant->techniques[tech_index];
        est_allie = (strstr(tech->cible, "allié") != NULL);
    }

    // Afficher les cibles possibles
    printf("\nCibles disponibles:\n");
    int num_cible = 1;
    
    // Parcourir les deux équipes
    for (int i = 0; i < combat->nombre_participants; i++) {
        EtatCombattant* cible_potentielle = &combat->participants[i];
        
        // Vérifier si la cible est valide
        bool est_dans_equipe1 = false;
        for (int j = 0; j < combat->equipe1->member_count; j++) {
            if (cible_potentielle->combattant == &combat->equipe1->members[j]) {
                est_dans_equipe1 = true;
                break;
            }
        }

        // Déterminer si c'est une cible valide selon le type d'action
        bool cible_valide = false;
        if (tech_index >= 0) {
            // Pour une technique spéciale
            if (est_allie) {
                cible_valide = (est_dans_equipe1 == attaquant_equipe1);
            } else {
                cible_valide = (est_dans_equipe1 != attaquant_equipe1);
            }
        } else {
            // Pour une attaque de base, cibler uniquement les ennemis
            cible_valide = (est_dans_equipe1 != attaquant_equipe1);
        }

        // Afficher la cible si elle est valide et pas KO
        if (cible_valide && !est_ko(cible_potentielle->combattant)) {
            printf("%d. %s (PV: %.0f/%.0f)\n", 
                   num_cible,
                   cible_potentielle->combattant->nom,
                   cible_potentielle->combattant->Vie.courrante,
                   cible_potentielle->combattant->Vie.max);
            cibles_valides_existent = true;
            num_cible++;
        }
    }

    // Si aucune cible valide n'existe
    if (!cibles_valides_existent) {
        printf("Aucune cible valide disponible!\n");
        return -1;
    }

    // Demander au joueur de choisir une cible
    printf("Choisissez une cible (1-%d): ", num_cible - 1);
    int choix = lire_entier_securise();
    
    // Valider le choix
    if (choix < 1 || choix >= num_cible) {
        printf("Choix invalide!\n");
        return -1;
    }

    return choix - 1;  // Retourner l'index de la cible choisie
}

int lire_entier_securise() {
    char buffer[32];
    if (fgets(buffer, sizeof(buffer), stdin)) {
        return atoi(buffer);
    }
    return -1;
}

void gerer_tour_joueur(Combat* combat, EtatCombattant* joueur) {
    afficher_menu_actions(joueur);
    printf("Votre choix : ");
    int choix = lire_entier_securise();
    
    if (choix == 1) {  // Attaque de base
        int index_cible = choisir_cible(combat, joueur->controleur, -1, joueur);
        if (index_cible >= 0) {
            attaque_base(joueur, &combat->participants[index_cible]);
        }
    } else if (choix >= 2 && choix < MAX_TECHNIQUES + 2) {
        int tech_index = choix - 2;
        if (joueur->cooldowns[tech_index] == 0 && 
            joueur->combattant->techniques[tech_index].activable) {
            int index_cible = choisir_cible(combat, joueur->controleur, tech_index, joueur);
            if (index_cible >= 0) {
                utiliser_technique(joueur, tech_index, &combat->participants[index_cible]);
            }
        } else {
            printf("Cette technique n'est pas disponible !\n");
        }
    } else {
        printf("Action invalide !\n");
    }
}

void transition_joueurs(Combat* combat, EtatCombattant* joueur_suivant) {
    printf("\nAppuyez sur Entrée pour passer au joueur suivant...");
    while (getchar() != '\n');  // Vide le buffer
    printf("\033[2J\033[H");    // Efface l'écran
}
