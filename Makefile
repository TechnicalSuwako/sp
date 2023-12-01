NAME=sp
VERSION=1.1.1
# Linux、Haiku、かIllumos = /usr、FreeBSDかOpenBSD = /usr/local、NetBSD = /usr/pkg
PREFIX=/usr
CC=cc
FILES=main.c showpass.c yankpass.c addpass.c delpass.c listpass.c genpass.c initpass.c otppass.c
CFLAGS=-Wall -Wextra -g
LDFLAGS=-lgpgme -lcrypto

all:
	${CC} ${CFLAGS} -o ${NAME} ${FILES} ${LDFLAGS}

clean:
	rm -f ${NAME}

dist: clean
	mkdir -p ${NAME}-${VERSION}
	cp -R LICENSE.txt Makefile README.md CHANGELOG.md\
		sp-completion.zsh\
		*.c *.h ${NAME}-${VERSION}
	tar zcfv ${NAME}-${VERSION}.tar.gz ${NAME}-${VERSION}
	rm -rf ${NAME}-${VERSION}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f ${NAME} ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/${NAME}

install-zsh:
	cp sp-completion.zsh ${DESTDIR}${PREFIX}/share/zsh/site-functions/_sp

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/${NAME}

.PHONY: all clean dist install install-zsh uninstall
