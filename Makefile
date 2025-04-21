# Nom de l'exécutable
EXEC = cyfighter

# Liste des fichiers source
SRC = $(wildcard *.c)

# Génère les fichiers objets (.o) à partir des fichiers source
OBJ = $(SRC:.c=.o)

# Options du compilateur
CC = gcc
CFLAGS = -Wall -Wextra -g

# Règle principale
all: $(EXEC)

# Compilation de l'exécutable
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Nettoyer les fichiers compilés
clean:
	rm -f *.o $(EXEC)

# Forcer la recompilation complète
rebuild: clean all

# Éviter que des fichiers comme "clean" soient interprétés comme fichiers
.PHONY: all clean rebuild
