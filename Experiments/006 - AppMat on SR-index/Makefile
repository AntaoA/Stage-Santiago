CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I"../005 -SR-index" -g

TARGET = main

LDLIBS = -ldivsufsort


# Tous les fichiers nécessaires
SRCS = main.c \
       ../005\ -\ SR-index/sr-index.c \
       ../005\ -\ SR-index/bwt.c \
       ../005\ -\ SR-index/lf.c \
       ../005\ -\ SR-index/sa.c \
       ../005\ -\ SR-index/lsa.c \
       ../005\ -\ SR-index/ft.c \
       ../005\ -\ SR-index/mapfl.c \
       ../005\ -\ SR-index/utils.c \
	../005\ -\ SR-index/c.c \
       count_approx.c \
       findToehold_approx.c \

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LDLIBS)

clean:
	rm -f $(TARGET) *.o ../005\ -\ SR-index/*.o