#include <unistd.h>
#include <dirent.h>

#include "common.h"
#include "delpass.h"

int delcnt(const char *str, char delimiter) {
  int count = 0;
  while (*str) {
    if (*str == delimiter) {
      count++;
    }
    str++;
  }

  return count;
}

char **explode(const char *str, char delimiter, int *numtokens) {
  int count = delcnt(str, delimiter) + 1;
  *numtokens = count;

  char **tokens = malloc(count * sizeof(char *));
  if (tokens == NULL) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  char *strcopy = strdup(str);
  if (strcopy == NULL) {
    perror("strdup");
    exit(EXIT_FAILURE);
  }

  char *token = strtok(strcopy, &delimiter);
  int i = 0;
  while (token != NULL) {
    tokens[i] = strdup(token);
    if (tokens[i] == NULL) {
      perror("strdup");
      exit(EXIT_FAILURE);
    }
    i++;
    token = strtok(NULL, &delimiter);
  }

  free(strcopy);

  return tokens;
}

void freetokens(char **tokens, int numtokens) {
  for (int i = 0; i < numtokens; i++) {
    free(tokens[i]);
  }

  free(tokens);
}

int delpass(char *file, int force) {
  char *lang = getlang();

  // パスを準備
  char pwfile[512];
  char *homedir = getenv("HOME");
  if (homedir == NULL) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Failed to getting home directory");
    else perror("ホームディレクトリを受取に失敗");
    return -1;
  }

  char *basedir = "/.local/share/sp/";
  char *ext = ".gpg";

  int alllen = snprintf(NULL, 0, "%s%s%s%s", homedir, basedir, file, ext) + 1;
  char *gpgpathchk = malloc(alllen);
  if (gpgpathchk == NULL) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Failed to allocating memory");
    else perror("メモリを割当に失敗");
    return -1;
  }

  // ファイルが既に存在するかどうか確認
  snprintf(gpgpathchk, alllen, "%s%s%s%s", homedir, basedir, file, ext);
  if (access(gpgpathchk, F_OK) != 0) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Password does not exist");
    else perror("パスワードが存在しません");
    free(gpgpathchk);
    return -1;
  }
  free(gpgpathchk);

  int needed = snprintf(
    pwfile,
    sizeof(pwfile),
    "%s%s%s%s",
    homedir,
    basedir,
    file,
    ext
  );
  if (needed >= (int)sizeof(pwfile)) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Error: Path is too long");
    else perror("エラー：パスが長すぎる");
    return -1;
  }

  // 削除を確認する
  if (force == 0) { // パスワードの変更の場合、確認は不要
    if (strncmp(lang, "en", 2) == 0)
      printf("Is it really good if I delete the password '%s'? (y/N): ", file);
    printf("パスワード「%s」を本当に削除する事が宜しいでしょうか？ (y/N): ", file);
    int confirm = getchar();
    if (confirm != 'y' && confirm != 'Y') {
      if (strncmp(lang, "en", 2) == 0) puts("Not deleted");
      else puts("削除しませんでした");
      return -1;
    }

    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
  }

  if (unlink(pwfile) == -1) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Password cannot be delete");
    else perror("パスワードを削除出来ませんですた");
    return -1;
  }

  // 空のディレクトリの場合
  int numt;
  char **tokens = explode(file, '/', &numt);

  char basepath[1024];
  char passpath[1024];
  snprintf(basepath, sizeof(basepath), "%s%s", homedir, basedir);
  snprintf(passpath, sizeof(passpath), "%s%s%s", homedir, basedir, tokens[0]);
  char *ls = strrchr(basepath, '/');
  if (ls != NULL) {
    *ls = '\0';
  }

  // TODO: ここはメモリに関するバグが起こっている為、未だmasterに入れない
  for (int i = 1; i < numt; i++) {
    if (i == (numt-1)) continue;
    strncat(passpath, "/", sizeof(passpath) - strlen(passpath) - 1);
    strncat(passpath, tokens[i], sizeof(passpath) - strlen(passpath) - 1);
  }

  for (int i = numt - 1; i >= 0; i--) {
    // ~/.local/share/sp を削除したら危険
    if (strncmp(passpath, basepath, sizeof(passpath)) == 0) {
      break;
    }

    // ディレクトリが空じゃない場合、削除を止める
    if (rmdir(passpath) == -1) {
      break;
    }

    char *last_slash = strrchr(passpath, '/');
    if (last_slash != NULL) {
      *last_slash = '\0';
    }
  }

  freetokens(tokens, numt);

  // sp -e の場合、「パスワードを削除しました」って要らない
  if (force == 1) return 0;

  if (strncmp(lang, "en", 2) == 0) puts("Deleted password");
  else puts("パスワードを削除しました");

  return 0;
}
