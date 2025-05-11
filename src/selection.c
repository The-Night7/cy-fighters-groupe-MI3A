#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "arene.h"

#define NB_PERSOS 8

typedef struct {
    const char *nom;
    SDL_Texture *texture;
    SDL_Texture *nom_texture;
    SDL_Rect rect;
    SDL_Rect nom_rect;
    int pris;
} Personnage;

const int ordre_persos[NB_PERSOS] = {
    0, 1, 2, 3, 4, 5, 6, 7
};

Personnage equipe1_affiche[3];
Personnage equipe2_affiche[3];

const char *noms[NB_PERSOS] = {"musu", "freettle", "sakura", "ronflex", "kirishima", "kingkrool", "marco", "furina"};

// Déclaration d'une variable globale pour la difficulté
int difficulte_selectionnee = 1;

void afficher_selec_difficulte(SDL_Renderer* renderer, SDL_Window* window) {
    SDL_Surface* fond_surface = SDL_LoadBMP("images/paysage.bmp");
    if (!fond_surface) return;
    SDL_Texture* fond = SDL_CreateTextureFromSurface(renderer, fond_surface);
    SDL_FreeSurface(fond_surface);

    SDL_Texture* btn_facile = IMG_LoadTexture(renderer, "images/facile.bmp");
    SDL_Texture* btn_moyen = IMG_LoadTexture(renderer, "images/moyen.bmp");
    SDL_Texture* btn_difficile = IMG_LoadTexture(renderer, "images/difficile.bmp");

    SDL_Rect rect_facile = { (1800 - 200) / 2, 300, 200, 60 };
    SDL_Rect rect_moyen = { (1800 - 200) / 2, 400, 200, 60 };
    SDL_Rect rect_difficile = { (1800 - 200) / 2, 500, 200, 60 };

    SDL_Event event;
    SDL_bool en_fenetre = SDL_TRUE;

    while (en_fenetre) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                en_fenetre = SDL_FALSE;
            }
            else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                en_fenetre = SDL_FALSE;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int x = event.button.x, y = event.button.y;

                if (x >= rect_facile.x && x <= rect_facile.x + rect_facile.w &&
                    y >= rect_facile.y && y <= rect_facile.y + rect_facile.h) {
                    SDL_Log("Mode Facile sélectionné !");
                    difficulte_selectionnee = 0;
                    en_fenetre = SDL_FALSE;
                }
                if (x >= rect_moyen.x && x <= rect_moyen.x + rect_moyen.w &&
                    y >= rect_moyen.y && y <= rect_moyen.y + rect_moyen.h) {
                    SDL_Log("Mode Moyen sélectionné !");
                    difficulte_selectionnee = 1;
                    en_fenetre = SDL_FALSE;
                }
                if (x >= rect_difficile.x && x <= rect_difficile.x + rect_difficile.w &&
                    y >= rect_difficile.y && y <= rect_difficile.y + rect_difficile.h) {
                    SDL_Log("Mode Difficile sélectionné !");
                    difficulte_selectionnee = 2;
                    en_fenetre = SDL_FALSE;
                }
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, fond, NULL, NULL);
        SDL_RenderCopy(renderer, btn_facile, NULL, &rect_facile);
        SDL_RenderCopy(renderer, btn_moyen, NULL, &rect_moyen);
        SDL_RenderCopy(renderer, btn_difficile, NULL, &rect_difficile);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(fond);
    SDL_DestroyTexture(btn_facile);
    SDL_DestroyTexture(btn_moyen);
    SDL_DestroyTexture(btn_difficile);
}


