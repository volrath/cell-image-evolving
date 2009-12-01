S_HELPERS = genetics.cpp
O_HELPERS = $(S_HELPERS:.cpp=.o)
HEADER = evolve.h

S_MAIN = image_evolve.cpp

CC = g++
CFLAGS = -g -O3 -lpthread -lspe2
MAGICKFLAGS = `Magick++-config --cppflags --cxxflags --ldflags --libs`

image_evolve:	${S_MAIN} ${O_HELPERS}
		${CC} ${CFLAGS} ${O_HELPERS} ${S_MAIN} spu_crossover_csf.o -o $@ ${MAGICKFLAGS}

genetics.o:	${S_HELPERS} ${HEADER} spu_crossover_csf.o
		${CC} ${CFLAGS} -c ${S_HELPERS} spu_crossover_csf.o ${MAGICKFLAGS}

spu_crossover_csf.o:	spu_crossover
			ppu-embedspu -m32 spu_crossover_handle spu_crossover spu_crossover_csf.o

spu_crossover: 	spu_crossover.cpp
		spu-g++ spu_crossover.cpp -o spu_crossover

clean:
		rm *.o spu_crossover
