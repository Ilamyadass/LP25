CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lncurses

# Liste de tous les fichiers sources
SRC = manager.c processus_ui.c process.c network.c
# Création automatique de la liste des objets (.o)
OBJ = $(SRC:.c=.o)

# Nom de l'exécutable final
TARGET = mytop

# Règle par défaut
all: check_deps $(TARGET)

# Vérification des dépendances (ncurses et sshpass)
check_deps:
	@which sshpass > /dev/null || echo "ATTENTION: 'sshpass' n'est pas installé. SSH avec mot de passe ne fonctionnera pas."

# Édition de liens
$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $(TARGET)

# Compilation des fichiers .c en .o
%.o: %.c processus_ui.h process.h network.h
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -f $(OBJ) $(TARGET)

# Règle pour lancer
run: $(TARGET)
	./$(TARGET)