#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <gpgme.h>

void initpass(char* gpgid);
#include "showpass.h"
#include "yankpass.h"
void listpass(char* dir);
#include "addpass.h"
void delpass(char* file);
void genpass(char* file, int count, bool issecure);
void otppass(char* file);
void helpme();

const char* sofname = "sp";
const char* version = "0.0.1";

void helpme() {
  printf("使い方：\n");
  //printf("%s -i <gpg-id>                              ：GPGと使ってパスワードストレージを初期設定\n", sofname);
  printf("%s -s <パスワード名>                        ：パスワードを表示\n", sofname);
  printf("%s -y <パスワード名>                        ：パスワードを表示せずクリップボードにコピーする\n", sofname);
  //printf("%s -l                                       ：パスワード一覧を表示\n", sofname);
  printf("%s -a <パスワード名>                        ：パスワードを追加\n", sofname);
  //printf("%s -d <パスワード名>                        ：パスワードを削除\n", sofname);
  //printf("%s -g <文字数> [risk|secure] <パスワード名> ：希望文字数でパスワードをランダムに作成して、追加する。risk＝英数字のみ（不安）、secure＝英数字＋特別文字（デフォルト）を使用\n", sofname);
  //printf("%s -o <パスワード名>\n                      ：ワンタイムパスワード（TOTP）を表示。存在しなければ、創作する", sofname);
  printf("%s -h                                       ：ヘルプを表示\n", sofname);
  printf("%s -v                                       ：バージョンを表示\n", sofname);
}

int main (int argc, char* argv[]) {
  if (argc < 2) {
    helpme();
    return 1;
  }

  if (argc == 3 && strcmp(argv[1], "-i") == 0) printf("TODO: 初期設定\n");
  else if (argc == 3 && strcmp(argv[1], "-s") == 0) showpass(argv[2]);
  else if (argc == 3 && strcmp(argv[1], "-y") == 0) yankpass(argv[2]);
  else if (argc == 2 && strcmp(argv[1], "-l") == 0) printf("TODO: 一覧\n");
  else if (argc == 3 && strcmp(argv[1], "-a") == 0) addpass(argv[2]);
  else if (argc == 3 && strcmp(argv[1], "-d") == 0) printf("TODO: 削除\n");
  else if ((argc == 4 || argc == 5) && strcmp(argv[1], "-g") == 0) printf("TODO: パスワードを創作\n");
  else if (argc == 3 && strcmp(argv[1], "-o") == 0) printf("TODO: otp\n");
  else if (argc == 2 && strcmp(argv[1], "-v") == 0) printf("%s-%s\n", sofname, version);
  else helpme();

  return 0;
}
