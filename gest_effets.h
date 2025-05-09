#ifndef GEST_EFFETS_H // Empêche l'inclusion multiple du fichier d'en-tête
#define GEST_EFFETS_H // Définit le symbole de protection d'inclusion multiple

#include "gestioncombattant.h" // Inclut les définitions des structures de combattants
// Fonctions de gestion des effets
void appliquer_effet(EtatCombattant* cible, TypeEffet effet, int duree, float puissance); // Applique un effet sur un combattant cible
void retirer_effet_type(Combat* combat, EtatCombattant* cible, TypeEffet type); // Retire un effet spécifique d'un combattant
void retirer_effet_index(Combat* combat, EtatCombattant* cible, int index_effet); // Retire l'effet à l'index spécifié d'un combattant
void appliquer_effets(Combat* combat); // Applique tous les effets actifs dans le combat
TypeEffet convertir_nom_effet(const char* nom_effet); // Convertit une chaîne de caractères en type d'effet
const char* obtenir_nom_effet(TypeEffet effet); // Obtient le nom textuel d'un type d'effet

#endif // Fin de la protection d'inclusion multiple
