#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <stdio.h>
#include <stdlib.h>
#include "selection.h"

#define WINDOW_WIDTH 1800
#define WINDOW_HEIGHT 900

void SDL_ExitWithError(const char *message);
void afficher_selection_ordi(SDL_Renderer*, SDL_Window*);

void surlignerBouton(SDL_Renderer *renderer, SDL_Rect *rect) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 150, 255, 100);  // bleu fluo transparent
    SDL_RenderFillRect(renderer, rect);
}

void afficher_menu_options(SDL_Renderer *renderer, SDL_Window *window) {
    // Charger le fond
    SDL_Surface *fond_surface = SDL_LoadBMP("images/paysage.bmp");
    if (!fond_surface) SDL_ExitWithError("Erreur chargement fond");
    SDL_Texture *fond = SDL_CreateTextureFromSurface(renderer, fond_surface);
    SDL_FreeSurface(fond_surface);

    // Charger le logo "Options"
    SDL_Surface *options_surface = SDL_LoadBMP("images/options.bmp");
    if (!options_surface) SDL_ExitWithError("Erreur chargement options");
    SDL_Texture *options_texture = SDL_CreateTextureFromSurface(renderer, options_surface);
    SDL_FreeSurface(options_surface);

    SDL_Rect rect_options;
    SDL_QueryTexture(options_texture, NULL, NULL, &rect_options.w, &rect_options.h);
    rect_options.x = (WINDOW_WIDTH - rect_options.w) / 2;
    rect_options.y = 50;

    // Charger l'image des instructions
    SDL_Surface *instr_surface = SDL_LoadBMP("images/instructions.bmp");
    if (!instr_surface) SDL_ExitWithError("Erreur chargement instructions");
    SDL_Texture *instr_texture = SDL_CreateTextureFromSurface(renderer, instr_surface);
    SDL_FreeSurface(instr_surface);

    SDL_Rect rect_instr;
    SDL_QueryTexture(instr_texture, NULL, NULL, &rect_instr.w, &rect_instr.h);
    rect_instr.x = (WINDOW_WIDTH - rect_instr.w) / 2;
    rect_instr.y = (WINDOW_HEIGHT - rect_instr.h) / 2;

    // Bouton retour centré sous l’image
    SDL_Rect bouton_retour = {
        .x = (WINDOW_WIDTH - 200) / 2,
        .y = rect_instr.y + rect_instr.h + 30,
        .w = 200,
        .h = 60
    };

    int volume = MIX_MAX_VOLUME;
    float luminosite = 1.0f;
    SDL_bool en_options = SDL_TRUE;

    while (en_options) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                exit(EXIT_SUCCESS);
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        en_options = SDL_FALSE;
                        break;
                    case SDLK_UP:
                        volume += 8;
                        if (volume > MIX_MAX_VOLUME) volume = MIX_MAX_VOLUME;
                        Mix_VolumeMusic(volume);
                        break;
                    case SDLK_DOWN:
                        volume -= 8;
                        if (volume < 0) volume = 0;
                        Mix_VolumeMusic(volume);
                        break;
                    case SDLK_RIGHT:
                        luminosite += 0.1f;
                        if (luminosite > 1.0f) luminosite = 1.0f;
                        SDL_SetWindowBrightness(window, luminosite);
                        break;
                    case SDLK_LEFT:
                        luminosite -= 0.1f;
                        if (luminosite < 0.1f) luminosite = 0.1f;
                        SDL_SetWindowBrightness(window, luminosite);
                        break;
                }
            } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int x = event.button.x;
                int y = event.button.y;
                if (x >= bouton_retour.x && x <= bouton_retour.x + bouton_retour.w &&
                    y >= bouton_retour.y && y <= bouton_retour.y + bouton_retour.h) {
                    en_options = SDL_FALSE;
                }
            }
        }

        // Affichage
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, fond, NULL, NULL);
        SDL_RenderCopy(renderer, options_texture, NULL, &rect_options);
        SDL_RenderCopy(renderer, instr_texture, NULL, &rect_instr);

        // Bouton retour (rectangle noir semi-transparent)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
        SDL_RenderFillRect(renderer, &bouton_retour);

        SDL_RenderPresent(renderer);
    }

    // Libération des textures
    SDL_DestroyTexture(fond);
    SDL_DestroyTexture(options_texture);
    SDL_DestroyTexture(instr_texture);
}

