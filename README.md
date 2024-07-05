# sp
シンプルなパスワードマネージャー
Simple Password Manager

## インストールする方法 | Installation
### CRUX
```sh
doas prt-get depinst gpgme gnupg pinentry bmake
bmake
doas bmake install PREFIX=/usr
doas bmake install-zsh PREFIX=/usr
```

### Artix
```sh
doas pacman -S base-devel gpgme gnupg pinentry bmake
bmake
doas bmake install PREFIX=/usr
doas bmake install-zsh PREFIX=/usr
```

### OpenBSD
```sh
doas pkg_add gpgme gnupg pinentry
make
doas make install
doas make install-zsh
```

### NetBSD
```sh
doas pkgin install gpgme gnupg pinentry
make
doas make install
doas make install-zsh
```

### FreeBSD
```sh
doas pkg install gpgme gnupg pinentry
make
doas make install
doas make install-zsh
```

### Haiku
```sh
pkgman install bmake libassuan libassuan_devel gpgme gpgme_devel gnupg llvm18_clang
bmake
bmake install
bmake install-zsh
```

### macOS
```sh
brew install bmake libassuan gpgme gnupg pinentry pinentry-mac
bmake
doas bmake install
doas bmake install-zsh
```

## 初期設定 | Initial setup
「gpg -k」でGPG鍵IDを確認して、「sp -i [GPG ID]」を実行して下さい。
Confirm your GPG key ID with "gpg -k", and run "sp -i [GPG ID]".

## 使い方 | Usage
### パスワードの作成 | Password generation
#### 強いパスワードの場合 | Strong password
```sh
$ sp -g 64
nSYGSF2lWGCUsqKCRB_~mZm+spaU<zvtt%um'01$tj4h,^nB6JqX#Cm$!U+s;c7:
```
又は
Or
```sh
$ sp -g 64 secure
nSYGSF2lWGCUsqKCRB_~mZm+spaU<zvtt%um'01$tj4h,^nB6JqX#Cm$!U+s;c7:
```

#### 弱いパスワードの場合 | Weak password
```sh
$ sp -g 12 risk
2aKBwb858mzg
```

### パスワードの追加 | Add password
「suwako」というユーザー名の場合：
If the username is "suwake":
```sh
$ sp -a 076.moe/suwako
パスワード： 
パスワード（確認用）： 
パスワードを保存出来ました。
```
入力中は何も表示されないので、ご注意下さい。

### 英語で表示する場合 | For English
```sh
$ SP_LANG=en sp -a 076.moe/suwako
Password: 
Password (for verification): 
The password got saved.
```

### パスワードの表示 | Show password
```sh
$ sp -s 076.moe/suwako
nSYGSF2lWGCUsqKCRB_~mZm+spaU<zvtt%um'01$tj4h,^nB6JqX#Cm$!U+s;c7:
```
表示せずコピーする場合は、「sp -y 076.moe/suwako」を使用して下さい。
If you want to copy the password to the clipboard without showing it,
use "sp -y 076.moe/suwako" instead.

### パスワードの削除 | Delete password
```sh
$ sp -d 076.moe/suwako
パスワード「076.moe/suwako」を本当に削除する事が宜しいでしょうか？ (y/N): y
パスワードを削除しました。
```

### パスワードの編集 | Edit password
```sh
$ sp -e 076.moe/suwako
パスワード： 
パスワード（確認用）： 
パスワードを保存出来ました。
```

## TOTP（ワンタイムパスワード） | TOTP (one time password)
### QRコードから | From QR code
QRコードをダウンロードし、zbarimgを使用して「QR-Code:」以降の部分をコピーし、
spに追加して下さい。\
`sp -a`を実行すると、「パスワード」を聞かれますが、
TOTPの場合は「otpauth://」から始まる文字列をコピペして下さい。

Download the QR code image, copy everything after "QR-Code:"
you get after using zbarimg, and add into sp.
If you run "sp -a", it'll ask for a "password", but in the case of TOTP,
paste everything starting from "otpauth://".
```sh
$ zbarimg -q gitler.png
QR-Code:otpauth://totp/Gitler%20%28gitler.moe%29:suwako?algorithm=SHA1&digits=6&issuer=Gitler%20%28gitler.moe%29&period=30&secret=〇〇

$ sp -a gitler-otp
パスワード： 
パスワード（確認用）： 
パスワードを保存出来ました。

$ sp -o gitler-otp
123456
```
