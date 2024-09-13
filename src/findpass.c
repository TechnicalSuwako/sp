#include "common.h"
#include "findpass.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAXFINDLEN 1024

List fullpaths;
List dispaths;

#if defined(__linux__)
char *strcasestr(const char *haystack, const char *needle) {
  size_t needle_len = strlen(needle);
  if (needle_len == 0) {
    return (char *)haystack;
  }

  while (*haystack) {
    if (strncasecmp(haystack, needle, needle_len) == 0) {
      return (char *)haystack;
    }
    haystack++;
  }
  return NULL;
}
#endif

void rmext(char *filename) {
  char *ext = strrchr(filename, '.');
  if (ext != NULL && strcmp(ext, ".gpg") == 0) {
    *ext = '\0';
  }
}

void scanDir(const char *dpath, const char *rpath, List *fpaths) {
  char *lang = getlang();

  DIR *dir = opendir(dpath);
  if (!dir) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Could not open directory");
    else perror("ディレクトリを開けられません");
    exit(EXIT_FAILURE);
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    const char *name = entry->d_name;
    if (strncmp(name, ".", 1) == 0 || strncmp(name, "..", 2) == 0) continue;

    char fpath[MAXFINDLEN];
    snprintf(fpath, sizeof(fpath), "%s/%s", dpath, name);

    struct stat s;
    if (stat(fpath, &s) != 0) {
      closedir(dir);
      return;
    }

    if (S_ISDIR(s.st_mode)) {
      scanDir(fpath, rpath, fpaths);
    } else if (strstr(name, ".gpg") != NULL) {
      const char *rel = fpath + strlen(rpath) + 1;
      addElement(fpaths, rel);
      addElement(&fullpaths, fpath);
      char *disname = strdup(rel);
      rmext(disname);
      addElement(&dispaths, disname);

      free(disname);
    }
  }

  closedir(dir);
}

void findpass(const char *dpath, const char *txt) {
  List fpaths;
  initList(&fpaths);
  initList(&fullpaths);
  initList(&dispaths);
  scanDir(dpath, dpath, &fpaths);

  for (size_t i = 0; i < dispaths.size; i++) {
    if (strcasestr(getElement(&dispaths, i), txt) != NULL) {
      printf("%s\n", getElement(&dispaths, i));
    }
  }

  freeList(&fpaths);
}
