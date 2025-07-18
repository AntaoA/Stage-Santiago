# Nom de l'exécutable
TARGET = main

# Fichiers sources
SRC = main.c sa.c bwt.c lsa.c ft.c mapfl.c c.c phi.c utils.c lf.c sr-index.c findToehold.c count.c

# Bibliothèques à linker
LDFLAGS = -ldivsufsort

# Compilation générale (valeurs par défaut)
CC = gcc

# === Règles ===

# Par défaut : debug
all: debug

# Compilation en mode debug (pour GDB)
debug: CFLAGS = -g -O0 -Wall
debug: $(TARGET)

# Compilation en mode release (optimisé)
release: CFLAGS = -O2 -Wall
release: $(TARGET)

# Règle générique de compilation
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Nettoyage
clean:
	rm -f $(TARGET)
