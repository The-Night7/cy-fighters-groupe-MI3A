#include "gest_effets.h" // Inclusion du header de gestion des effets
#include "util_combat.h" // Inclusion du header des utilitaires de combat
#include <stdlib.h> // Inclusion de la bibliothèque standard
#include <stdio.h> // Inclusion des fonctions d'entrée/sortie
#include <math.h> // Inclusion des fonctions mathématiques
#include <string.h> // Inclusion des fonctions de manipulation de chaînes
// Fonction pour convertir une chaîne d'effet en TypeEffet
TypeEffet convertir_nom_effet(const char* nom_effet) {
    if (!nom_effet) return EFFET_AUCUN; // Si la chaîne est NULL, retourne EFFET_AUCUN
    
    if (strcmp(nom_effet, "EFFET_POISON") == 0) return EFFET_POISON; // Compare avec "EFFET_POISON"
    if (strcmp(nom_effet, "EFFET_ETOURDISSEMENT") == 0) return EFFET_ETOURDISSEMENT; // Compare avec "EFFET_ETOURDISSEMENT"
    if (strcmp(nom_effet, "EFFET_BOOST_ATTAQUE") == 0) return EFFET_BOOST_ATTAQUE; // Compare avec "EFFET_BOOST_ATTAQUE"
    if (strcmp(nom_effet, "EFFET_BOOST_DEFENSE") == 0) return EFFET_BOOST_DEFENSE; // Compare avec "EFFET_BOOST_DEFENSE"
    if (strcmp(nom_effet, "EFFET_BOOST_VITESSE") == 0) return EFFET_BOOST_VITESSE; // Compare avec "EFFET_BOOST_VITESSE" 
    if (strcmp(nom_effet, "EFFET_BRULURE") == 0) return EFFET_BRULURE; // Compare avec "EFFET_BRULURE"
    if (strcmp(nom_effet, "EFFET_RECONSTITUTION") == 0) return EFFET_RECONSTITUTION; // Compare avec "EFFET_RECONSTITUTION"
    if (strcmp(nom_effet, "EFFET_BOUCLIER") == 0) return EFFET_BOUCLIER; // Compare avec "EFFET_BOUCLIER"
    if (strcmp(nom_effet, "EFFET_PROVOCATION") == 0) return EFFET_PROVOCATION; // Compare avec "EFFET_PROVOCATION"
    if (strcmp(nom_effet, "EFFET_VOL_DE_VIE") == 0) return EFFET_VOL_DE_VIE; // Compare avec "EFFET_VOL_DE_VIE"
    
    return EFFET_AUCUN; // Si aucune correspondance, retourne EFFET_AUCUN
}

// Fonction pour obtenir le nom d'un effet
const char* obtenir_nom_effet(TypeEffet effet) {
    switch(effet) { // Switch sur le type d'effet
        case EFFET_RECONSTITUTION: return "Reconstitution"; // Retourne le nom pour EFFET_RECONSTITUTION
        case EFFET_POISON: return "Poison"; // Retourne le nom pour EFFET_POISON 
        case EFFET_ETOURDISSEMENT: return "Étourdissement"; // Retourne le nom pour EFFET_ETOURDISSEMENT
        case EFFET_BOOST_ATTAQUE: return "Boost d'attaque"; // Retourne le nom pour EFFET_BOOST_ATTAQUE
        case EFFET_BOOST_DEFENSE: return "Boost de défense"; // Retourne le nom pour EFFET_BOOST_DEFENSE
        case EFFET_BOOST_VITESSE: return "Boost de vitesse"; // Retourne le nom pour EFFET_BOOST_VITESSE
        case EFFET_BRULURE: return "Brûlure"; // Retourne le nom pour EFFET_BRULURE
        case EFFET_BOUCLIER: return "Bouclier"; // Retourne le nom pour EFFET_BOUCLIER
        case EFFET_PROVOCATION: return "Provocation"; // Retourne le nom pour EFFET_PROVOCATION
        case EFFET_VOL_DE_VIE: return "Vol de vie"; // Retourne le nom pour EFFET_VOL_DE_VIE
        default: return "Aucun"; // Retourne "Aucun" par défaut
    }
}

