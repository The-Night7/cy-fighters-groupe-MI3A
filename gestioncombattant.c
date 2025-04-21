#include "gestioncombattant.h"

void init_technique(Technique *tech, const char *nom, const char *description, const char *cible,
                    float puissance, int nb_tour_recharge, bool possede, const char *nom_effet, int nb_tour_actifs)
{
    tech->nom = strdup(nom);
    tech->description = strdup(description);
    tech->puissance = puissance;
    tech->cible = strdup(cible);
    tech->nb_tour_recharge = nb_tour_recharge;
    tech->activable = true;
    tech->Effet.possede = possede;
    if (possede)
    {
        tech->Effet.nom = strdup(nom_effet);
        tech->Effet.nb_tour_actifs = nb_tour_actifs;
    }
    return;
}

void detruire_combattant(Combattant *combattant)
{
    if (!combattant)
        return;

    free(combattant->nom);
    for (int i = 0; i < MAX_TECHNIQUES; i++)
    {
        free(combattant->techniques[i].nom);
        free(combattant->techniques[i].description);
    }
    free(combattant);
}

Combattant *creer_combattant(const char *nom)
{
    if (!nom)
        return NULL;
    Combattant *combattant = malloc(sizeof(Combattant));
    if (!combattant)
        return NULL;
    combattant->nom = strdup(nom);

    if (strcmp(nom, "Musu") == 0)
    {

        combattant->Vie.courrante = combattant->Vie.max = 200;
        combattant->attaque = 100;
        combattant->defense = 20;
        combattant->agility = 0;
        combattant->speed = 20;

        init_technique(&combattant->techniques[0], "Tempête",
                       "Inflige des dégats d'eau majeurs. Doit se recharger.",
                       "Un ennemi",
                       0.75, 2, 0, NULL, 0);

        init_technique(&combattant->techniques[1], "Reconstitution",
                       "Restaure quelques points de vie à chaque tour pour tous les alliés pendant 2 tours. Doit se recharger.",
                       "Plusieurs alliés",
                       0, 4, 1, "Reconstitution", 3);

        init_technique(&combattant->techniques[2], "Eau énergisante",
                       "Restaure 20 % de vie. S'applique un boost de dégats pendant 2 tours",
                       "Un allié",
                       0, 0, 1, "Boost dégats", 2);
    }

    else
    {
        free(combattant->nom);
        free(combattant);
        return NULL;
    }

    return combattant;
}