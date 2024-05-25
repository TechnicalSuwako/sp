UNAME_S != uname -s
UNAME_M != uname -m

NAME != cat main.c | grep "const char \*sofname" | awk '{print $$5}' | \
	sed "s/\"//g" | sed "s/;//"
VERSION != cat main.c | grep "const char \*version" | awk '{print $$5}' | \
	sed "s/\"//g" | sed "s/;//"
PREFIX = /usr/local

MANPREFIX = ${PREFIX}/man

.if ${UNAME_S} == "FreeBSD"
MANPREFIX = ${PREFIX}/share/man
.elif ${UNAME_S} == "Linux"
PREFIX = /usr
MANPREFIX = ${PREFIX}/share/man
.elif ${UNAME_S} == "NetBSD"
PREFIX = /usr/pkg
MANPREFIX = ${PREFIX}/share/man
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
		${NAME}-completion.zsh ${NAME}.1 main.c src ${NAME}-${VERSION}
	tar zcfv dist/${NAME}-${VERSION}.tar.gz ${NAME}-${VERSION}
	rm -rf ${NAME}-${VERSION}

release-openbsd:
	mkdir -p release/bin
	${CC} ${CFLAGS} -o release/${NAME}-${VERSION}-openbsd-${UNAME_M} ${FILES} \
		-static -lgpgme -lcrypto -lc -lassuan -lgpg-error -lintl -liconv
	strip release/${NAME}-${VERSION}-openbsd-${UNAME_M}

release-freebsd:
	mkdir -p release/bin
	${CC} ${CFLAGS} -o release/${NAME}-${VERSION}-freebsd-${UNAME_M} ${FILES} \
		-static -lgpgme -lcrypto -lc -lassuan -lgpg-error -lthr -lintl
	strip release/${NAME}-${VERSION}-freebsd-${UNAME_M}

release-netbsd:
	mkdir -p release/bin
	${CC} ${CFLAGS} -o release/${NAME}-${VERSION}-netbsd-${UNAME_M} ${FILES} \
		-static -lgpgme -lcrypto -lcrypt -lc -lassuan -lgpg-error -lintl
	strip release/${NAME}-${VERSION}-netbsd-${UNAME_M}

release-linux:
	mkdir -p release/bin
	${CC} ${CFLAGS} -o release/${NAME}-${VERSION}-linux-${UNAME_M} ${FILES} \
		-static -lgpgme -lcrypto -lc -lassuan -lgpg-error
	strip release/${NAME}-${VERSION}-linux-${UNAME_M}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f ${NAME} ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/${NAME}
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	sed "s/VERSION/${VERSION}/g" < ${NAME}.1 > ${DESTDIR}${MANPREFIX}/man1/${NAME}.1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/${NAME}.1

install-zsh:
	cp sp-completion.zsh ${DESTDIR}${PREFIX}/share/zsh/site-functions/_sp

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/${NAME}

.PHONY: all clean dist install install-zsh uninstall
