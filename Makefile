CC = gcc
CFLAGS = -Wall -Wextra
CFLAGS += -Os

battery: main.o
	$(LINK.o) -o $@ $^
