#include "common.h"
#include "initpass.h"

void initpass(char *gpgid) {
  char *lang = getlang();

  char *basedir = getbasedir(1);

  if (mkdir_r(basedir, 0755) != 0 && errno != EEXIST) {
    if (strncmp(lang, "en", 2) == 0)
      fprintf(stderr, "Failed to create directory.\n");
    else fprintf(stderr, "ディレクトリを作成に失敗。\n");
    return;
  }

  char gpgidpath[512];
  snprintf(gpgidpath, sizeof(gpgidpath), "%s/.gpg-id", basedir);

  struct stat statbuf;
  if (stat(gpgidpath, &statbuf) == 0) {
    if (strncmp(lang, "en", 2) == 0)
      fprintf(stderr, ".gpg-id file already exists.\n");
    else fprintf(stderr, ".gpg-idファイルは既に存在します。\n");
    return;
  }

  FILE *gpgidfile = fopen(gpgidpath, "w");
  if (gpgidfile == NULL) {
    if (strncmp(lang, "en", 2) == 0)
      fprintf(stderr, "Failed to write .gpg-id file.\n");
    else fprintf(stderr, ".gpg-idファイルを書き込めません。\n");
    return;
  }

  if (fprintf(gpgidfile, "%s\n", gpgid) < 0) {
    if (strncmp(lang, "en", 2) == 0)
      fprintf(stderr, "Failed to write .gpg-id file.\n");
    else fprintf(stderr, ".gpg-idファイルへの書き込みに失敗しました。\n");
    fclose(gpgidfile);
    return;
  }

  fclose(gpgidfile);

  if (strncmp(lang, "en", 2) == 0)
    puts("Initialization completed.");
  else puts("初期設定に完了しました。");
}
