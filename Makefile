S_HELPERS = genetics.cpp
O_HELPERS = $(S_HELPERS:.cpp=.o)
HEADER = evolve.h

S_MAIN = image_evolve.cpp

CC = g++
CFLAGS = -g -O1
MAGICKFLAGS = `Magick++-config --cppflags --cxxflags --ldflags --libs`

image_evolve:	${S_MAIN} ${O_HELPERS}
		${CC} ${CFLAGS} ${O_HELPERS} ${S_MAIN} -o $@ ${MAGICKFLAGS}

genetics.o:	${S_HELPERS} ${HEADER}
		${CC} ${CFLAGS} -c ${S_HELPERS} ${MAGICKFLAGS}

clean:
		rm *.o
