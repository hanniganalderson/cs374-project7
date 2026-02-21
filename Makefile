CC=gcc
CFLAGS=-Wall -Wextra -Werror

grayscaler: grayscaler.c ppm.c ppm.h
	$(CC) $(CFLAGS) -o grayscaler grayscaler.c ppm.c -lpthread

.PHONY: clean

clean:
	rm -f grayscaler
