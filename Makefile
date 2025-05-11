CC       = gcc
CFLAGS   = -Wall -Iinclude `sdl2-config --cflags`
LIBS     = `sdl2-config --libs` -lSDL2_image -lSDL2_ttf -lSDL2_mixer

SRC      = src/menu.c src/selection.c src/arene.c src/interface_terminal.c src/gest_combat.c src/gest_effets.c src/aff_combat.c src/util_combat.c src/gestioncombattant.c
EXEC     = bin/jeu
EAU_EXEC = bin/eau
EAU_SRC  = src/interface_terminal.c src/gestioncombattant.c src/util_combat.c src/aff_combat.c src/gest_combat.c src/gest_effets.c logs_combat.c
EAU_LIBS = -lm 

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

$(EAU_EXEC) : $(EAU_SRC) $(EAU_HEADERS)
	@echo "💦​ C'est de l'eau (mais elle est fraiche)."
	@mkdir -p bin
	@$(CC) $(CFLAGS) -o $@ $(EAU_SRC) $(EAU_LIBS)
	@echo "✅ Compilation réussie : $(EAU_EXEC)"
	@echo "🎮 Lancement du jeu..."
	@./$(EAU_EXEC)

clean:
	@echo "🧹 Nettoyage..."
	@rm -f $(EXEC)
	@rm -f $(EAU_EXEC)
