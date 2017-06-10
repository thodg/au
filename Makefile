
CFLAGS = -W -Wall -O0 -ggdb

all:

AU = au
AU_H = au.h typed.h
AU_O = au.o typed.o
AU_LIBS = -lportaudio -lm
${AU_O}: ${AU_H}
${AU}: ${AU_O}
	${CC} ${CFLAGS} ${LDFLAGS} ${AU_O} ${AU_LIBS} -o ${AU}
CLEANFILES += ${AU} ${AU_O}
all: ${AU}

clean:
	rm -f ${CLEANFILES}

.PHONY: all clean
