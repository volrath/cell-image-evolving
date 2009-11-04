S_HELPERS = genetics.cpp
O_HELPERS = $(S_HELPERS:.cpp=.o)
HEADER = evolve.h

S_MAIN = image_evolve.cpp

CC = g++
CFLAGS = -g -o
MAGICKFLAGS = `Magick++-config --cppflags --cxxflags --ldflags --libs`


image_evolve:	${S_MAIN} ${O_HELPERS}
		${CC} ${CFLAGS} $@ ${O_HELPERS} ${S_MAIN} ${MAGICKFLAGS}

helpers:	${S_HELPERS}
		${CC} ${CFLAGS} -c  ${MAGICKFLAGS} ${S_HELPERS}

clean:
		rm *.o