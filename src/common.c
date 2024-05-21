#include "common.h"

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
