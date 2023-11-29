# sp
シンプルなパスワードマネージャー

## インストールする方法
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
