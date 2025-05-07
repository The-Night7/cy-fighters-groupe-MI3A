#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <stdio.h>
#include "selection.h"

void afficher_selection_perso(SDL_Renderer *renderer, SDL_Window *window) {

    // Fond
    SDL_Surface *fond_surface = SDL_LoadBMP("images/paysage.bmp");
    if (!fond_surface) {
        SDL_Log("Erreur chargement fond : %s", SDL_GetError());
        return;
    }
    SDL_Texture *fond = SDL_CreateTextureFromSurface(renderer, fond_surface);
    SDL_FreeSurface(fond_surface);

    // Bouton Personnages
    SDL_Surface *surface_titre = SDL_LoadBMP("images/btn_perso.bmp");  
    if (!surface_titre) {
        SDL_Log("Erreur chargement image Personnages : %s", SDL_GetError());
        SDL_DestroyTexture(fond);
        return;
    }
    SDL_Texture *texture_titre = SDL_CreateTextureFromSurface(renderer, surface_titre);
    SDL_FreeSurface(surface_titre);

    SDL_Rect rect_titre;
    SDL_QueryTexture(texture_titre, NULL, NULL, &rect_titre.w, &rect_titre.h);
    rect_titre.x = (1800 - rect_titre.w) / 2;
    rect_titre.y = 20;

    // Charger bouton ÉQUIPE J1
    SDL_Surface *surface_j1 = IMG_Load("images/T1.bmp");
    if (!surface_j1) {
        SDL_Log("Erreur chargement bouton J1 : %s", SDL_GetError());
        SDL_DestroyTexture(fond);
        SDL_DestroyTexture(texture_titre);
        return;
    }   
    SDL_Texture *texture_j1 = SDL_CreateTextureFromSurface(renderer, surface_j1);
    SDL_FreeSurface(surface_j1);

    SDL_Rect rect_j1;
    SDL_QueryTexture(texture_j1, NULL, NULL, &rect_j1.w, &rect_j1.h);
    rect_j1.x = 80;               // Position à gauche
    rect_j1.y = 140;              // En dessous du bouton titre

    // Charger bouton ÉQUIPE J2
    SDL_Surface *surface_j2 = IMG_Load("images/T2.bmp");
    if (!surface_j2) {
        SDL_Log("Erreur chargement bouton J2 : %s", SDL_GetError());
        SDL_DestroyTexture(fond);
        SDL_DestroyTexture(texture_titre);
        SDL_DestroyTexture(texture_j1);
        return;
    }
    SDL_Texture *texture_j2 = SDL_CreateTextureFromSurface(renderer, surface_j2);
    SDL_FreeSurface(surface_j2);

    SDL_Rect rect_j2;
    SDL_QueryTexture(texture_j2, NULL, NULL, &rect_j2.w, &rect_j2.h);
    rect_j2.x = 1800 - rect_j2.w - 80;  // Position à droite
    rect_j2.y = 140;                   // Même hauteur que J1


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
        SDL_RenderCopy(renderer, texture_titre, NULL, &rect_titre);
        SDL_RenderCopy(renderer, texture_j1, NULL, &rect_j1);
        SDL_RenderCopy(renderer, texture_j2, NULL, &rect_j2);
        
        SDL_RenderPresent(renderer);
    }
    
    SDL_DestroyTexture(texture_j1);
    SDL_DestroyTexture(texture_j2);
    SDL_DestroyTexture(texture_titre);
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
