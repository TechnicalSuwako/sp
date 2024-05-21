#include <unistd.h>

#include "common.h"
#include "delpass.h"

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

  if (force == 1) return 0;

  if (strncmp(lang, "en", 2) == 0) puts("Deleted password");
  else puts("パスワードを削除しました");

  return 0;
}
