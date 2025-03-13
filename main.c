#include "src/common.h"
#include "src/initpass.h"
#include "src/showpass.h"
#include "src/yankpass.h"
#include "src/listpass.h"
#include "src/addpass.h"
#include "src/delpass.h"
#include "src/genpass.h"
#include "src/otppass.h"
#include "src/findpass.h"
#include "src/vulnpass.h"
#include "src/chkpass.h"

#include <unistd.h>

const char *sofname = "sp";
const char *version = "1.5.1";
const char *avalopt = "abcdefgiloOsvy";
const char *madefor = "simpas 1.1.1";

void usage() {
  printf("%s-%s (%s)\nusage: %s [-%s]\n",
         sofname, version, madefor, sofname, avalopt);
}

char *getfullpath(char *arg) {
  char *lang = getlang();

  char *basedir = getbasedir(1);
  size_t fullPathLen;
  char *fullPath;
  fullPathLen = strlen(basedir) + (arg != NULL ? strlen(arg) + 5 : 0);

  fullPath = (char *)malloc(fullPathLen);
  if (fullPath == NULL) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Failed to allocating memory");
    else perror("メモリの役割に失敗");
    free(basedir);
    return NULL;
  }

  if (arg != NULL) {
    snprintf(fullPath, fullPathLen, "%s%s.gpg", basedir, arg);
  } else {
    snprintf(fullPath, fullPathLen, "%s", basedir);
  }

  return fullPath;
}

void editpass(char *file) {
  char *lang = getlang();

#if defined(__HAIKU__)
  const char *tmpfile = "/boot/system/cache/sp-tmp.gpg";
#else
  const char *tmpfile = "/tmp/sp-tmp.gpg";
#endif
  tmpcopy(file, tmpfile);
  if (delpass(file, 1) != 0) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Editing failed: Failed to delete.");
    else perror("編集に失敗:削除に失敗");
    return;
  }
  if (addpass(file) != 0) {
    tmpcopy(tmpfile, file);
    unlink(tmpfile);
    if (strncmp(lang, "en", 2) == 0)
      perror("Editing failed: Failed to add.");
    else perror("編集に失敗:追加に失敗");
    return;
  }

  unlink(tmpfile);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    usage();
    return 0;
  }

  if (strcmp(argv[1], "-g") == 0) {
    if (argc > 4) {
      usage();
      return 1;
    }

    if      (argc == 2) genpass(64, true);
    else if (argc == 3) genpass(atoi(argv[2]), true);
    else if (argc == 4 && strcmp(argv[3], "risk") == 0)
      genpass(atoi(argv[2]), false);
    else if (argc == 4 && strcmp(argv[3], "secure") == 0)
      genpass(atoi(argv[2]), true);

    return 0;
  }

  if (argc == 3) {
    if      (strcmp(argv[1], "-i") == 0) initpass(argv[2]);
    else if (strcmp(argv[1], "-s") == 0) {
      const char *pass = showpass(argv[2]);
      if (pass == NULL) return -1;
      printf("%s\n", pass);
    }
    else if (strcmp(argv[1], "-y") == 0) yankpass(argv[2], 45);
    else if (strcmp(argv[1], "-a") == 0) addpass(argv[2]);
    else if (strcmp(argv[1], "-d") == 0) delpass(argv[2], 0);
    else if (strcmp(argv[1], "-e") == 0) {
      editpass(argv[2]);
    } else if (strcmp(argv[1], "-o") == 0) {
      char *fullPath = getfullpath(argv[2]);
      if (fullPath == NULL) return -1;
      otppass(fullPath, 0, 0);
      free(fullPath);
    } else if (strcmp(argv[1], "-O") == 0) {
      char *fullPath = getfullpath(argv[2]);
      if (fullPath == NULL) return -1;
      otppass(fullPath, 1, 30);
      free(fullPath);
    } else if (strcmp(argv[1], "-f") == 0) {
      char *fullPath = getfullpath(NULL);
      if (fullPath == NULL) return -1;
      findpass(fullPath, argv[2]);
      free(fullPath);
    } else if (strcmp(argv[1], "-c") == 0) {
      char *fullPath = getfullpath(NULL);
      if (fullPath == NULL) return -1;
      chkpass(fullPath, argv[2]);
      free(fullPath);
    } else {
      usage();
      return 1;
    }
  } else if (argc == 4) {
    if (strcmp(argv[1], "-y") == 0) {
      int i;
      if (sscanf(argv[3], "%d", &i) == 0) yankpass(argv[2], 45);
      else yankpass(argv[2], atoi(argv[3]));
    } else if (strcmp(argv[1], "-O") == 0) {
      char *fullPath = getfullpath(argv[2]);
      if (fullPath == NULL) return -1;
      int i;
      if (sscanf(argv[3], "%d", &i) == 0) otppass(fullPath, 1, 30);
      else otppass(fullPath, 1, atoi(argv[3]));
      free(fullPath);
    }
  } else if (argc == 2) {
    char *basePath = getfullpath(NULL);
    if (basePath == NULL) return -1;

    if      (strcmp(argv[1], "-l") == 0) listpass(basePath, 0);
    else if (strcmp(argv[1], "-b") == 0) vulnpass(basePath);
    else if (strcmp(argv[1], "-v") == 0) {
      printf("%s-%s (%s)\n", sofname, version, madefor);
    } else {
      usage();
      free(basePath);
      return 1;
    }
    free(basePath);
  } else {
    usage();
    return 1;
  }

  return 0;
}
