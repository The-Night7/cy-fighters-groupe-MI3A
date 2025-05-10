#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define NB_PERSOS 8  // Ajout de King K. Rool

typedef struct {
    const char *nom;
    SDL_Texture *texture;
    SDL_Texture *nom_texture;
    SDL_Rect rect;
    SDL_Rect nom_rect;
    int pris; // 0 = non, 1 = équipe 1, 2 = équipe 2
} Personnage;

// Définition d'un tableau d'indices pour l'ordre des personnages
const int ordre_persos[NB_PERSOS] = {
    0, 1, 2, 3, 4, 5, 6, 7 // Musu, Freettle, Sakura, Ronflex, Kirishima, King K. Rool, Marco, Furina
};

// Tableau des noms des personnages
const char *noms[NB_PERSOS] = {"musu", "freettle", "sakura", "ronflex", "kirishima", "kingkrool", "marco", "furina"};

void afficher_selection_perso(SDL_Renderer *renderer, SDL_Window *window) {
    // Charger l'image de fond
    SDL_Surface *fond_surface = SDL_LoadBMP("images/paysage.bmp");
    if (!fond_surface) return;
    SDL_Texture *fond = SDL_CreateTextureFromSurface(renderer, fond_surface);
    SDL_FreeSurface(fond_surface);

    // Charger les images des boutons
    SDL_Texture *titre = IMG_LoadTexture(renderer, "images/btn_perso.bmp");
    SDL_Texture *btn_j1 = IMG_LoadTexture(renderer, "images/T1.bmp");
    SDL_Texture *btn_j2 = IMG_LoadTexture(renderer, "images/T2.bmp");

    // Position des titres et des boutons
    SDL_Rect rect_titre = { (1800 - 300) / 2, 20, 300, 80 };
    SDL_Rect rect_j1 = { 80, 140, 200, 70 };
    SDL_Rect rect_j2 = { 1800 - 200 - 80, 140, 200, 70 };

    // Déclaration du tableau des personnages et de leur taille
    Personnage persos[NB_PERSOS];
    int largeur = 140, hauteur = 140;

    // Positions fixes des personnages
    int x_pos[] = {
        60, 60, (1800 - largeur) / 2 - 385, (1800 - largeur) / 2, (1800 - largeur) / 2, (1800 - largeur) / 2 + 385, 1800 - largeur - 60, 1800 - largeur - 60
    };

    int y_pos[] = {
        460, 640, 460, 460, 640, 460, 460, 640
    };

    // Charger les personnages
    for (int i = 0; i < NB_PERSOS; i++) {
        char chemin[100];
        sprintf(chemin, "images/icon_%s.bmp", noms[ordre_persos[i]]);
        persos[i].texture = IMG_LoadTexture(renderer, chemin);
        persos[i].rect = (SDL_Rect){x_pos[i], y_pos[i], largeur, hauteur};
        persos[i].pris = 0;  // Personnage initialement non pris
        persos[i].nom = noms[ordre_persos[i]];

        // Charger le nom du personnage depuis "images/nom_%s.bmp"
        char chemin_nom[100];
        sprintf(chemin_nom, "images/nom_%s.bmp", noms[ordre_persos[i]]);
        persos[i].nom_texture = IMG_LoadTexture(renderer, chemin_nom);
        persos[i].nom_rect = (SDL_Rect){
            persos[i].rect.x,
            persos[i].rect.y + persos[i].rect.h + 5, // Position du nom juste en dessous de l'image
            largeur,
            30 // Taille du nom, ajustez si nécessaire
        };
    }

    int equipe1_count = 0, equipe2_count = 0;
    SDL_Rect equipe1_pos[3] = {
        {rect_j1.x, rect_j1.y + 80, 80, 80},  // Premier personnage de l'équipe 1
        {rect_j1.x + 160, rect_j1.y + 80, 80, 80},  // Deuxième perso à droite du premier
        {rect_j1.x + 320, rect_j1.y + 80, 80, 80}   // Troisième perso à droite du deuxième perso
    };
    
    SDL_Rect equipe2_pos[3] = {
        {rect_j2.x + rect_j2.w - 80, rect_j2.y + 80, 80, 80},  // Premier personnage de l'équipe 2
        {rect_j2.x + rect_j2.w - 240, rect_j2.y + 80, 80, 80},  // Deuxième perso à gauche du premier
        {rect_j2.x + rect_j2.w - 400, rect_j2.y + 80, 80, 80}   // Troisième perso à gauche du deuxième perso
    };

    int tour = 1;  // Le tour du joueur commence
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

                        // Remise en place du personnage dans son emplacement initial
                        persos[i].rect = (SDL_Rect){x_pos[i], y_pos[i], largeur, hauteur};
                        persos[i].nom_rect = (SDL_Rect){
                            persos[i].rect.x,
                            persos[i].rect.y + persos[i].rect.h + 5,
                            largeur,
                            30 // Taille du nom
                        };

                        // Réinitialisation de l'état pris et de l'équipe
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

        // Afficher les personnages
        for (int i = 0; i < NB_PERSOS; i++) {
            SDL_RenderCopy(renderer, persos[i].texture, NULL, &persos[i].rect);
            
            // Afficher le nom sous le personnage seulement si le personnage n'est pas pris
            if (persos[i].pris == 0) {
                SDL_RenderCopy(renderer, persos[i].nom_texture, NULL, &persos[i].nom_rect);  // Afficher le nom sous le personnage
            }

            // Ajouter un effet de surbrillance sur le personnage sélectionné par le joueur
            if (selection_en_cours && index_selection == i) {
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 180); // Jaune avec transparence
                SDL_RenderFillRect(renderer, &persos[i].rect); // Dessiner le rectangle de surbrillance
            }
        }

        SDL_RenderPresent(renderer);
    }

    // Libérer les ressources
    for (int i = 0; i < NB_PERSOS; i++) {
        SDL_DestroyTexture(persos[i].texture);
        SDL_DestroyTexture(persos[i].nom_texture);
    }

    SDL_DestroyTexture(fond);
    SDL_DestroyTexture(titre);
    SDL_DestroyTexture(btn_j1);
    SDL_DestroyTexture(btn_j2);
}


