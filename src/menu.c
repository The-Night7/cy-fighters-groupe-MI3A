#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define WINDOW_WIDTH 1800
#define WINDOW_HEIGHT 900

void SDL_ExitWithError(const char *message);

int main(int argc, char **argv){

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    //Lancement du SDL
    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        SDL_ExitWithError("Initalisation SDL");
    }
    
    //Création fenêtre + rendu
    if(SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer) != 0){
        SDL_ExitWithError("Création de fenêtre et de rendu échouée");
    }

    //Titre + Fond
    SDL_Surface *logo = NULL;
    SDL_Texture *texture = NULL;
    SDL_Surface *fond = NULL;
    SDL_Texture *background = NULL;

    //Boutons
    SDL_Surface *jouer = NULL;
    SDL_Texture *jouer_tex = NULL;
    SDL_Surface *options = NULL;
    SDL_Texture *options_tex = NULL;
    SDL_Surface *quitter = NULL;
    SDL_Texture *quitter_tex = NULL;



    //Initialisation du titre et du fond
    fond = SDL_LoadBMP("images/paysage.bmp");
    logo = SDL_LoadBMP("images/logo.bmp");

    //Initialisation des boutons
    jouer = SDL_LoadBMP("images/jouer.bmp");
    options = SDL_LoadBMP("images/options.bmp");
    quitter = SDL_LoadBMP("images/quitter.bmp");

    

    if(logo == NULL || fond == NULL){
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_ExitWithError("Impossible de charger les images");
    }

    if (jouer == NULL || options == NULL || quitter == NULL) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_ExitWithError("Erreur de chargement de bouton");
    }

    //Initialisation titre + fond (textures)
    background = SDL_CreateTextureFromSurface(renderer, fond);
    SDL_FreeSurface(fond);
    texture = SDL_CreateTextureFromSurface(renderer, logo);
    SDL_FreeSurface(logo);

    //Initialisation boutons (textures)
    jouer_tex = SDL_CreateTextureFromSurface(renderer, jouer);
    SDL_FreeSurface(jouer);
    options_tex = SDL_CreateTextureFromSurface(renderer, options);
    SDL_FreeSurface(options);
    quitter_tex = SDL_CreateTextureFromSurface(renderer, quitter);
    SDL_FreeSurface(quitter);

    

    if(texture == NULL || background == NULL){
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_ExitWithError("Impossible de créer la texture");
    }

    if (jouer_tex == NULL || options_tex == NULL || quitter_tex == NULL) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_ExitWithError("Erreur de création d'une texture de bouton");
    }

    SDL_Rect rectangle;

    if(SDL_QueryTexture(texture, NULL, NULL, &rectangle.w, &rectangle.h) != 0){
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_ExitWithError("Impossible de charger la texture");
    }

    rectangle.x = (WINDOW_WIDTH - rectangle.w) / 2;
    rectangle.y = WINDOW_HEIGHT / 8;


    if(SDL_RenderCopy(renderer, background, NULL, NULL) != 0){
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_ExitWithError("Impossible d'afficher le fond");
    }

    if(SDL_RenderCopy(renderer, texture, NULL, &rectangle) != 0){
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_ExitWithError("Impossible d'afficher la texture");
    }

    SDL_Rect rect_jouer, rect_options, rect_quitter;

    SDL_QueryTexture(jouer_tex, NULL, NULL, &rect_jouer.w, &rect_jouer.h);
    SDL_QueryTexture(options_tex, NULL, NULL, &rect_options.w, &rect_options.h);
    SDL_QueryTexture(quitter_tex, NULL, NULL, &rect_quitter.w, &rect_quitter.h);


    // Espace horizontal entre les boutons
    int espace = 40;

    // Y commun : en dessous du logo
    int y_boutons = rectangle.y + rectangle.h + 30;

    // Largeur totale (3 boutons + 2 espaces)
    int largeur_total = rect_jouer.w + rect_options.w + rect_quitter.w + 2 * espace;

    // Point de départ pour centrer l'ensemble
    int depart_x = (WINDOW_WIDTH - largeur_total) / 2;

    // Position des trois boutons côte à côte
    rect_jouer.x = depart_x;
    rect_jouer.y = y_boutons;

    rect_options.x = rect_jouer.x + rect_jouer.w + espace;
    rect_options.y = y_boutons;

    rect_quitter.x = rect_options.x + rect_options.w + espace;
    rect_quitter.y = y_boutons;


    SDL_RenderCopy(renderer, jouer_tex, NULL, &rect_jouer);
    SDL_RenderCopy(renderer, options_tex, NULL, &rect_options);
    SDL_RenderCopy(renderer, quitter_tex, NULL, &rect_quitter);


    SDL_RenderPresent(renderer);
    
    SDL_bool program_launched = SDL_TRUE;

    while(program_launched){
        SDL_Event event;
        while(SDL_PollEvent(&event) == 1){
            switch(event.type){
                
                case SDL_QUIT:
                    program_launched = SDL_FALSE;
                    break;
                
                default:
                    break;
            }
        }
    }


    SDL_DestroyTexture(jouer_tex);
    SDL_DestroyTexture(options_tex);
    SDL_DestroyTexture(quitter_tex);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(background);
    SDL_Quit();

    return EXIT_SUCCESS;

}

void SDL_ExitWithError(const char *message){
    SDL_Log("ERREUR : %s > %s\n", message, SDL_GetError());
    SDL_Quit();
    exit(EXIT_FAILURE);
}


/* 
    SDL_RENDERER_SOFTWARE
    SDL_RENDERER_ACCELERATED
    SDL_RENDERER_PRESENTVSYNC
    SDL_RENDERER_TARGETTEXTURE
*/
