# CY-Combattants Groupe MI3-A

## Description

CY-Combattants est un jeu de combat au tour par tour développé en C avec SDL2. Les joueurs peuvent s'affronter en mode JvJ (Joueur vs Joueur) ou JvIA (Joueur vs Intelligence Artificielle).

## Prérequis

### Version graphique (SDL)
- GCC
- SDL2
- SDL2_image 
- SDL2_ttf
- SDL2_mixer

### Version terminal
- GCC
- Bibliothèques standard C
- pthread

## Installation des dépendances

### Sur Ubuntu/Debian :
```bash
# Pour la version graphique
sudo apt-get install gcc libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev

# Pour la version terminal
sudo apt-get install gcc build-essential
```

## Compilation

Le projet propose deux versions du jeu :

### Version graphique (SDL) (incomplète)
```bash
make clean    # Supprime les fichiers compilés existants (évite tout conflit)
make compile   # Compile uniquement
make jeu      # Compile et lance le jeu
```

### Version terminal (fonctionnelle)
```bash
make clean    # Supprime les fichiers compilés existants (évite tout conflit)
make eau      # Compile et lance la version terminal
```

## Comment jouer (Version Terminal)

1. Au lancement, choisissez votre mode de jeu :
   - 1 : Joueur vs Joueur
   - 2 : Joueur vs Ordinateur

2. Si vous choisissez le mode JvIA, sélectionnez la difficulté :
   - 0 : Facile
   - 1 : Moyen
   - 2 : Difficile

3. Créez votre équipe :
   - Donnez un nom à votre équipe
   - Sélectionnez 3 personnages parmi les 8 disponibles :
     * Musu
     * Freettle
     * Ronflex
     * Kirishima
     * Marco
     * Furina
     * Sakura
     * King K Rool

4. Durant le combat :
   - À votre tour, choisissez entre :
     * Attaque de base (option 1)
     * Techniques spéciales (options 2+)
   - Sélectionnez une cible valide
   - Observez les effets de vos actions

5. Le combat continue jusqu'à ce qu'une équipe soit victorieuse

## Nettoyage
```bash
make clean    # Supprime les fichiers compilés
```