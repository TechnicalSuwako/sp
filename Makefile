NAME=sp
VERSION=1.0.0
# Linux、Haiku、かIllumos = /usr、FreeBSDかOpenBSD = /usr/local、NetBSD = /usr/pkg
PREFIX=/usr
CC=cc
FILES=main.c showpass.c yankpass.c addpass.c delpass.c listpass.c genpass.c initpass.c
CFLAGS=-Wall -Wextra -g
LDFLAGS=-lgpgme

all:
	${CC} ${CFLAGS} -o ${NAME} ${FILES} ${LDFLAGS}

clean:
	rm -f ${NAME}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f ${NAME} ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/${NAME}

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/${NAME}

.PHONY: all clean install uninstall
