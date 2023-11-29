#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "delpass.h"

void delpass(char* file) {
  // パスを準備
  char pwfile[512];
  char* homedir = getenv("HOME");
  if (homedir == NULL) {
    perror("ホームディレクトリを受取に失敗。");
    return;
  }

  char* basedir = "/.local/share/sp/";
  char* ext = ".gpg";
  snprintf(pwfile, sizeof(pwfile), "%s%s%s%s", homedir, basedir, file, ext);

  if (unlink(pwfile) == -1) {
    perror("パスワードを削除出来ませんですた。");
  }

  printf("パスワードを削除しました。\n");
  return;
}
