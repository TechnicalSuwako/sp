UNAME_S != uname -s
UNAME_M != uname -m
OS = ${UNAME_S}

.if ${UNAME_S} == "OpenBSD"
OS = openbsd
.elif ${UNAME_S} == "NetBSD"
OS = netbsd
.elif ${UNAME_S} == "FreeBSD"
OS = freebsd
.elif ${UNAME_S} == "Linux"
OS = linux
.endif

.if ${UNAME_M} == "x86_64"
UNAME_M = amd64
.endif

NAME != cat main.c | grep "const char \*sofname" | awk '{print $$5}' | \
	sed "s/\"//g" | sed "s/;//"
VERSION != cat main.c | grep "const char \*version" | awk '{print $$5}' | \
	sed "s/\"//g" | sed "s/;//"
PREFIX = /usr/local
.if ${UNAME_S} == "Haiku"
PREFIX = /boot/home/config/non-packaged
.elif ${UNAME_S} == "Linux"
PREFIX = /usr
.endif

MANPREFIX = ${PREFIX}/share/man
.if ${UNAME_S} == "OpenBSD"
MANPREFIX = ${PREFIX}/man
.elif ${UNAME_S} == "Haiku"
MANPREFIX = ${PREFIX}/documentation/man
.endif

DATAPREFIX = ${PREFIX}/share
.if ${UNAME_S} == "Haiku"
DATAPREFIX = ${PREFIX}/data
.endif

CC = cc
FILES = main.c src/*.c
CFLAGS = -Wall -Wextra -O3 -I${PREFIX}/include -L${PREFIX}/lib
.if ${UNAME_S} == "NetBSD"
CFLAGS += -I/usr/pkg/include -L/usr/pkg/lib -I/usr/include -L/usr/lib
.endif
LDFLAGS = -lgpgme -lcrypto

SLDFLAGS = -static ${LDFLAGS}
.if ${UNAME_S} == "OpenBSD"
SLDFLAGS += -lc -lassuan -lgpg-error -lintl -liconv
.elif ${UNAME_S} == "FreeBSD"
SLDFLAGS += -lc -lassuan -lgpg-error -lthr -lintl
.elif ${UNAME_S} == "NetBSD"
SLDFLAGS += -lcrypt -lc -lassuan -lgpg-error -lintl
.elif ${UNAME_S}
SLDFLAGS += -lc -lassuan -lgpg-error
.endif

all:
	${CC} ${CFLAGS} -o ${NAME} ${FILES} ${LDFLAGS}
	strip ${NAME}

clean:
	rm -f ${NAME}

dist: clean
	mkdir -p release/src ${NAME}-${VERSION}
	cp -R LICENSE.txt Makefile README.md CHANGELOG.md \
		${NAME}-completion.zsh man main.c src ${NAME}-${VERSION}
	tar zcfv release/src/${NAME}-${VERSION}.tar.gz ${NAME}-${VERSION}
	rm -rf ${NAME}-${VERSION}

man:
	mkdir -p release/man/${VERSION}
	sed "s/VERSION/${VERSION}/g" < man/${NAME}-en.1 > \
		release/man/${VERSION}/${NAME}-en.1
	sed "s/VERSION/${VERSION}/g" < man/${NAME}-jp.1 > \
		release/man/${VERSION}/${NAME}-jp.1

release:
	mkdir -p release/bin/${VERSION}/${OS}/${UNAME_M}
	${CC} ${CFLAGS} -o release/bin/${VERSION}/${OS}/${UNAME_M}/${NAME} ${FILES} \
		${SLDFLAGS}
	strip release/bin/${VERSION}/${OS}/${UNAME_M}/${NAME}

install:
	mkdir -p ${DESTDIR}${PREFIX}/bin ${DESTDIR}${MANPREFIX}/man1
	cp -f ${NAME} ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/${NAME}
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	sed "s/VERSION/${VERSION}/g" < man/${NAME}-en.1 > \
		${DESTDIR}${MANPREFIX}/man1/${NAME}-en.1
	sed "s/VERSION/${VERSION}/g" < man/${NAME}-jp.1 > \
		${DESTDIR}${MANPREFIX}/man1/${NAME}-jp.1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/${NAME}-en.1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/${NAME}-jp.1

install-zsh:
	mkdir -p ${DESTDIR}${DATAPREFIX}/zsh/site-functions
	cp sp-completion.zsh ${DESTDIR}${DATAPREFIX}/zsh/site-functions/_sp

uninstall:
	rm -rf ${DESTDIR}${PREFIX}/bin/${NAME}
	rm -rf ${DESTDIR}${MANPREFIX}/man1/${NAME}-en.1
	rm -rf ${DESTDIR}${MANPREFIX}/man1/${NAME}-jp.1
	rm -rf ${DESTDIR}${DATAPREFIX}/zsh/site-functions/_sp

.PHONY: all clean dist man release install install-zsh uninstall
