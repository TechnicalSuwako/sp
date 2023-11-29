# sp
シンプルなパスワードマネージャー

## インストールする方法
### CRUX
```sh
doas prt-get depinst gpgme gnupg pinentry
doas make install
```

又は

```sh
doas su
cd /etc/ports
wget https://076.moe/repo/crux/suwaports.httpup
echo "prtdir /usr/ports/suwaports" >> /etc/prt-get.conf
ports -u
prt-get depinst sp
```

### Artix
```sh
doas pacman -S base-devel gpgme gnupg pinentry
doas make install
```

### OpenBSD
```sh
doas pkg_add gmake gpgme gnupg pinentry
doas gmake install PREFIX=/usr/local
```

### FreeBSD
```sh
doas pkg install gmake gpgme gnupg pinentry
doas gmake install PREFIX=/usr/local
```

## 初期設定
「gpg -k」でGPG鍵IDを確認して、「sp -i [GPG ID]」を実行して下さい。
