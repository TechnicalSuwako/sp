#include "common.h"

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAXFINDLEN 1024

char *getbasedir(int trailing) {
  char *lang = getlang();

  char *homedir = getenv("HOME");
  if (homedir == NULL) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Failed to get home directory");
    else perror("ホームディレクトリを受取に失敗");
    return NULL;
  }

#if defined(__HAIKU__)
  char *basedir = "/config/settings/sp";
  char *slash = "/";
#elif defined(_WIN32)
  char *basedir = "\\AppData\\Local\\076\\sp";
  char *slash = "\\";
#else
  char *basedir = "/.local/share/sp";
  char *slash = "/";
#endif

  size_t len = strlen(homedir) + strlen(basedir) + strlen(slash) + 4;
  char *res = malloc(len);
  if (res == NULL) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Failed to allocate memory");
    else perror("メモリの役割に失敗");
    return NULL;
  }
  if (trailing == 1)
    snprintf(res, len, "%s%s%s", homedir, basedir, slash);
  else
    snprintf(res, len, "%s%s", homedir, basedir);

  return res;
}

char *getlang() {
  char *lang = NULL;

  lang = getenv("SP_LANG");
  if (lang == NULL) lang = "ja";

  return lang;
}

int mkdir_r(const char *path, mode_t mode) {
  char tmp[256];
  char *p = NULL;
  size_t len;

  snprintf(tmp, sizeof(tmp), "%s", path);

  len = strlen(tmp);
  if (tmp[len - 1] == '/') {
    tmp[len - 1] = 0; // 最後の「/」文字を取り消す
  }

  for (p = tmp + 1; *p; p++) {
    if (*p == '/') {
      *p = 0; // 最後の「/」文字を取り消す
      if (mkdir(tmp, mode) != 0 && errno != EEXIST) return -1;
      *p = '/'; // また追加
    }
  }

  if (mkdir(tmp, mode) != 0 && errno != EEXIST) {
    return -1;
  }

  return 0;
}

int tmpcopy(const char *inpath, const char *outpath) {
  FILE *src = fopen(inpath, "rb");
  if (!src) {
    return -1;
  }

  FILE *dst = fopen(outpath, "wb");
  if (!dst) {
    fclose(src);
    return -1;
  }

  size_t n, m;
  unsigned char buf[8192];
  do {
    n = fread(buf, 1, sizeof buf, src);
    if (n) m = fwrite(buf, 1, n, dst);
    else   m = 0;
  } while ((n > 0) && (n == m));
  if (m) return -1;

  fclose(src);
  fclose(dst);

  return 0;
}

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

void scanDir(const char *dpath, const char *rpath, List *fpaths,
    List *fullpaths, List *dispaths) {
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
      scanDir(fpath, rpath, fpaths, fullpaths, dispaths);
    } else if (strstr(name, ".gpg") != NULL) {
      const char *rel = fpath + strlen(rpath) + 1;
      addElement(fpaths, rel);
      addElement(fullpaths, fpath);
      char *disname = strdup(rel);
      rmext(disname);
      addElement(dispaths, disname);

      free(disname);
    }
  }

  closedir(dir);
}

void initList(List *list) {
  list->head = NULL;
  list->tail = NULL;
  list->size = 0;
}

void addElement(List *list, const char *data) {
  Node *newNode = (Node *)malloc(sizeof(Node));
  if (!newNode) {
    return;
  }

  newNode->data = strdup(data);
  if (!newNode->data) {
    free(newNode);
    return;
  }
  newNode->next = NULL;

  if (list->tail) {
    list->tail->next = newNode;
  } else {
    list->head = newNode;
  }

  list->tail = newNode;
  list->size++;
}

char *getElement(List *list, size_t idx) {
  if (idx >= list->size) return NULL;

  Node *current = list->head;
  for (size_t i = 0; i < idx; i++)
    current = current->next;

  return current->data;
}

void rmElement(List *list, size_t idx) {
  if (idx >= list->size) return;

  Node *current = list->head;
  Node *previous = NULL;

  if (idx == 0) {
    list->head = current->next;
    if (list->size == 1) list->tail = NULL;
  } else {
    for (size_t i = 0; i < idx; i++) {
      previous = current;
      current = current->next;
    }
    previous->next = current->next;
    if (idx == list->size - 1) {
      list->tail = previous;
    }
  }

  free(current->data);
  free(current);
  list->size--;
}

void freeList(List *list) {
  Node *current = list->head;

  while (current) {
    Node *next = current->next;
    free(current->data);
    free(current);
    current = next;
  }

  list->head = NULL;
  list->tail = NULL;
  list->size = 0;
}

void handle_sigint(int sig) {
  (void)sig;

  if (getenv("WAYLAND_DISPLAY") != NULL) {
    system("echo -n \"\" | wl-copy");
  } else {
    system("echo -n \"\" | xclip -selection clipboard");
  }

  if (strncmp(getlang(), "en", 2) == 0) {
    printf("\nClipboard cleared and program aborted.\n");
  } else {
    printf("\nクリップボードをクリアし、プログラムが中止されました。\n");
  }

  exit(0);
}