void afficher_selection_ordi(SDL_Renderer *renderer, SDL_Window *window) {
    // Charger l'image de fond
    SDL_Surface *fond_surface = SDL_LoadBMP("images/paysage.bmp");
    if (!fond_surface) return;
    SDL_Texture *fond = SDL_CreateTextureFromSurface(renderer, fond_surface);
    SDL_FreeSurface(fond_surface);

    // Charger les images des boutons
    SDL_Texture *titre = IMG_LoadTexture(renderer, "images/btn_perso.bmp");
    SDL_Texture *btn_j1 = IMG_LoadTexture(renderer, "images/T1.bmp");
    SDL_Texture *btn_j2 = IMG_LoadTexture(renderer, "images/TO.bmp");

    // Position des titres et des boutons
    SDL_Rect rect_titre = { (1800 - 300) / 2, 20, 300, 80 };
    SDL_Rect rect_j1 = { 80, 140, 200, 70 };
    SDL_Rect rect_j2 = { 1800 - 200 - 80, 140, 200, 70 };

    // Déclaration du tableau des personnages et de leur taille
    Personnage persos[NB_PERSOS];
    int largeur = 140, hauteur = 140;

    // Positions fixes des personnages
    int x_pos[] = {
        60, 60, (1800 - largeur) / 2 - 385, (1800 - largeur) / 2, (1800 - largeur) / 2, (1800 - largeur) / 2 + 385, 1800 - largeur - 60, 1800 - largeur - 60
    };

    int y_pos[] = {
        460, 640, 460, 460, 640, 460, 460, 640
    };

    // Charger les personnages
    for (int i = 0; i < NB_PERSOS; i++) {
        char chemin[100];
        sprintf(chemin, "images/icon_%s.bmp", noms[ordre_persos[i]]);
        persos[i].texture = IMG_LoadTexture(renderer, chemin);
        persos[i].rect = (SDL_Rect){x_pos[i], y_pos[i], largeur, hauteur};
        persos[i].pris = 0;  // Personnage initialement non pris
        persos[i].nom = noms[ordre_persos[i]];

        // Charger le nom du personnage depuis "images/nom_%s.bmp"
        char chemin_nom[100];
        sprintf(chemin_nom, "images/nom_%s.bmp", noms[ordre_persos[i]]);
        persos[i].nom_texture = IMG_LoadTexture(renderer, chemin_nom);
        persos[i].nom_rect = (SDL_Rect){
            persos[i].rect.x,
            persos[i].rect.y + persos[i].rect.h + 5, // Position du nom juste en dessous de l'image
            largeur,
            30 // Taille du nom, ajustez si nécessaire
        };
    }

    int equipe1_count = 0, equipe2_count = 0;
    SDL_Rect equipe1_pos[3] = {
        {rect_j1.x, rect_j1.y + 80, 80, 80},  // Premier personnage de l'équipe 1
        {rect_j1.x + 160, rect_j1.y + 80, 80, 80},  // Deuxième perso à droite du premier
        {rect_j1.x, rect_j1.y + 170, 80, 80}   // Troisième perso en dessous du premier
    };
    
    SDL_Rect equipe2_pos[3] = {
        {rect_j2.x + rect_j2.w - 80, rect_j2.y + 80, 80, 80},  // Premier personnage de l'équipe 2
        {rect_j2.x + rect_j2.w - 240, rect_j2.y + 80, 80, 80},  // Deuxième perso à gauche du premier
        {rect_j2.x + rect_j2.w - 80, rect_j2.y + 170, 80, 80}   // Troisième perso en dessous du premier
    };

    int tour = 1;  // Le tour du joueur commence
    int selection_en_cours = 0;
    int index_selection = -1;

    // Historique des sélections pour pouvoir faire retour avec Tab
    int historique_selection[6];
    int top_historique = 0;

    SDL_bool en_fenetre = SDL_TRUE;
    SDL_Event event;

    // Liste des indices des personnages restants pour l'ordinateur
    int persos_restants[NB_PERSOS];
    for (int i = 0; i < NB_PERSOS; i++) {
        persos_restants[i] = i;  // Initialement, tous les personnages sont disponibles
    }

    // Suivi des personnages sélectionnés dans chaque équipe
    int equipe1[3] = {-1, -1, -1};  // Personnages de l'équipe 1
    int equipe2[3] = {-1, -1, -1};  // Personnages de l'équipe 2

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
                            30 // Taille du nom
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
                    // Le premier personnage sélectionné dans l'équipe 1
                    persos[index_selection].rect = equipe1_pos[equipe1_count++];
                    persos[index_selection].pris = 1;
                    equipe1[equipe1_count - 1] = index_selection; // Enregistrer le personnage choisi
                    tour = 2;
                    historique_selection[top_historique++] = index_selection;

                    // L'ordinateur choisit immédiatement son personnage
                    int random_index = rand() % NB_PERSOS;  // Sélection aléatoire parmi tous les personnages
                    while (persos[random_index].pris != 0) {
                        random_index = rand() % NB_PERSOS;  // Si déjà pris, recommencer
                    }

                    // L'ordinateur choisit ce personnage
                    persos[random_index].rect = equipe2_pos[equipe2_count++];
                    persos[random_index].pris = 2;
                    equipe2[equipe2_count - 1] = random_index; // Enregistrer le personnage choisi

                    // Marquer le personnage comme pris
                    persos[random_index].pris = 2;

                    tour = 1;
                    historique_selection[top_historique++] = random_index;
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

        // Afficher les personnages
        for (int i = 0; i < NB_PERSOS; i++) {
            SDL_RenderCopy(renderer, persos[i].texture, NULL, &persos[i].rect);
            
            // Afficher le nom sous le personnage si le personnage n'est pas pris
            if (persos[i].pris == 0) {
                SDL_RenderCopy(renderer, persos[i].nom_texture, NULL, &persos[i].nom_rect);  // Afficher le nom sous le personnage
            }

            // Ajouter un effet de surbrillance sur le personnage sélectionné par le joueur
            if (selection_en_cours && index_selection == i) {
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 180); // Jaune avec transparence
                SDL_RenderFillRect(renderer, &persos[i].rect); // Dessiner le rectangle de surbrillance
            }
        }

        // Mettre à jour les positions des personnages dans les équipes (placer à droite du deuxième perso)
        if (equipe1_count > 1) {
            persos[equipe1[1]].rect.x = persos[equipe1[0]].rect.x + 160;  // Deuxième personnage à droite du premier
        }
        if (equipe1_count > 2) {
            persos[equipe1[2]].rect.x = persos[equipe1[1]].rect.x + 160;  // Troisième personnage à droite du deuxième
            persos[equipe1[2]].rect.y = persos[equipe1[0]].rect.y;  
        }

        if (equipe2_count > 1) {
            persos[equipe2[1]].rect.x = persos[equipe2[0]].rect.x - 160;  // Deuxième personnage à gauche du premier
        }
        if (equipe2_count > 2) {
            persos[equipe2[2]].rect.x = persos[equipe2[1]].rect.x - 160;  // Troisième personnage à gauche du deuxième
            persos[equipe2[2]].rect.y = persos[equipe2[0]].rect.y;  
        }

        SDL_RenderPresent(renderer);
    }

    // Libérer les ressources
    for (int i = 0; i < NB_PERSOS; i++) {
        SDL_DestroyTexture(persos[i].texture);
        SDL_DestroyTexture(persos[i].nom_texture);
    }

    SDL_DestroyTexture(fond);
    SDL_DestroyTexture(titre);
    SDL_DestroyTexture(btn_j1);
    SDL_DestroyTexture(btn_j2);
}









