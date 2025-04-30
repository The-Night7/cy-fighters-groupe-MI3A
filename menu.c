#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

void SDL_ExitWithError(const char *message);
int estClique(SDL_Event *event, SDL_Rect *bouton);

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        SDL_ExitWithError("SDL Init");

    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
        SDL_ExitWithError("SDL_image Init");

    SDL_Window *window = SDL_CreateWindow("CY Fighters", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    if (!window)
        SDL_ExitWithError("CreateWindow");

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
        SDL_ExitWithError("CreateRenderer");

    // Chargement des textures
    SDL_Texture *fond = IMG_LoadTexture(renderer, "ressources/images/fond_menu.png");
    SDL_Texture *logo = IMG_LoadTexture(renderer, "ressources/images/titre_jeu.png");

    SDL_Texture *btn_jouer = IMG_LoadTexture(renderer, "ressources/images/btn_jouer.png");
    SDL_Texture *btn_options = IMG_LoadTexture(renderer, "ressources/images/btn_options.png");
    SDL_Texture *btn_quitter = IMG_LoadTexture(renderer, "ressources/images/btn_quitter.png");

    if (!fond || !logo || !btn_jouer || !btn_options || !btn_quitter)
        SDL_ExitWithError("Erreur chargement image");

    // Définir la taille standard réduite des boutons
    int btn_width = 220;
    int btn_height = 60;
    int center_x = 290;

    SDL_Rect rect_logo = {150, 40, 500, 120};
    SDL_Rect rect_jouer = {center_x, 200, btn_width, btn_height};
    SDL_Rect rect_options = {center_x, 280, btn_width, btn_height};
    SDL_Rect rect_quitter = {center_x, 360, btn_width, btn_height};

    SDL_Event event;
    SDL_bool enCours = SDL_TRUE;

    while (enCours) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                enCours = SDL_FALSE;

            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                if (estClique(&event, &rect_jouer)) {
                    // Logique de démarrage du jeu
                    SDL_Log("JOUER cliqué !");
                } else if (estClique(&event, &rect_options)) {
                    SDL_Log("OPTIONS cliqué !");
                } else if (estClique(&event, &rect_quitter)) {
                    SDL_Log("QUITTER cliqué !");
                    enCours = SDL_FALSE;
                }
            }
        }

        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, fond, NULL, NULL);
        SDL_RenderCopy(renderer, logo, NULL, &rect_logo);
        SDL_RenderCopy(renderer, btn_jouer, NULL, &rect_jouer);
        SDL_RenderCopy(renderer, btn_options, NULL, &rect_options);
        SDL_RenderCopy(renderer, btn_quitter, NULL, &rect_quitter);

        SDL_RenderPresent(renderer);
    }

    // Nettoyage
    SDL_DestroyTexture(fond);
    SDL_DestroyTexture(logo);
    SDL_DestroyTexture(btn_jouer);
    SDL_DestroyTexture(btn_options);
    SDL_DestroyTexture(btn_quitter);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return EXIT_SUCCESS;
}

int estClique(SDL_Event *event, SDL_Rect *bouton) {
    return (event->button.x >= bouton->x && event->button.x <= bouton->x + bouton->w &&
            event->button.y >= bouton->y && event->button.y <= bouton->y + bouton->h);
}

void SDL_ExitWithError(const char *message) {
    SDL_Log("Erreur : %s > %s\n", message, SDL_GetError());
    SDL_Quit();
    exit(EXIT_FAILURE);
}

