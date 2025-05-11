#include "arene.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>


// Redéclaration locale de Personnage pour éviter les conflits
typedef struct {
    const char *nom;
    SDL_Texture *texture;
    SDL_Texture *nom_texture;
    SDL_Rect rect;
    SDL_Rect nom_rect;
    int pris;
} Personnage;

extern Personnage equipe1_affiche[3];
extern Personnage equipe2_affiche[3];

int afficher_popup_retour(SDL_Renderer* renderer) {
    SDL_Texture* popup = IMG_LoadTexture(renderer, "images/retour_perso.bmp");
    if (!popup) {
        SDL_Log("Erreur chargement image popup : %s", SDL_GetError());
        return 0;
    }

    // Popup en haut à gauche, plus petit
    SDL_Rect rect_popup = { 10, 10, 350, 200 };
    SDL_Rect rect_oui = { rect_popup.x + 30, rect_popup.y + 120, 120, 50 };
    SDL_Rect rect_non = { rect_popup.x + 200, rect_popup.y + 120, 120, 50 };

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

    Mix_Music* musique_arene = Mix_LoadMUS("sons/mario.mp3");
    if (!musique_arene) {
        SDL_Log("Erreur chargement musique arène : %s", Mix_GetError());
    } else {
        Mix_PlayMusic(musique_arene, -1); // -1 = boucle infinie
    }


    // 🔽 Taille réduite (largeur et hauteur)
    int sprite_largeur = 120;
    int sprite_hauteur = 180;

    // 🔼 Plus en haut → modifie y ici (ex: 400 ou 300)
    int y_placement = 450;

    // Positions des 3 personnages à gauche
    SDL_Rect positions_gauche[3] = {
        {250, y_placement, sprite_largeur, sprite_hauteur},
        {400, y_placement, sprite_largeur, sprite_hauteur},
        {550, y_placement, sprite_largeur, sprite_hauteur}
    };

    // Positions des 3 personnages à droite
    SDL_Rect positions_droite[3] = {
        {1450, y_placement, sprite_largeur, sprite_hauteur},
        {1300, y_placement, sprite_largeur, sprite_hauteur},
        {1150, y_placement, sprite_largeur, sprite_hauteur}
    };

    // Chargement des sprites full
    SDL_Texture* full_sprites[6];
    for (int i = 0; i < 3; i++) {
        char chemin1[100], chemin2[100];
        sprintf(chemin1, "images/perso_%s.bmp", equipe1_affiche[i].nom);
        sprintf(chemin2, "images/perso_%s.bmp", equipe2_affiche[i].nom);

        full_sprites[i] = IMG_LoadTexture(renderer, chemin1);
        full_sprites[i + 3] = IMG_LoadTexture(renderer, chemin2);
    }

    SDL_Event event;
    SDL_bool en_fenetre = SDL_TRUE;

    while (en_fenetre) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) return 0;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                int retour = afficher_popup_retour(renderer);
                if (retour == 1) return 1;
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, fond, NULL, NULL);

        for (int i = 0; i < 3; i++) {
            SDL_RenderCopy(renderer, full_sprites[i], NULL, &positions_gauche[i]);
            SDL_RenderCopy(renderer, full_sprites[i + 3], NULL, &positions_droite[i]);
        }

        SDL_RenderPresent(renderer);
    }

    for (int i = 0; i < 6; i++) {
        SDL_DestroyTexture(full_sprites[i]);
    }
    SDL_DestroyTexture(fond);
    Mix_HaltMusic();
    Mix_FreeMusic(musique_arene);


    return 0;
}
