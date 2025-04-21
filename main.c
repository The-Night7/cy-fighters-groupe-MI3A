#include "main.h"
#include "gestioncombattant.h"

int main(void)
{
    Combattant *combattant = creer_combattant("Musu");
    if (combattant)
    {
        printf("Created Combattant: %s\n", combattant->nom);
        printf("Health: %.0f/%.0f\n", combattant->Vie.courrante, combattant->Vie.max);
        printf("Techniques:\n");
        for (int i = 0; i < MAX_TECHNIQUES; i++)
        {
            printf("- %s: %s\n",
                   combattant->techniques[i].nom,
                   combattant->techniques[i].description);
        }
        detruire_combattant(combattant);
    }
    return 0;
}