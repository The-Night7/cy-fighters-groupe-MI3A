#include "gestioncombattant.h"

/*fonction qui remplie la structure Technique (1 appel de la fct = les données de UNE technique sont stocké dans différentes variables),
si la technique possède un effet(exemple : étourdissement) alors la fonction est initialisée avec possède = true,
lorsque possède = true, la fonction remplie des informations supplémentaires comme le nom de l'effet et la durée de cet effet.
note : la variable cible est un entier qui correspond à : 1 cible unique,2 plusieurs cibles,
De même pour la variable type : 1 dégats, 2 soins, 3 bouclier,4 brulûre*/

void init_technique(Technique *tech, const char *nom, const char *description, const char *cible, int ncible,
                    float puissance, int nb_tour_recharge, bool possede, const char *nom_effet, int nb_tour_actifs, int type)
{
    tech->nom = strdup(nom);
    tech->description = strdup(description);
    tech->puissance = puissance;
    tech->cible = strdup(cible);
    tech->nb_tour_recharge = nb_tour_recharge;
    tech->activable = true;
    tech->cible = cible;
    tech->Effet.possede = possede;
    tech->type = type;
    if (possede)
    {
        tech->Effet.nom = strdup(nom_effet);
        tech->Effet.nb_tour_actifs = nb_tour_actifs;
    }
    return;
}

/*Permet de free les données des combattants après un combat par exemple
(d'ailleurs j'ai oublié de free nom_effet, je m'en occuperai)*/
void detruire_combattant(Combattant *combattant)
{
    if (!combattant)
        return;

    free(combattant->nom);
    for (int i = 0; i < MAX_TECHNIQUES; i++)
    {
        free(combattant->techniques[i].nom);
        free(combattant->techniques[i].description);
        free(combattant->techniques[i].Effet.nom);
    }
    free(combattant);
}

/*Dans cette fonction permet de créer les combattants en utilisant la fonction init_techniques
Plus globalement cette fonction contient toutes les données de toute les combattant pour les stocker à dans des cases mémoire en cas de besoin.*/
Combattant *creer_combattant(const char *nom)
{
    if (!nom)
        return NULL;
    Combattant *combattant = malloc(sizeof(Combattant));
    if (!combattant)
        return NULL;
    combattant->nom = strdup(nom);

    // Personnage
    if (strcmp(nom, "Musu") == 0)
    {

        combattant->Vie.courrante = combattant->Vie.max = 200;
        combattant->attaque = 100;
        combattant->defense = 20;
        combattant->agility = 0;
        combattant->speed = 100;

        init_technique(&combattant->techniques[0], "Tempête",
                       "Inflige des dégats d'eau majeurs. Doit se recharger.",
                       "Un ennemi", 1,
                       0.75, 2, 0, NULL, 0, 1);

        init_technique(&combattant->techniques[1], "Reconstitution",
                       "Restaure quelques points de vie à chaque tour pour tous les alliés pendant 2 tours. Doit se recharger.",
                       "Plusieurs alliés", 2,
                       0, 4, 1, "Reconstitution", 3, 2);

        init_technique(&combattant->techniques[2], "Eau énergisante",
                       "Restaure 20 % de vie. S'applique un boost de dégats pendant 2 tours",
                       "Un allié", 1,
                       0, 0, 1, "Boost dégats", 2, 5);
    }

    else if (strcmp(nom, "Freettle") == 0)
    {

        combattant->Vie.courrante = combattant->Vie.max = 200;
        combattant->attaque = 100;
        combattant->defense = 20;
        combattant->agility = 0;
        combattant->speed = 60;

        init_technique(&combattant->techniques[0], "Poing de la revanche",
                       "Inflige des dégats mineurs",
                       "Un ennemi", 1,
                       0.3, 0, 0, NULL, 0, 1);

        init_technique(&combattant->techniques[1], "Etincelle de feu",
                       "Inflige des dégats Mineurs et brûle la cible",
                       "Un ennemi", 1,
                       0, 2, 1, "Brulûre", 3, 4);

        init_technique(&combattant->techniques[2], "Mur infranchissable",
                       "Applique un bouclier moyen à la cible",
                       "Un allié", 1,
                       0, 0, 1, "Bouclier Moyen", 2, 3);
    }

    else
    {
        free(combattant->nom);
        free(combattant);
        return NULL;
    }

    return combattant;
}