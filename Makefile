UNAME_S := $(shell uname -s)

NAME := $(shell cat main.c | grep "const char\* sofname" | awk '{print $$5}' | sed "s/\"//g" | sed "s/;//" )
VERSION := $(shell cat main.c | grep "const char\* version" | awk '{print $$5}' | sed "s/\"//g" | sed "s/;//" )
PREFIX=/usr
ifeq ($(UNAME_S),FreeBSD)
	PREFIX=/usr/local
endif
ifeq ($(UNAME_S),OpenBSD)
	PREFIX=/usr/local
endif
ifeq ($(UNAME_S),NetBSD)
	PREFIX=/usr/pkg
endif
CC=cc
FILES=main.c showpass.c yankpass.c addpass.c delpass.c listpass.c genpass.c initpass.c otppass.c base32.c
CFLAGS=-Wall -Wextra -O3 -I${PREFIX}/include -L${PREFIX}/lib
LDFLAGS=-lgpgme -lcrypto

all:
	${CC} ${CFLAGS} -o ${NAME} ${FILES} ${LDFLAGS}
	strip ${NAME}

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
