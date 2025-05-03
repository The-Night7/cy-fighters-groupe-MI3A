CC       = gcc
CFLAGS   = -Wall -Iinclude `sdl2-config --cflags`
LIBS     = `sdl2-config --libs` -lSDL2_image -lSDL2_ttf -lSDL2_mixer

SRC      = src/menu.c
EXEC     = bin/jeu
EAU_EXEC = bin/eau
EAU_SRC  = interface_terminal.c combat.c gestioncombattant.c
EAU_LIBS = -lm  # Ajout de la bibliothèque mathématique

.PHONY: all compile jeu clean

all: $(EXEC)

$(EXEC): $(SRC)
	@mkdir -p bin
	@$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	@echo "✅ Compilation réussie : $(EXEC)"

compile: all

jeu: all
	@echo "🎮 Lancement du jeu..."
	@./$(EXEC)

eau : $(EAU_EXEC)

$(EAU_EXEC) : $(EAU_SRC)
	@echo "💦​ C'est de l'eau (mais elle est fraiche)."
	@mkdir -p bin
	@$(CC) $(CFLAGS) -o $@ $^ $(EAU_LIBS)  # Utilisation de EAU_LIBS au lieu de LIBS
	@echo "✅ Compilation réussie : $(EAU_EXEC)"
	@echo "🎮 Lancement du jeu..."
	@./$(EAU_EXEC)

clean:
	@echo "🧹 Nettoyage..."
	@rm -f $(EXEC)
	@rm -f $(EAU_EXEC)