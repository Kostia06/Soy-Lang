CC = g++
CFLAGS = -c -std=c++23 -g 
LDFLAGS = -g `sdl2-config --cflags --libs` `pkg-config sdl2_image --cflags --libs` `pkg-config sdl2_ttf --cflags --libs`

SRC = ${wildcard src/* .cpp}
HDR = ${wildcard include/* .hpp}
OBJ = ${SRC:.c=.o}
EXEC = lang

all: ${SRC} ${OBJ} ${EXEC} 

${EXEC}: ${OBJ}
	${CC} ${LDFLAGS} $^ -o $@

%.o: %.c ${HDR}
	${CC} ${CFLAGS} $< -o $@


delete:
	del /f /usr/local/bin/soy-lang
install:
	make
	cp ./lang /usr/local/bin/soy-lang

clear:
	rm src/*.o ${EXEC}