// Fonction pour appliquer un effet à un combattant
void appliquer_effet(EtatCombattant* cible, TypeEffet effet, int duree, float puissance) {
    for (int i = 0; i < cible->nb_effets; i++) { // Parcours des effets existants
        if (cible->effets[i].type == effet) { // Si l'effet existe déjà
            cible->effets[i].tours_restants = duree; // Met à jour la durée
            cible->effets[i].puissance = puissance; // Met à jour la puissance
            return; // Sort de la fonction
        }
    }
    
    if (cible->nb_effets < MAX_EFFECTS) { // Vérifie s'il reste de la place
        cible->effets[cible->nb_effets] = (EffetTemporaire){ // Crée un nouvel effet
            .type = effet, // Définit le type
            .tours_restants = duree, // Définit la durée
            .puissance = puissance // Définit la puissance
        };
        cible->nb_effets++; // Incrémente le nombre d'effets
        switch (effet) { // Switch sur le type d'effet
            case EFFET_BOOST_ATTAQUE: // Cas du boost d'attaque
                cible->combattant->attaque *= (1 + puissance); // Augmente l'attaque
                break;
            case EFFET_BOOST_DEFENSE: // Cas du boost de défense
                cible->combattant->defense *= (1 + puissance); // Augmente la défense
                break;
            case EFFET_BOOST_VITESSE: // Cas du boost de vitesse
                cible->combattant->vitesse *= (1 + puissance); // Augmente la vitesse
                break;
                default: // Autres cas
                    break; // Ne fait rien
            }
        }
    }

// Fonction pour appliquer tous les effets en cours
void appliquer_effets(Combat* combat) {
    for (int i = 0; i < combat->nombre_participants; i++) { // Parcours des participants
        EtatCombattant* cs = &combat->participants[i]; // Récupère l'état du combattant
        
        for (int j = 0; j < cs->nb_effets; j++) { // Parcours des effets
            EffetTemporaire* eff = &cs->effets[j]; // Récupère l'effet courant
            
            switch (eff->type) { // Switch sur le type d'effet
                case EFFET_AUCUN: // Cas sans effet
                    break; // Ne fait rien
                    
                case EFFET_POISON: // Cas du poison
                    cs->combattant->Vie.courrante -= eff->puissance; // Applique les dégâts
                    printf("%s subit %.1f dégâts de poison!\n", 
                           cs->combattant->nom, eff->puissance); // Affiche le message
                    break;
                
                case EFFET_BOOST_ATTAQUE: // Cas du boost d'attaque
                    break; // Déjà appliqué à l'initialisation
                    
                case EFFET_ETOURDISSEMENT: // Cas de l'étourdissement
                    printf("%s est étourdi et ne peut pas agir!\n", cs->combattant->nom); // Affiche le message
                    break;
                    
                case EFFET_BOOST_DEFENSE: // Cas du boost de défense
                    break; // Déjà appliqué à l'initialisation

                case EFFET_BOOST_VITESSE: // Cas du boost de vitesse
                    break; // Déjà appliqué à l'initialisation
                    
                case EFFET_BRULURE: // Cas de la brûlure
                    float degats_brulure = eff->puissance * (4 - eff->tours_restants); // Calcule les dégâts
                    cs->combattant->Vie.courrante -= degats_brulure; // Applique les dégâts
                    printf("%s subit %.1f dégâts de brûlure! Les flammes s'intensifient!\n", 
                           cs->combattant->nom, degats_brulure); // Affiche le message
                    break;
                    
                case EFFET_RECONSTITUTION: // Cas de la reconstitution
                    float soin = cs->combattant->Vie.max * eff->puissance; // Calcule le soin
                    cs->combattant->Vie.courrante += soin; // Applique le soin
                    if (cs->combattant->Vie.courrante > cs->combattant->Vie.max) { // Si dépassement des PV max
                        cs->combattant->Vie.courrante = cs->combattant->Vie.max; // Limite aux PV max
}
                    printf("%s récupère %.1f points de vie grâce à la reconstitution!\n", 
                           cs->combattant->nom, soin); // Affiche le message
                    break;
                    
                case EFFET_BOUCLIER: // Cas du bouclier
                    for (int k = 0; k < combat->nombre_participants; k++) { // Parcours des participants
                        if (strcmp(combat->participants[k].combattant->nom, "Ronflex") == 0) { // Si c'est Ronflex
                            if (!est_ko(combat->participants[k].combattant)) { // Si Ronflex n'est pas KO
                                float degats_rediriges = cs->combattant->Vie.max * 0.1; // Calcule les dégâts
                                combat->participants[k].combattant->Vie.courrante -= degats_rediriges; // Applique les dégâts
                                printf("Ronflex intercepte l'attaque et subit à la place de %s %.1f points de dégâts!\n",
                                    cs->combattant->nom, degats_rediriges); // Affiche le message
                            } else { // Si Ronflex est KO
                                printf("Ronflex est KO, %s n'est pas protégé !\n", cs->combattant->nom); // Affiche le message
                                retirer_effet_type(combat, cs, EFFET_BOUCLIER); // Retire l'effet
                            }
                            break; // Sort de la boucle
                        }
                    }
                    break;

                case EFFET_PROVOCATION: // Cas de la provocation
                    printf("%s est provoqué et doit attaquer!\n", cs->combattant->nom); // Affiche le message
                    break;
                    
                case EFFET_VOL_DE_VIE: // Cas du vol de vie
                    float vol = cs->combattant->attaque * eff->puissance; // Calcule le vol de vie
                    cs->combattant->Vie.courrante += vol; // Applique le soin
                    if (cs->combattant->Vie.courrante > cs->combattant->Vie.max) { // Si dépassement des PV max
                        cs->combattant->Vie.courrante = cs->combattant->Vie.max; // Limite aux PV max
            }
                    printf("%s vole %.1f points de vie!\n", cs->combattant->nom, vol); // Affiche le message
                    break;
                    }
            
            eff->tours_restants--; // Décrémente la durée
            
            if (eff->tours_restants <= 0) { // Si l'effet est terminé
                retirer_effet_type(combat, cs, eff->type); // Retire l'effet
                j--; // Ajuste l'index
                    }
            }
        }
    }

