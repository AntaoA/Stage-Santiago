# Nom de l'exécutable
TARGET = main

# Compilateur et options
CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11

# Fichiers sources
SRCS = main.c DynP.c

# Règle par défaut
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

.PHONY: all clean