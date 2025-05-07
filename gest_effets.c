#include "gest_effets.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h> // pour strcmp

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
                    // Le bouclier est déjà appliqué par le boost de défense
                    // Pas d'effet supplémentaire à chaque tour
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

// Dans la fonction retirer_effet, modifiez le switch pour gérer tous les types d'effets
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
                    
                case EFFET_RECONSTITUTION: // Si reconstitution
                    // La reconstitution n'a pas besoin d'être annulée
                    printf("L'effet de reconstitution sur %s se dissipe.\n", cs->combattant->nom); // Message de fin de reconstitution
                    break;
                    
                case EFFET_BOUCLIER: // Si bouclier
                    // Le bouclier est géré par le boost de défense, pas besoin de code supplémentaire
                    printf("Le bouclier protégeant %s disparaît.\n", cs->combattant->nom); // Message de fin de bouclier
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