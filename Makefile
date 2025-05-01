# Nom de l'exécutable
EXEC = cyfighter

# Tous les fichiers source .c du dossier courant
SRC = $(wildcard *.c)

# Fichiers objets correspondants
OBJ = $(SRC:.c=.o)

# Compilateur
CC = gcc

# Flags de compilation (inclusion SDL2)
CFLAGS = -Wall -Wextra -g -I/usr/include/SDL2 -D_REENTRANT

# Flags de liaison (linker)
LDFLAGS = -lSDL2 

# Règle principale
all: $(EXEC)

# Règle de création de l'exécutable
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Nettoyage
clean:
	rm -f *.o $(EXEC)

# Recompilation complète
rebuild: clean all

.PHONY: all clean rebuild
