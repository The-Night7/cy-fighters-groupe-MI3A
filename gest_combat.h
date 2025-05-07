#ifndef GEST_COMBAT_H
#define GEST_COMBAT_H

#include "gestioncombattant.h"

// Fonctions du combat
void initialiser_combat_mode(Combat* combat, Equipe* eq1, Equipe* eq2, bool mode_jvj);
void gerer_tour_combat(Combat* combat);
void attaque_base(EtatCombattant* attaquant, EtatCombattant* cible);
void gerer_tour_joueur(Combat* combat, EtatCombattant* joueur);
int choisir_cible(Combat* combat, TypeJoueur controleur, int tech_index, EtatCombattant* attaquant);
int lire_entier_securise();
float calculer_degats(Combattant* attaquant, Technique* tech, Combattant* cible);
void transition_joueurs(Combat* combat, EtatCombattant* joueur_suivant);

#endif /* GEST_COMBAT_H */
