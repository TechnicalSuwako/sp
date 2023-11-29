#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#include <gpgme.h>
#include <termios.h>

#include "addpass.h"

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

void getpasswd(char* prompt, char*pw, size_t pwlen) {
  struct termios old, new;
  printf("%s", prompt);

  // 端末設定を受け取る
  tcgetattr(fileno(stdin), &old);
  new = old;

  // echoを無効にして
  new.c_lflag &= ~ECHO;
  tcsetattr(fileno(stdin), TCSANOW, &new);

  // パスワードを読み込む
  fgets(pw, pwlen, stdin);

  // 端末設定をもとに戻す
  tcsetattr(fileno(stdin), TCSANOW, &old);

  // 改行文字を取り消す
  pw[strcspn(pw, "\n")] = 0;
}

void addpass(char* file) {
  char pass[100];
  char knin[100];

  // パスワードを受け取る
  getpasswd("パスワード： ", pass, sizeof(pass));
  puts("");
  getpasswd("パスワード（確認用）： ", knin, sizeof(knin));
  puts("");

  // パスワードが一致するかどうか確認
  if (strcmp(pass, knin) != 0) {
    fprintf(stderr, "パスワードが一致していません。終了…\n");
    return;
  }

  // パスワードを保存する
  gpgme_ctx_t ctx;
  gpgme_error_t err;
  gpgme_key_t key;
  gpgme_data_t in, out;
  FILE *gpgfile;

  // GPGMEライブラリを設置
  setlocale(LC_ALL, "");
  gpgme_check_version(NULL);
  gpgme_set_locale(NULL, LC_CTYPE, setlocale(LC_CTYPE, NULL));

  // GPGMEを創作
  err = gpgme_new(&ctx);
  if (err) {
    fprintf(stderr, "GPGMEを創作に失敗：%s\n", gpgme_strerror(err));
    return;
  }

  // GPGMEは非対話的モードに設定
  err = gpgme_set_pinentry_mode(ctx, GPGME_PINENTRY_MODE_LOOPBACK);
  if (err) {
    fprintf(stderr, "pinentryモードを設定に失敗： %s\n", gpgme_strerror(err));
    gpgme_release(ctx);
    return;
  }

  // パスワードからデータオブジェクトを創作
  err = gpgme_data_new_from_mem(&in, pass, strlen(pass), 0);
  if (err) {
    fprintf(stderr, "データオブジェクトを創作に失敗： %s\n", gpgme_strerror(err));
    gpgme_release(ctx);
    return;
  }

  gpgme_data_new(&out);

  // パスを準備
  char* homedir = getenv("HOME");
  if (homedir == NULL) {
    perror("ホームディレクトリを受取に失敗。");
    return;
  }

  char* basedir = "/.local/share/sp/";

  // 鍵を受け取る
  char keypath[256];
  snprintf(keypath, sizeof(keypath), "%s%s%s", homedir, basedir, ".gpg-id");
  keypath[sizeof(keypath) - 1] = '\0';

  FILE* keyfile = fopen(keypath, "rb");
  if (keyfile == NULL) {
    perror(".gpg-idファイルを開くに失敗。");
    printf("失敗したパス： %s\n", keypath);
    return;
  }

  char* keyid = malloc(100);
  if (!fgets(keyid, 100, keyfile)) {
    perror("鍵IDを読込に失敗。");
    fclose(keyfile);
    free(keyid);
    return;
  }

  keyid[strcspn(keyid, "\n")] = 0;
  fclose(keyfile);

  err = gpgme_get_key(ctx, keyid, &key, 0);
  if (err) {
    fprintf(stderr, "鍵を受取に失敗： %s\n", gpgme_strerror(err));
    free(keyid);
    return;
  }

  if (key == NULL) {
    fprintf(stderr, "エラー：鍵はNULLです。\n");
    free(keyid);
    return;
  }

  free(keyid);

  // 暗号化
  gpgme_key_t recp[1] = {key};
  err = gpgme_op_encrypt(ctx, recp, GPGME_ENCRYPT_ALWAYS_TRUST, in, out);
  if (err) {
    fprintf(stderr, "暗号化に失敗： %s\n", gpgme_strerror(err));
    gpgme_data_release(in);
    gpgme_data_release(out);
    gpgme_release(ctx);
    gpgme_key_release(key);
    return;
  }

  // 暗号化したタイルを開く
  char* ext = ".gpg";
  int alllen = snprintf(NULL, 0, "%s%s%s%s", homedir, basedir, file, ext) + 1;
  char* gpgpath = malloc(alllen);
  if (gpgpath == NULL) {
    gpgme_data_release(in);
    gpgme_data_release(out);
    gpgme_release(ctx);
    gpgme_key_release(key);
    perror("メモリを割当に失敗。");
    return;
  }

  // ディレクトリを創作
  char dirpath[512];
  snprintf(dirpath, sizeof(dirpath), "%s%s%s", homedir, basedir, file);
  dirpath[sizeof(dirpath) - 1] = '\0';

  char* lastsla = strrchr(dirpath, '/');
  if (lastsla != NULL) {
    *lastsla = '\0';
    if (mkdir_r(dirpath, 0755) != 0) {
      free(gpgpath);
      gpgme_data_release(in);
      gpgme_data_release(out);
      gpgme_release(ctx);
      gpgme_key_release(key);
      perror("ディレクトリを創作に失敗。");
      return;
    }
  }

  sprintf(gpgpath, "%s%s%s%s", homedir, basedir, file, ext);
  gpgfile = fopen(gpgpath, "wb");
  if (gpgfile == NULL) {
    perror("ファイルを開くに失敗。");
    printf("失敗したパス： %s\n", gpgpath);
    free(gpgpath);
    gpgme_data_release(in);
    gpgme_data_release(out);
    gpgme_release(ctx);
    gpgme_key_release(key);
    return;
  }

  // データが保存したかどうか確認
  ssize_t encrypted_data_size = gpgme_data_seek(out, 0, SEEK_END);
  if (encrypted_data_size <= 0) {
    fprintf(stderr, "データを保存に失敗。\n");
    fclose(gpgfile);
    free(gpgpath);
    gpgme_data_release(in);
    gpgme_data_release(out);
    gpgme_release(ctx);
    gpgme_key_release(key);
    return;
  }

  // 復号化したパスワードを表示する
  gpgme_data_seek(out, 0, SEEK_SET);

  char buffer[512];
  ssize_t read_bytes;
  while ((read_bytes = gpgme_data_read(out, buffer, sizeof(buffer))) > 0) {
    fwrite(buffer, 1, read_bytes, gpgfile);
  }

  // 掃除
  fclose(gpgfile);
  free(gpgpath);
  gpgme_data_release(in);
  gpgme_data_release(out);
  gpgme_release(ctx);
  gpgme_key_release(key);

  printf("パスワードを保存出来ました。\n");
}
