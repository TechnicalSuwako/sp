#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#include "initpass.h"
#include "addpass.h"

void initpass(char* gpgid) {
  char *lang = getenv("SP_LANG");

  char* homedir = getenv("HOME");
  if (homedir == NULL) {
    if (lang != NULL && strncmp(lang, "en", 2) == 0)
      perror("Failed to getting home directory.");
    else perror("ホームディレクトリを受取に失敗。");
    return;
  }

  char* basedir = "/.local/share/sp/";
  char dirpath[256];
  snprintf(dirpath, sizeof(dirpath), "%s%s", homedir, basedir);

  if (mkdir_r(dirpath, 0755) != 0 && errno != EEXIST) {
    if (lang != NULL && strncmp(lang, "en", 2) == 0)
      perror("Failed to creating directory.");
    else perror("ディレクトリを作成に失敗。");
    return;
  }

  char gpgidpath[512];
  snprintf(gpgidpath, sizeof(gpgidpath), "%s/.gpg-id", dirpath);

  struct stat statbuf;
  if (stat(gpgidpath, &statbuf) == 0) {
    if (lang != NULL && strncmp(lang, "en", 2) == 0)
      perror(".gpg-id file is already exist.");
    else perror(".gpg-idファイルは既に存在します。");
    return;
  }

  FILE* gpgidfile = fopen(gpgidpath, "w");
  if (gpgidfile == NULL) {
    if (lang != NULL && strncmp(lang, "en", 2) == 0)
      perror("Failed to writing .gpg-id file.");
    else perror(".gpg-idファイルを書き込めません。");
    fclose(gpgidfile);
    return;
  }

  if (fputs(gpgid, gpgidfile) == EOF) {
    if (lang != NULL && strncmp(lang, "en", 2) == 0)
      perror("Failed to writing .gpg-id file.");
    else perror(".gpg-idファイルへの書き込みに失敗しました。");
    fclose(gpgidfile);
    return;
  }

  fclose(gpgidfile);
  if (lang != NULL && strncmp(lang, "en", 2) == 0)
    puts("First time setup was complete.");
  else puts("初期設定に完了しました。");
}