void afficher_choix_mode_jeu(SDL_Renderer *renderer, SDL_Window *window) {
    SDL_Surface *fond_surface = SDL_LoadBMP("images/paysage.bmp");
    if (!fond_surface) SDL_ExitWithError("Erreur fond mode jeu");
    SDL_Texture *fond = SDL_CreateTextureFromSurface(renderer, fond_surface);
    SDL_FreeSurface(fond_surface);

    SDL_Surface *choix_surface = SDL_LoadBMP("images/choix.bmp");
    if (!choix_surface) SDL_ExitWithError("Erreur chargement choix");
    SDL_Texture *choix_texture = SDL_CreateTextureFromSurface(renderer, choix_surface);
    SDL_FreeSurface(choix_surface);

    SDL_Surface *j1j2_surface = SDL_LoadBMP("images/J1vsJ2.bmp");
    SDL_Surface *j1ordi_surface = SDL_LoadBMP("images/J1vsOrdi.bmp");
    if (!j1j2_surface || !j1ordi_surface) SDL_ExitWithError("Erreur chargement modes");

    SDL_Texture *j1j2_tex = SDL_CreateTextureFromSurface(renderer, j1j2_surface);
    SDL_Texture *j1ordi_tex = SDL_CreateTextureFromSurface(renderer, j1ordi_surface);
    SDL_FreeSurface(j1j2_surface);
    SDL_FreeSurface(j1ordi_surface);

    SDL_Rect rect_choix, rect_j1j2, rect_j1ordi;
    SDL_QueryTexture(choix_texture, NULL, NULL, &rect_choix.w, &rect_choix.h);
    SDL_QueryTexture(j1j2_tex, NULL, NULL, &rect_j1j2.w, &rect_j1j2.h);
    SDL_QueryTexture(j1ordi_tex, NULL, NULL, &rect_j1ordi.w, &rect_j1ordi.h);

    // Titre "choix"
    rect_choix.x = (WINDOW_WIDTH - rect_choix.w) / 2;
    rect_choix.y = 40;

    // Espacement horizontal entre les deux boutons
    int espacement = 100;

    // Décalage vertical vers le bas pour laisser de la place en haut
    int decalage_y = 90;

    // Position gauche et droite du centre
    rect_j1j2.x = (WINDOW_WIDTH / 2) - rect_j1j2.w - (espacement / 2);
    rect_j1j2.y = (WINDOW_HEIGHT - rect_j1j2.h) / 2 + decalage_y;

    rect_j1ordi.x = (WINDOW_WIDTH / 2) + (espacement / 2);
    rect_j1ordi.y = (WINDOW_HEIGHT - rect_j1ordi.h) / 2 + decalage_y;

    SDL_Rect bouton_retour = {
        .x = 20, .y = 20, .w = 100, .h = 50
    };


    SDL_bool en_choix = SDL_TRUE;
    while (en_choix) {
        SDL_Event event;
        int x, y;
        SDL_GetMouseState(&x, &y);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                exit(EXIT_SUCCESS);
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    en_choix = SDL_FALSE;
                }
            } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                if (x >= rect_j1j2.x && x <= rect_j1j2.x + rect_j1j2.w &&
                    y >= rect_j1j2.y && y <= rect_j1j2.y + rect_j1j2.h) {
                        afficher_selection_perso(renderer, window);

                } else if (x >= rect_j1ordi.x && x <= rect_j1ordi.x + rect_j1ordi.w &&
                           y >= rect_j1ordi.y && y <= rect_j1ordi.y + rect_j1ordi.h) {
                            afficher_selection_ordi(renderer, window);
                } else if (x >= bouton_retour.x && x <= bouton_retour.x + bouton_retour.w &&
                           y >= bouton_retour.y && y <= bouton_retour.y + bouton_retour.h) {
                    en_choix = SDL_FALSE;
                }
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, fond, NULL, NULL);
        SDL_RenderCopy(renderer, choix_texture, NULL, &rect_choix);
        SDL_RenderCopy(renderer, j1j2_tex, NULL, &rect_j1j2);
        SDL_RenderCopy(renderer, j1ordi_tex, NULL, &rect_j1ordi);

        // Surbrillance
        if (x >= rect_j1j2.x && x <= rect_j1j2.x + rect_j1j2.w &&
            y >= rect_j1j2.y && y <= rect_j1j2.y + rect_j1j2.h) {
            surlignerBouton(renderer, &rect_j1j2);
        }
        if (x >= rect_j1ordi.x && x <= rect_j1ordi.x + rect_j1ordi.w &&
            y >= rect_j1ordi.y && y <= rect_j1ordi.y + rect_j1ordi.h) {
            surlignerBouton(renderer, &rect_j1ordi);
        }

        // Dessiner le bouton retour
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
        SDL_RenderFillRect(renderer, &bouton_retour);

        SDL_RenderPresent(renderer);
    }

    // Nettoyage
    SDL_DestroyTexture(fond);
    SDL_DestroyTexture(j1j2_tex);
    SDL_DestroyTexture(j1ordi_tex);
    SDL_DestroyTexture(choix_texture);
}


