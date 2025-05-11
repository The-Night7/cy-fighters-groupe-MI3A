CC = gcc
CFLAGS = -Wall -Wextra -Iinclude `sdl2-config --cflags`
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lm

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Main game sources and objects
GAME_SRCS = $(SRC_DIR)/menu.c $(SRC_DIR)/selection.c $(SRC_DIR)/arene.c \
            $(SRC_DIR)/interface_terminal.c $(SRC_DIR)/gest_combat.c \
            $(SRC_DIR)/gest_effets.c $(SRC_DIR)/aff_combat.c \
            $(SRC_DIR)/util_combat.c $(SRC_DIR)/gestioncombattant.c
GAME_OBJS = $(GAME_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
GAME_EXEC = $(BIN_DIR)/jeu

# Eau version sources and objects  
EAU_SRCS = $(SRC_DIR)/interface_terminal.c $(SRC_DIR)/gestioncombattant.c \
           $(SRC_DIR)/util_combat.c $(SRC_DIR)/aff_combat.c \
           $(SRC_DIR)/gest_combat.c $(SRC_DIR)/gest_effets.c
EAU_OBJS = $(EAU_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
EAU_EXEC = $(BIN_DIR)/eau

.PHONY: all clean jeu eau

all: $(GAME_EXEC)

$(GAME_EXEC): $(GAME_OBJS)
	@mkdir -p $(BIN_DIR)
	@$(CC) $(GAME_OBJS) -o $@ $(LDFLAGS)
	@echo "✅ Compilation réussie : $(GAME_EXEC)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

jeu: $(GAME_EXEC)
	@echo "🎮 Lancement du jeu..."
	@./$(GAME_EXEC)

eau: $(EAU_EXEC)
	@echo "💦​ C'est de l'eau (mais elle est fraiche)."
	@echo "🎮 Lancement du jeu..."
	@./$(EAU_EXEC)

$(EAU_EXEC): $(EAU_OBJS)
	@mkdir -p $(BIN_DIR)
	@$(CC) $(EAU_OBJS) -o $@ $(LDFLAGS)
	@echo "✅ Compilation réussie : $(EAU_EXEC)"
clean:
	@echo "🧹 Nettoyage..."
	@rm -rf $(OBJ_DIR) $(BIN_DIR)