void afficher_selection_perso(SDL_Renderer *renderer, SDL_Window *window) {
    SDL_Surface *fond_surface = SDL_LoadBMP("images/paysage.bmp");
    if (!fond_surface) return;
    SDL_Texture *fond = SDL_CreateTextureFromSurface(renderer, fond_surface);
    SDL_FreeSurface(fond_surface);

    SDL_Texture *titre = IMG_LoadTexture(renderer, "images/btn_perso.bmp");
    SDL_Texture *btn_j1 = IMG_LoadTexture(renderer, "images/T1.bmp");
    SDL_Texture *btn_j2 = IMG_LoadTexture(renderer, "images/T2.bmp");
    SDL_Texture *btn_valider = IMG_LoadTexture(renderer, "images/valider.bmp");

    SDL_Rect rect_titre = {(1800 - 300) / 2, 20, 300, 80};
    SDL_Rect rect_j1 = {80, 140, 200, 70};
    SDL_Rect rect_j2 = {1800 - 280, 140, 200, 70};
    SDL_Rect rect_valider = {(1800 - 200) / 2, 220, 200, 60};

    Personnage persos[NB_PERSOS];
    int largeur = 140, hauteur = 140;
    int x_pos[] = {
        60,                         // Musu 
        60,                         // Freettle 
        (1800 - 140) / 2 - 385,     // Sakura
        (1800 - 140) / 2,           // Ronflex 
        830,                        // Kirishima 
        (1800 - 140) / 2 + 385,     // King K. Rool
        1600,                       // Marco 
        1600                        // Furina 
    };

    int y_pos[] = {
        460,                        // Ligne du haut
        640,                        // Ligne du bas
        460,
        460,
        640,
        460,
        460,
        640
    };

    for (int i = 0; i < NB_PERSOS; i++) {
        char chemin[100];
        sprintf(chemin, "images/icon_%s.bmp", noms[i]);
        persos[i].texture = IMG_LoadTexture(renderer, chemin);
        persos[i].rect = (SDL_Rect){x_pos[i], y_pos[i], largeur, hauteur};
        persos[i].pris = 0;
        persos[i].nom = noms[i];

        char chemin_nom[100];
        sprintf(chemin_nom, "images/nom_%s.bmp", noms[i]);
        persos[i].nom_texture = IMG_LoadTexture(renderer, chemin_nom);
        persos[i].nom_rect = (SDL_Rect){x_pos[i], y_pos[i] + hauteur + 5, largeur, 30};
    }

    int equipe1[3], equipe2[3], equipe1_count = 0, equipe2_count = 0;
    SDL_Rect pos_equipe1[3] = {{80, 220, 80, 80}, {240, 220, 80, 80}, {400, 220, 80, 80}};
    SDL_Rect pos_equipe2[3] = {{1600, 220, 80, 80}, {1440, 220, 80, 80}, {1280, 220, 80, 80}};
    int historique[6], historique_top = 0;
    int tour = 1, selection_en_cours = 0, index_selection = -1;

    SDL_Event event;
    SDL_bool en_fenetre = SDL_TRUE;

    while (en_fenetre) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || 
               (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                en_fenetre = SDL_FALSE;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_TAB && historique_top > 0) {
                int i = historique[--historique_top];
                persos[i].rect = (SDL_Rect){x_pos[i], y_pos[i], largeur, hauteur};
                persos[i].nom_rect = (SDL_Rect){x_pos[i], y_pos[i] + hauteur + 5, largeur, 30};
                if (persos[i].pris == 1) { equipe1_count--; tour = 1; }
                else if (persos[i].pris == 2) { equipe2_count--; tour = 2; }
                persos[i].pris = 0;
            } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int x = event.button.x, y = event.button.y;

                if (equipe1_count == 3 && equipe2_count == 3 &&
                    x >= rect_valider.x && x <= rect_valider.x + rect_valider.w &&
                    y >= rect_valider.y && y <= rect_valider.y + rect_valider.h) {

                    for (int i = 0; i < 3; i++) {
                        equipe1_affiche[i] = persos[equipe1[i]];
                        equipe2_affiche[i] = persos[equipe2[i]];
                    }

                    int retour = afficher_arene(renderer);
                    if (retour == 1)
                        afficher_selection_perso(renderer, window);
                    en_fenetre = SDL_FALSE;
                }

                for (int i = 0; i < NB_PERSOS; i++) {
                    if (persos[i].pris == 0 &&
                        x >= persos[i].rect.x && x <= persos[i].rect.x + largeur &&
                        y >= persos[i].rect.y && y <= persos[i].rect.y + hauteur) {
                        index_selection = i;
                        selection_en_cours = 1;
                        break;
                    }
                }
            } else if (event.type == SDL_MOUSEBUTTONUP && selection_en_cours && index_selection != -1) {
                if (tour == 1 && equipe1_count < 3) {
                    persos[index_selection].rect = pos_equipe1[equipe1_count];
                    equipe1[equipe1_count++] = index_selection;
                    persos[index_selection].pris = 1;
                    historique[historique_top++] = index_selection;
                    tour = 2;
                } else if (tour == 2 && equipe2_count < 3) {
                    persos[index_selection].rect = pos_equipe2[equipe2_count];
                    equipe2[equipe2_count++] = index_selection;
                    persos[index_selection].pris = 2;
                    historique[historique_top++] = index_selection;
                    tour = 1;
                }
                selection_en_cours = 0;
                index_selection = -1;
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, fond, NULL, NULL);
        SDL_RenderCopy(renderer, titre, NULL, &rect_titre);
        SDL_RenderCopy(renderer, btn_j1, NULL, &rect_j1);
        SDL_RenderCopy(renderer, btn_j2, NULL, &rect_j2);

        for (int i = 0; i < NB_PERSOS; i++) {
            SDL_RenderCopy(renderer, persos[i].texture, NULL, &persos[i].rect);
            if (persos[i].pris == 0)
                SDL_RenderCopy(renderer, persos[i].nom_texture, NULL, &persos[i].nom_rect);
        }

        if (equipe1_count == 3 && equipe2_count == 3)
            SDL_RenderCopy(renderer, btn_valider, NULL, &rect_valider);

        SDL_RenderPresent(renderer);
    }

    for (int i = 0; i < NB_PERSOS; i++) {
        SDL_DestroyTexture(persos[i].texture);
        SDL_DestroyTexture(persos[i].nom_texture);
    }

    SDL_DestroyTexture(fond);
    SDL_DestroyTexture(titre);
    SDL_DestroyTexture(btn_j1);
    SDL_DestroyTexture(btn_j2);
    SDL_DestroyTexture(btn_valider);
}




