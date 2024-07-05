UNAME_S != uname -s
UNAME_M != uname -m

.if ${UNAME_M} == "x86_64"
UNAME_M = amd64
.endif

NAME != cat main.c | grep "const char \*sofname" | awk '{print $$5}' | \
	sed "s/\"//g" | sed "s/;//"
VERSION != cat main.c | grep "const char \*version" | awk '{print $$5}' | \
	sed "s/\"//g" | sed "s/;//"
PREFIX = /usr/local
.if ${UNAME_S} == "Haiku"
PREFIX = /boot/system
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
CFLAGS += -I/usr/local/include -L/usr/local/lib -I/usr/include -L/usr/lib
.endif
LDFLAGS = -lgpgme -lcrypto

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

release-openbsd:
	mkdir -p release/bin
	${CC} ${CFLAGS} -o release/bin/${NAME}-${VERSION}-openbsd-${UNAME_M} ${FILES} \
		-static -lgpgme -lcrypto -lc -lassuan -lgpg-error -lintl -liconv
	strip release/bin/${NAME}-${VERSION}-openbsd-${UNAME_M}

release-freebsd:
	mkdir -p release/bin
	${CC} ${CFLAGS} -o release/bin/${NAME}-${VERSION}-freebsd-${UNAME_M} ${FILES} \
		-static -lgpgme -lcrypto -lc -lassuan -lgpg-error -lthr -lintl
	strip release/bin/${NAME}-${VERSION}-freebsd-${UNAME_M}

release-netbsd:
	mkdir -p release/bin
	${CC} ${CFLAGS} -o release/bin/${NAME}-${VERSION}-netbsd-${UNAME_M} ${FILES} \
		-static -lgpgme -lcrypto -lcrypt -lc -lassuan -lgpg-error -lintl
	strip release/bin/${NAME}-${VERSION}-netbsd-${UNAME_M}

release-linux:
	mkdir -p release/bin
	${CC} ${CFLAGS} -o release/bin/${NAME}-${VERSION}-linux-${UNAME_M} ${FILES} \
		-static -lgpgme -lcrypto -lc -lassuan -lgpg-error
	strip release/bin/${NAME}-${VERSION}-linux-${UNAME_M}

install:
	mkdir -p ${DESTDIR}${PREFIX}/bin
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
	cp sp-completion.zsh ${DESTDIR}${DATAPREFIX}/zsh/site-functions/_sp

uninstall:
	rm -rf ${DESTDIR}${PREFIX}/bin/${NAME}
	rm -rf ${DESTDIR}${MANPREFIX}/man1/${NAME}-en.1
	rm -rf ${DESTDIR}${MANPREFIX}/man1/${NAME}-jp.1
	rm -rf ${DESTDIR}${DATAPREFIX}/zsh/site-functions/_sp

.PHONY: all clean dist install install-zsh uninstall
