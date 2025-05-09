#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string.h>

#define NB_PERSOS 6

typedef struct {
    const char *nom;
    SDL_Texture *texture;
    SDL_Texture *nom_texture;
    SDL_Rect rect;
    SDL_Rect nom_rect;
    int pris; // 0 = non, 1 = équipe 1, 2 = équipe 2
} Personnage;

void afficher_selection_perso(SDL_Renderer *renderer, SDL_Window *window) {
    SDL_Surface *fond_surface = SDL_LoadBMP("images/paysage.bmp");
    if (!fond_surface) return;
    SDL_Texture *fond = SDL_CreateTextureFromSurface(renderer, fond_surface);
    SDL_FreeSurface(fond_surface);

    SDL_Texture *titre = IMG_LoadTexture(renderer, "images/btn_perso.bmp");
    SDL_Texture *btn_j1 = IMG_LoadTexture(renderer, "images/T1.bmp");
    SDL_Texture *btn_j2 = IMG_LoadTexture(renderer, "images/T2.bmp");

    SDL_Rect rect_titre = { (1800 - 300) / 2, 20, 300, 80 };
    SDL_Rect rect_j1 = { 80, 140, 200, 70 };
    SDL_Rect rect_j2 = { 1800 - 200 - 80, 140, 200, 70 };

    const char *noms[NB_PERSOS] = {"musu", "freettle", "ronflex", "kirishima", "marco", "furina"};
    Personnage persos[NB_PERSOS];

    int largeur = 140, hauteur = 140;
    int x_pos[] = {60, 60, (1800 - largeur) / 2, (1800 - largeur) / 2, 1800 - largeur - 60, 1800 - largeur - 60};
    int y_pos[] = {460, 640, 460, 640, 460, 640};

    for (int i = 0; i < NB_PERSOS; i++) {
        char chemin[100];
        sprintf(chemin, "images/icon_%s.bmp", noms[i]);
        persos[i].texture = IMG_LoadTexture(renderer, chemin);
        persos[i].rect = (SDL_Rect){x_pos[i], y_pos[i], largeur, hauteur};
        persos[i].pris = 0;
        persos[i].nom = noms[i];

        // Chargement du nom du personnage en image
        char chemin_nom[100];
        sprintf(chemin_nom, "images/nom_%s.bmp", noms[i]);
        persos[i].nom_texture = IMG_LoadTexture(renderer, chemin_nom);
        persos[i].nom_rect = (SDL_Rect){
            persos[i].rect.x,
            persos[i].rect.y + persos[i].rect.h + 5,
            largeur,
            hauteur / 4
        };
    }

    int equipe1_count = 0, equipe2_count = 0;
    SDL_Rect equipe1_pos[3] = {
        {rect_j1.x, rect_j1.y + 80, 80, 80},
        {rect_j1.x, rect_j1.y + 170, 80, 80},
        {rect_j1.x, rect_j1.y + 260, 80, 80}
    };
    SDL_Rect equipe2_pos[3] = {
        {rect_j2.x + rect_j2.w - 80, rect_j2.y + 80, 80, 80},
        {rect_j2.x + rect_j2.w - 80, rect_j2.y + 170, 80, 80},
        {rect_j2.x + rect_j2.w - 80, rect_j2.y + 260, 80, 80}
    };

    int tour = 1;
    int selection_en_cours = 0;
    int index_selection = -1;

    // Historique des sélections pour pouvoir faire retour avec Tab
    int historique_selection[6];
    int top_historique = 0;

    SDL_bool en_fenetre = SDL_TRUE;
    SDL_Event event;

    while (en_fenetre) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                en_fenetre = SDL_FALSE;

            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    en_fenetre = SDL_FALSE;

                } else if (event.key.keysym.sym == SDLK_TAB) {
                    if (top_historique > 0) {
                        int i = historique_selection[--top_historique];

                        persos[i].rect = (SDL_Rect){x_pos[i], y_pos[i], largeur, hauteur};
                        persos[i].nom_rect = (SDL_Rect){
                            persos[i].rect.x,
                            persos[i].rect.y + persos[i].rect.h + 5,
                            largeur,
                            hauteur / 4
                        };

                        if (persos[i].pris == 1 && equipe1_count > 0) {
                            equipe1_count--;
                            tour = 1;
                        } else if (persos[i].pris == 2 && equipe2_count > 0) {
                            equipe2_count--;
                            tour = 2;
                        }

                        persos[i].pris = 0;
                    }
                }

            } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int x = event.button.x, y = event.button.y;
                for (int i = 0; i < NB_PERSOS; i++) {
                    if (persos[i].pris == 0 &&
                        x >= persos[i].rect.x && x <= persos[i].rect.x + persos[i].rect.w &&
                        y >= persos[i].rect.y && y <= persos[i].rect.y + persos[i].rect.h) {
                        if (!selection_en_cours) {
                            index_selection = i;
                            selection_en_cours = 1;
                        }
                        break;
                    }
                }
            } else if (event.type == SDL_MOUSEBUTTONUP && selection_en_cours && index_selection != -1) {
                if (tour == 1 && equipe1_count < 3) {
                    persos[index_selection].rect = equipe1_pos[equipe1_count++];
                    persos[index_selection].pris = 1;
                    tour = 2;
                    historique_selection[top_historique++] = index_selection;
                } else if (tour == 2 && equipe2_count < 3) {
                    persos[index_selection].rect = equipe2_pos[equipe2_count++];
                    persos[index_selection].pris = 2;
                    tour = 1;
                    historique_selection[top_historique++] = index_selection;
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
            SDL_RenderCopy(renderer, persos[i].nom_texture, NULL, &persos[i].nom_rect);
        }

        if (selection_en_cours && index_selection >= 0) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 180);
            SDL_RenderFillRect(renderer, &persos[index_selection].rect);
        }

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
}




// Menu des personnages pour le mode "Joueur 1 VS l'Ordinateur"
void afficher_selection_ordi(SDL_Renderer *renderer, SDL_Window *window) {
    SDL_Surface *fond_surface = SDL_LoadBMP("images/paysage.bmp");
    if (!fond_surface) {
        SDL_Log("Erreur chargement fond : %s", SDL_GetError());
        return;
    }

    SDL_Texture *fond = SDL_CreateTextureFromSurface(renderer, fond_surface);
    SDL_FreeSurface(fond_surface);

    SDL_bool en_fenetre = SDL_TRUE;
    SDL_Event event;

    while (en_fenetre) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || 
               (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                en_fenetre = SDL_FALSE;
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, fond, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(fond);
}
