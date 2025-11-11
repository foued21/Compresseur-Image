# Variables
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -fPIC
LDFLAGS = -shared -lm 
SRCS = src/pgm_to_pixmap.c src/pixmap_to_quadtree.c src/quadtree_to_pixmap.c \
       src/bitstream.c src/codec.c src/filtrage.c src/pixmap_to_pgm.c \
       src/segmentation.c

OBJS = $(SRCS:src/%.c=obj/%.o)
LIB = libqtc.so

all: $(LIB)
$(LIB): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)
	rm -f $(OBJS)

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf obj/*.o $(LIB)
