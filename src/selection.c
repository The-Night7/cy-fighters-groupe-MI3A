#include <SDL2/SDL.h>
#include <stdio.h>
#include "selection.h"

void afficher_selection_perso(SDL_Renderer *renderer, SDL_Window *window) {
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
