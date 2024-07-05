#include "common.h"
#include "initpass.h"

void initpass(char *gpgid) {
  char *lang = getlang();

  char *homedir = getenv("HOME");
  if (homedir == NULL) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Failed to get home directory.");
    else perror("ホームディレクトリを受取に失敗。");
    return;
  }

#if defined(__HAIKU)
  char *basedir = "/config/settings/sp/";
#else
  char *basedir = "/.local/share/sp/";
#endif
  char dirpath[256];
  snprintf(dirpath, sizeof(dirpath), "%s%s", homedir, basedir);

  if (mkdir_r(dirpath, 0755) != 0 && errno != EEXIST) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Failed to create directory.");
    else perror("ディレクトリを作成に失敗。");
    return;
  }

  char gpgidpath[512];
  snprintf(gpgidpath, sizeof(gpgidpath), "%s/.gpg-id", dirpath);

  struct stat statbuf;
  if (stat(gpgidpath, &statbuf) == 0) {
    if (strncmp(lang, "en", 2) == 0)
      perror(".gpg-id file already exists.");
    else perror(".gpg-idファイルは既に存在します。");
    return;
  }

  FILE *gpgidfile = fopen(gpgidpath, "w");
  if (gpgidfile == NULL) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Failed to write .gpg-id file.");
    else perror(".gpg-idファイルを書き込めません。");
    fclose(gpgidfile);
    return;
  }

  if (fputs(gpgid, gpgidfile) == EOF) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Failed to write .gpg-id file.");
    else perror(".gpg-idファイルへの書き込みに失敗しました。");
    fclose(gpgidfile);
    return;
  }

  fclose(gpgidfile);
  if (strncmp(lang, "en", 2) == 0)
    puts("Initialization completed.");
  else puts("初期設定に完了しました。");
}
