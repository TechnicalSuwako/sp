#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <gpgme.h>

#include "initpass.h"
#include "showpass.h"
#include "yankpass.h"
#include "listpass.h"
#include "addpass.h"
#include "delpass.h"
#include "genpass.h"
#include "otppass.h"
void helpme();

const char* sofname = "sp";
const char* version = "1.2.0";

void helpme() {
  printf("０７６ %s %s - シンプルなパスワードマネージャー\n", sofname, version);
  printf("https://076.moe/ | https://gitler.moe/suwako/%s\n\n", sofname);
  puts  ("使い方：\n");
  printf("%s -i <gpg-id>               ：GPGと使ってパスワードストレージを初期設定\n", sofname);
  printf("%s -s <パスワード名>         ：パスワードを表示\n", sofname);
  printf("%s -y <パスワード名>         ：パスワードを表示せずクリップボードにコピーする\n", sofname);
  printf("%s -l                        ：パスワード一覧を表示\n", sofname);
  printf("%s -a <パスワード名>         ：パスワードを追加\n", sofname);
  printf("%s -d <パスワード名>         ：パスワードを削除\n", sofname);
  printf("%s -e <パスワード名>         ：パスワードを変更\n", sofname);
  printf("%s -g <文字数> [risk|secure] ：希望文字数でパスワードをランダムに作成する。risk＝英数字のみ（不安）、secure＝英数字＋特別文字（デフォルト）を使用\n", sofname);
  printf("%s -o <パスワード名>\n       ：ワンタイムパスワード（TOTP）を表示。存在しなければ、創作する\n", sofname);
  printf("%s -h                        ：ヘルプを表示\n", sofname);
  printf("%s -v                        ：バージョンを表示\n", sofname);
}

int main (int argc, char* argv[]) {
  if (argc < 2) {
    helpme();
    return 0;
  }

  if (argc == 3 && strcmp(argv[1], "-i") == 0) initpass(argv[2]);
  else if (argc == 3 && strcmp(argv[1], "-s") == 0) showpass(argv[2]);
  else if (argc == 3 && strcmp(argv[1], "-y") == 0) yankpass(argv[2]);
  else if (argc == 2 && strcmp(argv[1], "-l") == 0) {
    char basePath[512];
    char* homedir = getenv("HOME");
    if (homedir == NULL) {
      perror("ホームディレクトリを受取に失敗。");
      return -1;
    }

    char* basedir = "/.local/share/sp/";
    snprintf(basePath, sizeof(basePath), "%s%s", homedir, basedir);

    listpass(basePath, 0);
  }
  else if (argc == 3 && strcmp(argv[1], "-a") == 0) addpass(argv[2]);
  else if (argc == 3 && strcmp(argv[1], "-d") == 0) delpass(argv[2], 0);
  else if (argc == 3 && strcmp(argv[1], "-e") == 0) {
    delpass(argv[2], 1);
    addpass(argv[2]);
  }
  else if (strcmp(argv[1], "-g") == 0) {
    if (argc == 3) genpass(atoi(argv[2]), true);
    else if (argc == 4 && strcmp(argv[3], "risk") == 0) genpass(atoi(argv[2]), false);
    else if (argc == 4 && strcmp(argv[3], "secure") == 0) genpass(atoi(argv[2]), true);
    else helpme();
  }
  else if (argc == 3 && strcmp(argv[1], "-o") == 0) {
    char fullPath[512];
    char* homedir = getenv("HOME");
    if (homedir == NULL) {
      perror("ホームディレクトリを受取に失敗。");
      return -1;
    }

    char* basedir = "/.local/share/sp/";
    snprintf(fullPath, sizeof(fullPath), "%s%s%s.gpg", homedir, basedir, argv[2]);

    otppass(fullPath);
  }
  else if (argc == 2 && strcmp(argv[1], "-v") == 0) printf("%s-%s\n", sofname, version);
  else helpme();

  return 0;
}
