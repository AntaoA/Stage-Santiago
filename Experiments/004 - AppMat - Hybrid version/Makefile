TARGET = main
CC = gcc
CFLAGS = -Wall -Wextra -O0 -g -std=c11

SRCS = main.c \
  ../001\ -\ AppMat\ using\ dynamic\ programming/DynP.c \
  ../002\ -\ AppMat\ using\ ST/suffix_trie.c \
  ../002\ -\ AppMat\ using\ ST/STforAppMat.c \
  ../003\ -\ AppMat\ using\ fragments/fragmentsAM.c \
  ../003\ -\ AppMat\ using\ fragments/rabin_karp.c \
  hybridAM.c

all: $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) -o $@ $(SRCS)

clean:
	rm -f $(TARGET)

.PHONY: all clean