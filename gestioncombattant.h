typedef enum {
    EFFET_AUCUN,
    EFFET_POISON,
    EFFET_ETOURDISSEMENT,
    EFFET_BOOST_ATTAQUE,
    EFFET_BOOST_DEFENSE,
    EFFET_BOOST_VITESSE,
    EFFET_BRULURE,
    EFFET_RECONSTITUTION,
    EFFET_BOUCLIER,
    EFFET_PROVOCATION,
    EFFET_VOL_DE_VIE
} TypeEffet;

typedef struct {
    char *nom;
    struct {
        float courrante;
        float max;  
    } Vie;
    float attaque;
    float defense;
    float vitesse;
    Technique techniques[MAX_TECHNIQUES];
} Combattant;