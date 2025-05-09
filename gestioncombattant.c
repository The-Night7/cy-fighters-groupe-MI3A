#include "gestioncombattant.h"

/*fonction qui remplie la structure Technique (1 appel de la fct = les données de UNE technique sont stocké dans différentes variables),
si la technique possède un effet(exemple : étourdissement) alors la fonction est initialisée avec possède = true,
lorsque possède = true, la fonction remplie des informations supplémentaires comme le nom de l'effet et la durée de cet effet.
note : la variable cible est un entier qui correspond à : 1 cible unique,2 plusieurs cibles,3 soi-même,
De même pour la variable type : 0 boost, 1 dégats, 2 soins, 3 bouclier*/

void init_technique(Technique *tech, const char *nom, const char *description, const char *cible, int ncible,
                    float puissance, int nb_tour_recharge, bool possede, const char *nom_effet, int nb_tour_actifs, int type)
{
    tech->nom = strdup(nom);
    tech->description = strdup(description);
    tech->puissance = puissance;
    tech->cible = strdup(cible);        // Copie de la chaîne de caractères
    tech->nb_tour_recharge = nb_tour_recharge;
    tech->activable = true;
    tech->ncible = ncible;              // Correction: utiliser ncible au lieu de réassigner cible
    tech->Effet.possede = possede;
    tech->type = type;
    if (possede)
    {
        tech->Effet.nom = strdup(nom_effet);
        tech->Effet.nb_tour_actifs = nb_tour_actifs;
    }
    else
    {
        tech->Effet.nom = NULL;         // Initialiser à NULL pour éviter des problèmes lors de la libération
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
        free(combattant->techniques[i].cible);   
        if (combattant->techniques[i].Effet.possede)
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
        combattant->speed = 115;

        init_technique(&combattant->techniques[0], "Tempête",
                       "Inflige des dégats d'eau majeurs. Doit se recharger.",
                       "Un ennemi", 1,
                       0.75, 2, 0, NULL, 0, 1);

        init_technique(&combattant->techniques[1], "Reconstitution",
                       "Restaure quelques points de vie à chaque tour pour tous les alliés pendant 2 tours. Doit se recharger.",
                       "Plusieurs alliés", 2,
                       -0.2, 4, 1, "EFFET_RECONSTITUTION", 3, 2); // puissance = taux de guérison -> négatif car si puissance <=0 alors soin.

        init_technique(&combattant->techniques[2], "Eau énergisante",
                       "Restaure 20 % de vie. S'applique un boost de dégats pendant 2 tours",
                       "Un allié", 1,
                       -0.2, 0, 1, "EFFET_BOOST_ATTAQUE", 2, 2);
    }

    else if (strcmp(nom, "Freettle") == 0)
    {

        combattant->Vie.courrante = combattant->Vie.max = 200;
        combattant->attaque = 100;
        combattant->defense = 20;
        combattant->speed = 110;

        init_technique(&combattant->techniques[0], "Poing de la revanche",
                       "Inflige des dégats mineurs",
                       "Un ennemi", 1,
                       0.3, 0, 0, NULL, 0, 1);

        init_technique(&combattant->techniques[1], "Etincelle de feu",
                       "Inflige des dégats Mineurs et brûle la cible",
                       "Un ennemi", 1,
                       0, 2, 1, "EFFET_BRULURE", 3, 4);

        init_technique(&combattant->techniques[2], "Mur infranchissable",
                       "Applique un bouclier moyen à la cible",
                       "Un allié", 1,
                       0, 0, 1, "EFFET_BOUCLIER", 2, 3);
    }
    
    else if (strcmp(nom, "Marco") == 0)
    {

        combattant->Vie.courrante = combattant->Vie.max = 150;
        combattant->attaque = 60;
        combattant->defense = 80;
        combattant->speed = 120;

        init_technique(&combattant->techniques[0], "Flammes régénératrice",
                       "Flammes redonnant 50% des pv max",
                       "Tous les alliés", 2,
                       -0.5, 3, 0, NULL, 0, 2);

        init_technique(&combattant->techniques[1], "Blue bird",
                       "Inflige des dégats moyens à la cible",
                       "Un ennemi", 1,
                       0.4, 0, 0, NULL, 0, 1);

        init_technique(&combattant->techniques[2], "Flamme énergisantes",
                       "Applique un boost de dégats à toute l'équipe",
                       "Tous les alliés", 2,
                       0, 1, 1, "EFFET_BOOST_ATTAQUE", 2, 0);
    }
    
    else if (strcmp(nom, "Ronflex") == 0)
    {

        combattant->Vie.courrante = combattant->Vie.max = 400;
        combattant->attaque = 120;
        combattant->defense = 150;
        combattant->speed = 50;

        init_technique(&combattant->techniques[0], "Plaquage",
                       "Ecrase un ennemi avec son ventre",
                       "Un ennemi", 1,
                       0.6, 0, 0, NULL, 0, 1);

        init_technique(&combattant->techniques[1], "Gloutonnerie",
                       "Se soigne de 30%",
                       "Soi-même", 3,
                       -0.3, 1, 0, NULL, 0, 2);

        init_technique(&combattant->techniques[2], "Protection frontale",
                       "Applique un boost de dégats à toute l'équipe",
                       "Un allié", 1,
                       0, 1, 1, "EFFET_BOUCLIER", 2, 3);
    }

    else if (strcmp(nom, "Furina") == 0)
    {

        combattant->Vie.courrante = combattant->Vie.max = 250;
        combattant->attaque = 80;
        combattant->defense = 100;
        combattant->speed = 130;

        init_technique(&combattant->techniques[0], "Bénédiction aquatique",
                       "Soigne un allié de 100PV",
                       "Un allié", 1,
                       -100, 4, 0, NULL, 0, 2);

        init_technique(&combattant->techniques[1], "Danse de la pluie",
                       "Augmente l'agilité de toute l'équipe",
                       "Plusieurs alliés", 2,
                       0, 5, 1, "EFFET_BOOST_AGILITE", 2, 0);

        init_technique(&combattant->techniques[2], "Bouclier hydrique",
                       "Crée une barrière qui absorbe 50% des dégats pendant 1 tour",
                       "Un allié", 1,
                       0, 3, 1, "EFFET_BOUCLIER", 1, 3);
    }

    else if (strcmp(nom, "Kirishima") == 0)
    {

        combattant->Vie.courrante = combattant->Vie.max = 350;
        combattant->attaque = 140;
        combattant->defense = 180;
        combattant->speed = 60;

        init_technique(&combattant->techniques[0], "Frappe Dévastatrice",
                       "Attaque puissante qui ignore 30% de la défense ennemie",
                       "Un ennemi", 1,
                       0.7, 2, 0, NULL, 0, 1);

        init_technique(&combattant->techniques[1], "Endurance du guerrier",
                       "Augmente sa propre défense de 50% pendant 2 tours",
                       "Soi-même", 3,
                       0, 4, 1, "EFFET_BOOST_DEFENSE", 2, 0);

        init_technique(&combattant->techniques[2], "Provocation",
                       "Force les ennemis à le cibler",
                       "Soi-même", 3,
                       0, 3, 1, "EFFET_Provocation", 2, 0);
    }

    else
    {
        free(combattant->nom);
        free(combattant);
        return NULL;
    }

    return combattant;
}
