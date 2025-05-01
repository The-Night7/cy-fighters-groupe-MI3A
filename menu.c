#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

/* Compiler : gcc menu.c -o menu -lSDL2 -I/usr/include/SDL2 -D_REENTRANT
   Exécuter : ./menu  */

void SDL_ExitWithError(const char *message);

int main(int argc, char **argv){

    SDL_Window *window = NULL;

    //Lancement du SDL
    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        SDL_ExitWithError("Initalisation SDL");
    }
    
    //Création de fenêtre
    window = SDL_CreateWindow("CY-Fighters", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1800, 900, 0);

    if(window == NULL){
        SDL_ExitWithError("Création de fenêtre échouée");
    }

    SDL_Delay(5000);


    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;

}

void SDL_ExitWithError(const char *message){
    SDL_Log("ERREUR : %s > %s\n", message, SDL_GetError());
    SDL_Quit();
    exit(EXIT_FAILURE);
}