// Fonction pour retirer un effet à partir de son type
void retirer_effet_type(Combat* combat, EtatCombattant* cible, TypeEffet type) {
    for (int i = 0; i < cible->nb_effets; i++) { // Parcours des effets
        if (cible->effets[i].type == type) { // Si l'effet correspond
            retirer_effet_index(combat, cible, i); // Retire l'effet
            break; // Sort de la boucle
}
    }
}

// Fonction pour retirer un effet à partir de son index
void retirer_effet_index(Combat* combat, EtatCombattant* cible, int index_effet) {
    if (index_effet >= 0 && index_effet < cible->nb_effets) { // Vérifie la validité de l'index
        for (int i = index_effet; i < cible->nb_effets - 1; i++) { // Décale les effets
            cible->effets[i] = cible->effets[i + 1]; // Copie l'effet suivant
        }
        cible->nb_effets--; // Décrémente le nombre d'effets
        
        for (int k = 0; k < combat->nombre_participants; k++) { // Parcours des participants
            EtatCombattant* cs = &combat->participants[k]; // Récupère l'état du combattant
            
            switch (cible->effets[index_effet].type) { // Switch sur le type d'effet
                case EFFET_BOOST_ATTAQUE: // Cas du boost d'attaque
                    if (cs->combattant == cible->combattant) { // Si même combattant
                        cs->combattant->attaque /= (1 + cible->effets[index_effet].puissance); // Annule le boost
                    }
                    break;
                case EFFET_BOOST_DEFENSE: // Cas du boost de défense
                    if (cs->combattant == cible->combattant) { // Si même combattant
                        cs->combattant->defense /= (1 + cible->effets[index_effet].puissance); // Annule le boost
                    }
                    break;
                case EFFET_BOOST_VITESSE: // Cas du boost de vitesse
                    if (cs->combattant == cible->combattant) { // Si même combattant
                        cs->combattant->vitesse /= (1 + cible->effets[index_effet].puissance); // Annule le boost
                    }
                    break;
                default: // Autres cas
                    break; // Ne fait rien
            }
        }
        printf("L'effet %s se dissipe sur %s\n", 
               obtenir_nom_effet(cible->effets[index_effet].type), 
               cible->combattant->nom); // Affiche le message
    }
}