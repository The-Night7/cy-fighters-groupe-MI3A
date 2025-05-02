CC       = gcc
CFLAGS   = -Wall -Iinclude `sdl2-config --cflags`
LIBS     = `sdl2-config --libs` -lSDL2_image -lSDL2_ttf -lSDL2_mixer

SRC      = src/menu.c
EXEC     = bin/jeu

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

clean:
	@echo "🧹 Nettoyage..."
	@rm -f $(EXEC)
