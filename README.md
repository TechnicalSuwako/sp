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

## 使い方
### パスワードの作成
#### 強いパスワードの場合
```sh
$ sp -g 64
nSYGSF2lWGCUsqKCRB_~mZm+spaU<zvtt%um'01$tj4h,^nB6JqX#Cm$!U+s;c7:
```
又は
```sh
$ sp -g 64 secure
nSYGSF2lWGCUsqKCRB_~mZm+spaU<zvtt%um'01$tj4h,^nB6JqX#Cm$!U+s;c7:
```

#### 弱いパスワードの場合
```sh
$ sp -g 12 risk
2aKBwb858mzg
```

### パスワードの追加
「suwako」というユーザー名の場合：
```sh
$ sp -a 076.moe/suwako
パスワード： 
パスワード（確認用）： 
パスワードを保存出来ました。
```
入力中は何も表示されないので、ご注意下さい。

### パスワードの表示
```sh
$ sp -s 076.moe/suwako
nSYGSF2lWGCUsqKCRB_~mZm+spaU<zvtt%um'01$tj4h,^nB6JqX#Cm$!U+s;c7:
```
表示せずコピーする場合は、「sp -y 076.moe/suwako」を使用して下さい。

### パスワードの削除
```sh
$ sp -d 076.moe/suwako
パスワード「076.moe/suwako」を本当に削除する事が宜しいでしょうか？ (y/N): y
パスワードを削除しました。
```

## TOTP（ワンタイムパスワード）
### QRコードから
QRコードをダウンロードし、zbarimgを使用して「QR-Code:」以降の部分をコピーし、spに追加して下さい。\
`sp -a`を実行すると、「パスワード」を聞かれますが、TOTPの場合は「otpauth://」から始まる文字列をコピペして下さい。
```sh
$ zbarimg -q gitler.png
QR-Code:otpauth://totp/Gitler%20%28gitler.moe%29:suwako?algorithm=SHA1&digits=6&issuer=Gitler%20%28gitler.moe%29&period=30&secret=〇〇

$ sp -a gitler-otp
パスワード： 
パスワード（確認用）： 
パスワードを保存出来ました。

$ sp -q gitler-otp
123456
```
