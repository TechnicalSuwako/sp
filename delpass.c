#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "delpass.h"

int delpass(char* file, int force) {
  // パスを準備
  char pwfile[512];
  char* homedir = getenv("HOME");
  if (homedir == NULL) {
    perror("ホームディレクトリを受取に失敗。");
    return -1;
  }

  char* basedir = "/.local/share/sp/";
  char* ext = ".gpg";

  int alllen = snprintf(NULL, 0, "%s%s%s%s", homedir, basedir, file, ext) + 1;
  char* gpgpathchk = malloc(alllen);
  if (gpgpathchk == NULL) {
    perror("メモリを割当に失敗。");
    return -1;
  }

  // ファイルが既に存在するかどうか確認
  snprintf(gpgpathchk, alllen, "%s%s%s%s", homedir, basedir, file, ext);
  if (access(gpgpathchk, F_OK) != 0) {
    fprintf(stderr, "パスワードが存在しません。\n");
    free(gpgpathchk);
    return -1;
  }
  free(gpgpathchk);

  int needed = snprintf(pwfile, sizeof(pwfile), "%s%s%s%s", homedir, basedir, file, ext);
  if (needed >= (int)sizeof(pwfile)) {
    fprintf(stderr, "エラー：パスが長すぎる。\n");
    return -1;
  }

  // 削除を確認する
  if (force == 0) { // パスワードの変更のばあい、確認は不要
    printf("パスワード「%s」を本当に削除する事が宜しいでしょうか？ (y/N): ", file);
    int confirm = getchar();
    if (confirm != 'y' && confirm != 'Y') {
      printf("削除しませんでした。\n");
      return -1;
    }

    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
  }

  if (unlink(pwfile) == -1) {
    perror("パスワードを削除出来ませんですた。");
    return -1;
  }

  if (force == 1) return 0;

  printf("パスワードを削除しました。\n");
  return 0;
}
