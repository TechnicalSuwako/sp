# sp
シンプルなパスワードマネージャー

## インストールする方法
### CRUX
```sh
doas prt-get depinst gpgme gnupg pinentry
```

### Artix
```sh
doas pacman -S base-devel gpgme gnupg pinentry
```

### OpenBSD
```sh
doas pkg_add gmake gpgme gnupg pinentry
```

### FreeBSD
```sh
doas pkg install gmake gpgme gnupg pinentry
```

### 全部
```sh
mkdir -p ~/.local/share/sp
gpg --generate-key
gpg -k | less
```
鍵をコピーして下さい。
```sh
echo "（コピーした鍵）" >> ~/.local/share/sp/.gpg-id
```

### Linux
```sh
make
doas make install
```

### FreeBSDかOpenBSD
```sh
gmake
doas gmake install PREFIX=/usr/local
```
