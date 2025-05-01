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

    SDL_Surface *logo = NULL;
    SDL_Texture *texture = NULL;
    SDL_Surface *fond = NULL;
    SDL_Texture *background = NULL;

    //Initialisation de l'image et du fond
    fond = SDL_LoadBMP("images/paysage.bmp");
    logo = SDL_LoadBMP("images/bouton_jouer.bmp");
    

    if(logo == NULL || fond == NULL){
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_ExitWithError("Impossible de charger les images");
    }

    background = SDL_CreateTextureFromSurface(renderer, fond);
    SDL_FreeSurface(fond);
    texture = SDL_CreateTextureFromSurface(renderer, logo);
    SDL_FreeSurface(logo);
    

    if(texture == NULL || background == NULL){
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_ExitWithError("Impossible de créer la texture");
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
