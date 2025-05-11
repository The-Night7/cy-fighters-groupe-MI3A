#include "arene.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

int afficher_popup_retour(SDL_Renderer* renderer) {
    SDL_Texture* popup = IMG_LoadTexture(renderer, "images/retour_perso.bmp");
    if (!popup) {
        SDL_Log("Erreur chargement image popup : %s", SDL_GetError());
        return 0;
    }

    SDL_Rect rect_popup = { (1800 - 700) / 2, (1000 - 400) / 2, 700, 400 };
    SDL_Rect rect_oui = { rect_popup.x + 90, rect_popup.y + 250, 200, 80 };
    SDL_Rect rect_non = { rect_popup.x + 410, rect_popup.y + 250, 200, 80 };

    SDL_Event event;
    SDL_bool attendre_reponse = SDL_TRUE;

    while (attendre_reponse) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) return 0;

            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int x = event.button.x, y = event.button.y;
                if (x >= rect_oui.x && x <= rect_oui.x + rect_oui.w &&
                    y >= rect_oui.y && y <= rect_oui.y + rect_oui.h) {
                    SDL_DestroyTexture(popup);
                    return 1; // retour à la sélection
                }
                if (x >= rect_non.x && x <= rect_non.x + rect_non.w &&
                    y >= rect_non.y && y <= rect_non.y + rect_non.h) {
                    SDL_DestroyTexture(popup);
                    return 0; // fermer popup
                }
            }
        }

        SDL_RenderCopy(renderer, popup, NULL, &rect_popup);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(popup);
    return 0;
}


int afficher_arene(SDL_Renderer* renderer) {
    SDL_Surface* fond_surface = SDL_LoadBMP("images/arene.bmp");
    if (!fond_surface) return 0;
    SDL_Texture* fond = SDL_CreateTextureFromSurface(renderer, fond_surface);
    SDL_FreeSurface(fond_surface);

    SDL_bool en_fenetre = SDL_TRUE;
    SDL_Event event;

    while (en_fenetre) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                return 0;

            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                int retour = afficher_popup_retour(renderer);
                if (retour == 1) {
                    return 1;  // retour à la sélection
                }
                // sinon on continue l'arène
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, fond, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(fond);
    return 0;
}
