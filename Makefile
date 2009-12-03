S_HELPERS = genetics.cpp
O_HELPERS = $(S_HELPERS:.cpp=.o)
HEADER = evolve.h

S_MAIN = image_evolve.cpp

CC = g++
CFLAGS = -g -O3 -lpthread -lspe2
MAGICKFLAGS = `Magick++-config --cppflags --cxxflags --ldflags --libs`

SPU_CC = spu-g++
SPU_PROGRAM = spu_crossover

image_evolve:	${S_MAIN} ${O_HELPERS}
		${CC} ${CFLAGS} ${O_HELPERS} ${S_MAIN} ${SPU_PROGRAM}_csf.o -o $@ ${MAGICKFLAGS}

genetics.o:	${S_HELPERS} ${HEADER} ${SPU_PROGRAM}_csf.o
		${CC} ${CFLAGS} -c ${S_HELPERS} ${SPU_PROGRAM}_csf.o ${MAGICKFLAGS}

spu_crossover_csf.o:	${SPU_PROGRAM}
			ppu-embedspu -m32 ${SPU_PROGRAM}_handle ${SPU_PROGRAM} $@

spu_crossover: 	${SPU_PROGRAM}.cpp
		${SPU_CC} ${SPU_PROGRAM}.cpp -o $@

clean:
		rm *.o ${SPU_PROGRAM}
