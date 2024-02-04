// TODO:未完了
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "chkpass.h"
#include "showpass.h"

struct PassList {
  char filepath[1024];
  char password[256];
};

void chkpass(char* basePath) {
  char *lang = getenv("SP_LANG");

  // パスワードを保存する
  struct PassList* allpass = malloc(2048 * sizeof(struct PassList));
  if (allpass == NULL) {
    if (lang != NULL && strncmp(lang, "en", 2) == 0) perror("Failed to allocating memory");
    else perror("メモリを役割に失敗");
    return;
  }

  struct dirent* entry;
  DIR* dir = opendir(basePath);
  if (!dir) {
    if (lang != NULL && strncmp(lang, "en", 2) == 0) perror("Could not opening directory");
    else perror("ディレクトリを開けられません");
    free(allpass);
    return;
  }

  int i = 0;

  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    char path[1000];
    int needed = snprintf(path, sizeof(path), "%s/%s", basePath, entry->d_name);
    if (needed >= (int)sizeof(path) || needed < 0) {
      if (lang != NULL && strncmp(lang, "en", 2) == 0) perror("Error: Path is too long, or failed to getting lenth");
      else perror("エラー：パスが長すぎる、又は長さを受取に失敗");
      continue;
    }

    struct stat statbuf;
    if (stat(path, &statbuf) == -1) {
      if (lang != NULL && strncmp(lang, "en", 2) == 0) perror("Failed to reading file status");
      else perror("ファイル状況を読込に失敗");
      continue;
    }

    if (!S_ISREG(statbuf.st_mode)) {
      continue;
    }

    char* filename = entry->d_name;
    char* ext = strstr(filename, ".gpg");
    if (ext) *ext = '\0';
    char spath[2048];
    snprintf(spath, sizeof(spath), "%s/%s", path, filename);

    if (i >= 2048) {
      if (lang != NULL && strncmp(lang, "en", 2) == 0) perror("Too much password");
      else perror("パスワードが多すぎる");
      break;
    }

    strcpy(allpass[i].filepath, spath);
    strcpy(allpass[i].password, showpass(spath));
    i++;
  }

  closedir(dir);

  // TODO: パスワードの確認
  /* for (size_t i = 0; i < sizeof(allpass) / sizeof(allpass[0]); i++) { */
  /*   char passpath[1000]; */
  /*   snprintf(passpath, sizeof(passpath), "%s/%s", basePath, allpass.filepath); */
  /* } */

  // 掃除
  free(allpass);
}
