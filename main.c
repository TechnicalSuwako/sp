#include "src/common.h"
#include "src/initpass.h"
#include "src/showpass.h"
#include "src/yankpass.h"
#include "src/listpass.h"
#include "src/addpass.h"
#include "src/delpass.h"
#include "src/genpass.h"
#include "src/otppass.h"

const char *sofname = "sp";
const char *version = "1.3.0";

void usage() {
  printf("usage: %s-%s [-adegilosvy]\n", sofname, version);
}

char *getfullpath(char *arg) {
  char *lang = getlang();

  char *homedir = getenv("HOME");
  if (homedir == NULL) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Failed to getting home directory");
    else perror("ホームディレクトリを受取に失敗");
    return NULL;
  }

  char *basedir = "/.local/share/sp/";
  size_t fullPathLen;
  char *fullPath;
  if (arg != NULL) {
    fullPathLen = strlen(homedir) + strlen(basedir) + strlen(arg) + 5;
  } else {
    fullPathLen = strlen(homedir) + strlen(basedir);
  }

  fullPath = (char *)malloc(fullPathLen);
  if (fullPath == NULL) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Failed to allocating memory");
    else perror("メモリの役割に失敗");
    free(fullPath);
    free(homedir);
    return NULL;
  }

  if (arg != NULL) {
    snprintf(fullPath, fullPathLen, "%s%s%s.gpg", homedir, basedir, arg);
  } else {
    snprintf(fullPath, fullPathLen, "%s%s", homedir, basedir);
  }

  return fullPath;
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
    else if (strcmp(argv[1], "-y") == 0) yankpass(argv[2]);
    else if (strcmp(argv[1], "-a") == 0) addpass(argv[2]);
    else if (strcmp(argv[1], "-d") == 0) delpass(argv[2], 0);
    else if (strcmp(argv[1], "-e") == 0) {
      delpass(argv[2], 1);
      addpass(argv[2]);
    } else if (strcmp(argv[1], "-o") == 0) {
      char *fullPath = getfullpath(argv[2]);
      if (fullPath == NULL) return -1;
      otppass(fullPath);
      free(fullPath);
    } else {
      usage();
      return 1;
    }
  } else if (argc == 2) {
    char *basePath = getfullpath(NULL);
    if (basePath == NULL) return -1;

    if      (strcmp(argv[1], "-l") == 0) listpass(basePath, 0);
    else if (strcmp(argv[1], "-v") == 0) printf("%s-%s\n", sofname, version);
    else {
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
