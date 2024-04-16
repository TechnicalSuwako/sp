#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "listpass.h"

void listpass(char* basePath, int level) {
  char *lang = getenv("SP_LANG");

  struct dirent* entry;
  DIR* dir = opendir(basePath);
  if (!dir) {
    if (lang != NULL && strncmp(lang, "en", 2) == 0)
      perror("Could not opening directory");
    else perror("ディレクトリを開けられません");
    return;
  }

  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    char path[1000];
    int needed = snprintf(path, sizeof(path), "%s/%s", basePath, entry->d_name);
    if (needed >= (int)sizeof(path) || needed < 0) {
      if (lang != NULL && strncmp(lang, "en", 2) == 0)
        perror("Error: Path is too long, or failed to getting lenth");
      else perror("エラー：パスが長すぎる、又は長さを受取に失敗");
      continue;
    }

    struct stat statbuf;
    if (stat(path, &statbuf) == -1) {
      if (lang != NULL && strncmp(lang, "en", 2) == 0)
        perror("Failed to reading file status");
      else perror("ファイル状況を読込に失敗");
      continue;
    }

    for (int i = 0; i < level; i++) {
      printf("    ");
    }

    if (S_ISDIR(statbuf.st_mode)) {
      printf("|-- %s\n", entry->d_name);
      listpass(path, level + 1);
    } else if (S_ISREG(statbuf.st_mode)) {
      char* filename = entry->d_name;
      char* ext = strstr(filename, ".gpg");
      if (ext) *ext = '\0';
      printf("|-- %s\n", filename);
    }
  }

  closedir(dir);
}
