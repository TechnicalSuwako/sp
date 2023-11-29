#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "delpass.h"

int delpass(char* file) {
  // パスを準備
  char pwfile[512];
  char* homedir = getenv("HOME");
  if (homedir == NULL) {
    perror("ホームディレクトリを受取に失敗。");
    return -1;
  }

  char* basedir = "/.local/share/sp/";
  char* ext = ".gpg";
  int needed = snprintf(pwfile, sizeof(pwfile), "%s%s%s%s", homedir, basedir, file, ext);
  if (needed >= (int)sizeof(pwfile)) {
    fprintf(stderr, "エラー：パスが長すぎる。\n");
    return -1;
  }

  // 削除を確認する
  printf("パスワード「%s」を本当に削除する事が宜しいでしょうか？ (y/N): ", file);
  int confirm = getchar();
  if (confirm != 'y' && confirm != 'Y') {
    printf("削除しませんでした。\n");
    return -1;
  }

  int ch;
  while ((ch = getchar()) != '\n' && ch != EOF);

  if (unlink(pwfile) == -1) {
    perror("パスワードを削除出来ませんですた。");
    return -1;
  }

  printf("パスワードを削除しました。\n");
  return -1;
}
