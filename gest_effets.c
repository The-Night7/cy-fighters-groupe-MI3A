#include "gest_effets.h"
#include "util_combat.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h> // pour strcmp

// Fonction pour convertir une chaîne d'effet en TypeEffet
TypeEffet convertir_nom_effet(const char* nom_effet) {
    if (!nom_effet) return EFFET_AUCUN;
    
    if (strcmp(nom_effet, "EFFET_POISON") == 0) return EFFET_POISON;
    if (strcmp(nom_effet, "EFFET_ETOURDISSEMENT") == 0) return EFFET_ETOURDISSEMENT;
    if (strcmp(nom_effet, "EFFET_BOOST_ATTAQUE") == 0) return EFFET_BOOST_ATTAQUE;
    if (strcmp(nom_effet, "EFFET_BOOST_DEFENSE") == 0) return EFFET_BOOST_DEFENSE;
    if (strcmp(nom_effet, "EFFET_BRULURE") == 0) return EFFET_BRULURE;
    if (strcmp(nom_effet, "EFFET_RECONSTITUTION") == 0) return EFFET_RECONSTITUTION;
    if (strcmp(nom_effet, "EFFET_BOUCLIER") == 0) return EFFET_BOUCLIER;
    
    return EFFET_AUCUN;
}

// Fonction pour obtenir le nom d'un effet (à ajouter)
const char* obtenir_nom_effet(TypeEffet effet) {
    switch(effet) {
        case EFFET_RECONSTITUTION: return "Reconstitution";
        case EFFET_POISON: return "Poison";
        case EFFET_ETOURDISSEMENT: return "Étourdissement";
        case EFFET_BOOST_ATTAQUE: return "Boost d'attaque";
        case EFFET_BOOST_DEFENSE: return "Boost de défense";
        case EFFET_BRULURE: return "Brûlure";
        case EFFET_BOUCLIER: return "Bouclier";
        default: return "Aucun";
    }
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

// Dans la fonction appliquer_effets, modifiez le switch pour gérer tous les types d'effets
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
                    
                case EFFET_RECONSTITUTION: // Si reconstitution
                    // Soigne le combattant progressivement
                    float soin = cs->combattant->Vie.max * eff->puissance; // Calcul du soin
                    cs->combattant->Vie.courrante += soin; // Applique le soin
                    if (cs->combattant->Vie.courrante > cs->combattant->Vie.max) {
                        cs->combattant->Vie.courrante = cs->combattant->Vie.max; // Limite aux PV max
                    }
                    printf("%s récupère %.1f points de vie grâce à la reconstitution!\n", 
                           cs->combattant->nom, soin); // Message de soin
                    break;
                    
                case EFFET_BOUCLIER: // Si bouclier
                    for (int k = 0; k < combat->nombre_participants; k++) {
                        if (strcmp(combat->participants[k].combattant->nom, "Ronflex") == 0) {
                            if (!est_ko(combat->participants[k].combattant)) {
                                float degats_rediriges = cs->combattant->Vie.max * 0.1;
                                combat->participants[k].combattant->Vie.courrante -= degats_rediriges;
                                printf("Ronflex intercepte l'attaque et subit à la place de %s %.1f points de dégâts!\n",
                                    cs->combattant->nom, degats_rediriges);
                            } else {
                                printf("Ronflex est KO, %s n'est pas protégé !\n", cs->combattant->nom);
                                retirer_effet_type(combat, cs, EFFET_BOUCLIER);
                            }
                                    break;
                        }
                    }
                    break;
            }
            
            // Décrémenter le compteur
            eff->tours_restants--; // Réduit la durée restante
            
            // Si effet terminé
            if (eff->tours_restants <= 0) { // Si l'effet est terminé
                retirer_effet_type(combat, cs, eff->type); // Retire l'effet
                j--; // On revient en arrière car l'effet a été retiré
            }
        }
    }
}

// Fonction pour retirer un effet à partir de son type
void retirer_effet_type(Combat* combat, EtatCombattant* cible, TypeEffet type) {
    // Chercher l'effet du type spécifié
    for (int i = 0; i < cible->nb_effets; i++) {
        if (cible->effets[i].type == type) {
            retirer_effet_index(combat, cible, i);
            break;
        }
    }
}

// Fonction pour retirer un effet à partir de son index
void retirer_effet_index(Combat* combat, EtatCombattant* cible, int index_effet) {
    if (index_effet >= 0 && index_effet < cible->nb_effets) {
        // Déplacer tous les effets suivants d'une position vers le haut
        for (int i = index_effet; i < cible->nb_effets - 1; i++) {
            cible->effets[i] = cible->effets[i + 1];
        }
        cible->nb_effets--;
        
        // Mise à jour des effets sur les autres combattants si nécessaire
        for (int k = 0; k < combat->nombre_participants; k++) {
            EtatCombattant* cs = &combat->participants[k];
            
            switch (cible->effets[index_effet].type) {
                case EFFET_BOOST_ATTAQUE:
                    if (cs->combattant == cible->combattant) {
                        cs->combattant->attaque /= (1 + cible->effets[index_effet].puissance);
                    }
                    break;
                case EFFET_BOOST_DEFENSE:
                    if (cs->combattant == cible->combattant) {
                        cs->combattant->defense /= (1 + cible->effets[index_effet].puissance);
                    }
                    break;
                case EFFET_POISON:
                case EFFET_ETOURDISSEMENT:
                case EFFET_BRULURE:
                case EFFET_RECONSTITUTION:
                case EFFET_BOUCLIER:
                case EFFET_AUCUN:
                    // Ces effets n'ont pas besoin de nettoyage spécial
                    break;
            }
        }

        printf("L'effet %s se dissipe sur %s\n", 
               obtenir_nom_effet(cible->effets[index_effet].type), 
               cible->combattant->nom);
    }
}