#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#include "initpass.h"
#include "addpass.h"

void initpass(char* gpgid) {
  char* homedir = getenv("HOME");
  if (homedir == NULL) {
    perror("ホームディレクトリを受取に失敗。");
    return;
  }

  char* basedir = "/.local/share/sp/";
  char dirpath[256];
  snprintf(dirpath, sizeof(dirpath), "%s%s", homedir, basedir);

  if (mkdir_r(dirpath, 0755) != 0 && errno != EEXIST) {
    perror("ディレクトリを作成に失敗。");
    return;
  }

  char gpgidpath[512];
  snprintf(gpgidpath, sizeof(gpgidpath), "%s/.gpg-id", dirpath);

  struct stat statbuf;
  if (stat(gpgidpath, &statbuf) == 0) {
    fprintf(stderr, ".gpg-idファイルは既に存在します。\n");
    return;
  }

  FILE* gpgidfile = fopen(gpgidpath, "w");
  if (gpgidfile == NULL) {
    perror(".gpg-idファイルを書き込めません。");
    fclose(gpgidfile);
    return;
  }

  if (fputs(gpgid, gpgidfile) == EOF) {
    fprintf(stderr, ".gpg-idファイルへの書き込みに失敗しました。\n");
    fclose(gpgidfile);
    return;
  }

  fclose(gpgidfile);
  printf("初期設定に完了しました。");
}