void afficher_selection_ordi(SDL_Renderer *renderer, SDL_Window *window) {
    afficher_selec_difficulte(renderer, window);

    SDL_Surface *fond_surface = SDL_LoadBMP("images/paysage.bmp");
    if (!fond_surface) return;
    SDL_Texture *fond = SDL_CreateTextureFromSurface(renderer, fond_surface);
    SDL_FreeSurface(fond_surface);

    SDL_Texture *titre = IMG_LoadTexture(renderer, "images/btn_perso.bmp");
    SDL_Texture *btn_j1 = IMG_LoadTexture(renderer, "images/T1.bmp");
    SDL_Texture *btn_j2 = IMG_LoadTexture(renderer, "images/TO.bmp");
    SDL_Texture *btn_valider = IMG_LoadTexture(renderer, "images/valider.bmp");

    SDL_Rect rect_titre = {(1800 - 300) / 2, 20, 300, 80};
    SDL_Rect rect_j1 = {80, 140, 200, 70};
    SDL_Rect rect_j2 = {1800 - 280, 140, 200, 70};
    SDL_Rect rect_valider = {(1800 - 200) / 2, 220, 200, 60};

    Personnage persos[NB_PERSOS];
    int largeur = 140, hauteur = 140;
    int x_pos[] = {
        60,                         // Musu 
        60,                         // Freettle 
        (1800 - 140) / 2 - 385,     // Sakura
        (1800 - 140) / 2,           // Ronflex 
        830,                        // Kirishima 
        (1800 - 140) / 2 + 385,     // King K. Rool
        1600,                       // Marco 
        1600                        // Furina 
    };

    int y_pos[] = {
        460,                        // Ligne du haut
        640,                        // Ligne du bas
        460,
        460,
        640,
        460,
        460,
        640
    };

    for (int i = 0; i < NB_PERSOS; i++) {
        char chemin[100];
        sprintf(chemin, "images/icon_%s.bmp", noms[i]);
        persos[i].texture = IMG_LoadTexture(renderer, chemin);
        persos[i].rect = (SDL_Rect){x_pos[i], y_pos[i], largeur, hauteur};
        persos[i].pris = 0;
        persos[i].nom = noms[i];

        char chemin_nom[100];
        sprintf(chemin_nom, "images/nom_%s.bmp", noms[i]);
        persos[i].nom_texture = IMG_LoadTexture(renderer, chemin_nom);
        persos[i].nom_rect = (SDL_Rect){x_pos[i], y_pos[i] + hauteur + 5, largeur, 30};
    }

    int equipe1[3] = {-1, -1, -1};
    int equipe2[3] = {-1, -1, -1};
    int equipe1_count = 0, equipe2_count = 0;

    SDL_Rect pos_equipe1[3] = {{80, 220, 80, 80}, {240, 220, 80, 80}, {400, 220, 80, 80}};
    SDL_Rect pos_equipe2[3] = {{1600, 220, 80, 80}, {1440, 220, 80, 80}, {1280, 220, 80, 80}};

    int historique[6];
    int historique_top = 0;

    int selection_en_cours = 0;
    int index_selection = -1;

    SDL_Event event;
    SDL_bool en_fenetre = SDL_TRUE;

    while (en_fenetre) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT ||
                (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                afficher_selection_ordi(renderer, window);  // Retour au menu des persos
                return;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_TAB && historique_top >= 2) {
                int ordi = historique[--historique_top];
                int joueur = historique[--historique_top];

                persos[ordi].rect = (SDL_Rect){x_pos[ordi], y_pos[ordi], largeur, hauteur};
                persos[ordi].nom_rect = (SDL_Rect){x_pos[ordi], y_pos[ordi] + hauteur + 5, largeur, 30};
                persos[ordi].pris = 0;
                equipe2_count--;

                persos[joueur].rect = (SDL_Rect){x_pos[joueur], y_pos[joueur], largeur, hauteur};
                persos[joueur].nom_rect = (SDL_Rect){x_pos[joueur], y_pos[joueur] + hauteur + 5, largeur, 30};
                persos[joueur].pris = 0;
                equipe1_count--;
            } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int x = event.button.x, y = event.button.y;

                if (equipe1_count == 3 && equipe2_count == 3 &&
                    x >= rect_valider.x && x <= rect_valider.x + rect_valider.w &&
                    y >= rect_valider.y && y <= rect_valider.y + rect_valider.h) {

                    for (int i = 0; i < 3; i++) {
                        equipe1_affiche[i] = persos[equipe1[i]];
                        equipe2_affiche[i] = persos[equipe2[i]];
                    }

                    afficher_arene(renderer);
                    return;
                }

                for (int i = 0; i < NB_PERSOS; i++) {
                    if (persos[i].pris == 0 &&
                        x >= persos[i].rect.x && x <= persos[i].rect.x + largeur &&
                        y >= persos[i].rect.y && y <= persos[i].rect.y + hauteur) {
                        index_selection = i;
                        selection_en_cours = 1;
                        break;
                    }
                }
            } else if (event.type == SDL_MOUSEBUTTONUP && selection_en_cours && index_selection != -1) {
                if (equipe1_count < 3) {
                    persos[index_selection].rect = pos_equipe1[equipe1_count];
                    equipe1[equipe1_count++] = index_selection;
                    persos[index_selection].pris = 1;
                    historique[historique_top++] = index_selection;

                    int r = rand() % NB_PERSOS;
                    while (persos[r].pris != 0) r = rand() % NB_PERSOS;

                    persos[r].rect = pos_equipe2[equipe2_count];
                    equipe2[equipe2_count++] = r;
                    persos[r].pris = 2;
                    historique[historique_top++] = r;
                }
                selection_en_cours = 0;
                index_selection = -1;
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, fond, NULL, NULL);
        SDL_RenderCopy(renderer, titre, NULL, &rect_titre);
        SDL_RenderCopy(renderer, btn_j1, NULL, &rect_j1);
        SDL_RenderCopy(renderer, btn_j2, NULL, &rect_j2);

        for (int i = 0; i < NB_PERSOS; i++) {
            SDL_RenderCopy(renderer, persos[i].texture, NULL, &persos[i].rect);
            if (persos[i].pris == 0)
                SDL_RenderCopy(renderer, persos[i].nom_texture, NULL, &persos[i].nom_rect);
        }

        if (equipe1_count == 3 && equipe2_count == 3)
            SDL_RenderCopy(renderer, btn_valider, NULL, &rect_valider);

        SDL_RenderPresent(renderer);
    }

    for (int i = 0; i < NB_PERSOS; i++) {
        SDL_DestroyTexture(persos[i].texture);
        SDL_DestroyTexture(persos[i].nom_texture);
    }

    SDL_DestroyTexture(fond);
    SDL_DestroyTexture(titre);
    SDL_DestroyTexture(btn_j1);
    SDL_DestroyTexture(btn_j2);
    SDL_DestroyTexture(btn_valider);
}