int main(int argc, char **argv) {
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        SDL_ExitWithError("Initialisation SDL");

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
        SDL_ExitWithError("Initialisation de SDL_mixer");

    if (SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer) != 0)
        SDL_ExitWithError("Création de fenêtre et de rendu échouée");

    SDL_Surface *fond = SDL_LoadBMP("images/paysage.bmp");
    SDL_Surface *logo = SDL_LoadBMP("images/logo.bmp");
    if (!fond || !logo) SDL_ExitWithError("Erreur chargement fond ou logo");

    SDL_Surface *jouer = SDL_LoadBMP("images/jouer.bmp");
    SDL_Surface *options = SDL_LoadBMP("images/options.bmp");
    SDL_Surface *quitter = SDL_LoadBMP("images/quitter.bmp");
    if (!jouer || !options || !quitter) SDL_ExitWithError("Erreur chargement boutons");

    SDL_Texture *background = SDL_CreateTextureFromSurface(renderer, fond);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, logo);
    SDL_Texture *jouer_tex = SDL_CreateTextureFromSurface(renderer, jouer);
    SDL_Texture *options_tex = SDL_CreateTextureFromSurface(renderer, options);
    SDL_Texture *quitter_tex = SDL_CreateTextureFromSurface(renderer, quitter);
    SDL_FreeSurface(fond); SDL_FreeSurface(logo);
    SDL_FreeSurface(jouer); SDL_FreeSurface(options); SDL_FreeSurface(quitter);

    SDL_Rect rectangle;
    SDL_QueryTexture(texture, NULL, NULL, &rectangle.w, &rectangle.h);
    rectangle.x = (WINDOW_WIDTH - rectangle.w) / 2;
    rectangle.y = WINDOW_HEIGHT / 8;

    SDL_Rect rect_jouer, rect_options, rect_quitter;
    SDL_QueryTexture(jouer_tex, NULL, NULL, &rect_jouer.w, &rect_jouer.h);
    SDL_QueryTexture(options_tex, NULL, NULL, &rect_options.w, &rect_options.h);
    SDL_QueryTexture(quitter_tex, NULL, NULL, &rect_quitter.w, &rect_quitter.h);

    int espace = 40;
    int y_boutons = rectangle.y + rectangle.h + 30;
    int largeur_total = rect_jouer.w + rect_options.w + rect_quitter.w + 2 * espace;
    int depart_x = (WINDOW_WIDTH - largeur_total) / 2;

    rect_jouer.x = depart_x;
    rect_jouer.y = y_boutons;
    rect_options.x = rect_jouer.x + rect_jouer.w + espace;
    rect_options.y = y_boutons;
    rect_quitter.x = rect_options.x + rect_options.w + espace;
    rect_quitter.y = y_boutons;

    SDL_RenderCopy(renderer, background, NULL, NULL);
    SDL_RenderCopy(renderer, texture, NULL, &rectangle);
    SDL_RenderCopy(renderer, jouer_tex, NULL, &rect_jouer);
    SDL_RenderCopy(renderer, options_tex, NULL, &rect_options);
    SDL_RenderCopy(renderer, quitter_tex, NULL, &rect_quitter);
    SDL_RenderPresent(renderer);

    Mix_Music *musique = Mix_LoadMUS("sons/SSBB menu theme.mp3");
    if (!musique) SDL_ExitWithError("Impossible de charger la musique");
    Mix_PlayMusic(musique, -1);

    SDL_bool program_launched = SDL_TRUE;
    while (program_launched) {
        SDL_Event event;
        int x_souris, y_souris;
        SDL_GetMouseState(&x_souris, &y_souris);

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    program_launched = SDL_FALSE;
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        int x = event.button.x;
                        int y = event.button.y;

                        if (x >= rect_quitter.x && x <= rect_quitter.x + rect_quitter.w &&
                            y >= rect_quitter.y && y <= rect_quitter.y + rect_quitter.h) {
                            program_launched = SDL_FALSE;
                        }
                        if (x >= rect_options.x && x <= rect_options.x + rect_options.w &&
                            y >= rect_options.y && y <= rect_options.y + rect_options.h) {
                            afficher_menu_options(renderer, window);
                        }
                        if (x >= rect_jouer.x && x <= rect_jouer.x + rect_jouer.w &&
                            y >= rect_jouer.y && y <= rect_jouer.y + rect_jouer.h) {
                            afficher_choix_mode_jeu(renderer, window);
                        }
                    }
                    break;
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, background, NULL, NULL);
        SDL_RenderCopy(renderer, texture, NULL, &rectangle);
        SDL_RenderCopy(renderer, jouer_tex, NULL, &rect_jouer);
        SDL_RenderCopy(renderer, options_tex, NULL, &rect_options);
        SDL_RenderCopy(renderer, quitter_tex, NULL, &rect_quitter);

        if (x_souris >= rect_jouer.x && x_souris <= rect_jouer.x + rect_jouer.w &&
            y_souris >= rect_jouer.y && y_souris <= rect_jouer.y + rect_jouer.h) {
            surlignerBouton(renderer, &rect_jouer);
        }
        if (x_souris >= rect_options.x && x_souris <= rect_options.x + rect_options.w &&
            y_souris >= rect_options.y && y_souris <= rect_options.y + rect_options.h) {
            surlignerBouton(renderer, &rect_options);
        }
        if (x_souris >= rect_quitter.x && x_souris <= rect_quitter.x + rect_quitter.w &&
            y_souris >= rect_quitter.y && y_souris <= rect_quitter.y + rect_quitter.h) {
            surlignerBouton(renderer, &rect_quitter);
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(jouer_tex);
    SDL_DestroyTexture(options_tex);
    SDL_DestroyTexture(quitter_tex);
    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(background);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_FreeMusic(musique);
    Mix_CloseAudio();
    SDL_Quit();

    return EXIT_SUCCESS;
}



void SDL_ExitWithError(const char *message){
    SDL_Log("ERREUR : %s > %s\n", message, SDL_GetError());
    SDL_Quit();
    exit(EXIT_FAILURE);
}



