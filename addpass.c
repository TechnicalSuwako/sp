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

void cleanup(gpgme_ctx_t ctx, gpgme_key_t key, gpgme_data_t in, gpgme_data_t out) {
  gpgme_data_release(in);
  gpgme_data_release(out);
  gpgme_release(ctx);
  gpgme_key_release(key);
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
  if (fgets(pw, pwlen, stdin) != NULL) {
    // あれば、改行を取り消す
    size_t len = strlen(pw);
    if (len > 0 && pw[len - 1] == '\n') {
      pw[len - 1] = '\0';
    } else {
      // 掃除
      int c;
      while ((c = getchar()) != '\n' && c != EOF);
    }
  }

  // 端末設定をもとに戻す
  tcsetattr(fileno(stdin), TCSANOW, &old);
}

void addpass(char* file) {
  // パスを準備
  char* homedir = getenv("HOME");
  if (homedir == NULL) {
    perror("ホームディレクトリを受取に失敗。");
    return;
  }

  char* basedir = "/.local/share/sp/";
  char* ext = ".gpg";

  char pass[256];
  char knin[256];

  int alllen = snprintf(NULL, 0, "%s%s%s%s", homedir, basedir, file, ext) + 1;
  char* gpgpathchk = malloc(alllen);
  if (gpgpathchk == NULL) {
    perror("メモリを割当に失敗。");
    return;
  }

  // ファイルが既に存在するかどうか確認
  snprintf(gpgpathchk, alllen, "%s%s%s%s", homedir, basedir, file, ext);
  if (access(gpgpathchk, F_OK) != -1) {
    fprintf(stderr, "パスワードが既に存在しています。\n変更するには、「 sp -e %s 」を実行して下さい。\n", file);
    free(gpgpathchk);
    return;
  }
  free(gpgpathchk);

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
  gpgme_key_t key[2] = {NULL, NULL};
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

  char* keyid = malloc(256);
  if (!fgets(keyid, 256, keyfile)) {
    perror("鍵IDを読込に失敗。");
    fclose(keyfile);
    free(keyid);
    return;
  }

  keyid[strcspn(keyid, "\n")] = 0;
  fclose(keyfile);

  err = gpgme_get_key(ctx, keyid, &key[0], 0);
  if (err) {
    fprintf(stderr, "鍵を受取に失敗： %s\n", gpgme_strerror(err));
    free(keyid);
    return;
  }

  if (key[0] == NULL) {
    fprintf(stderr, "エラー：鍵はNULLです。\n");
    free(keyid);
    return;
  }

  free(keyid);

  // 暗号化
  err = gpgme_op_encrypt(ctx, &key[0], GPGME_ENCRYPT_ALWAYS_TRUST, in, out);
  if (err) {
    fprintf(stderr, "暗号化に失敗： %s\n", gpgme_strerror(err));
    cleanup(ctx, key[0], in, out);
    return;
  }

  // 暗号化したタイルを開く
  char* gpgpath = malloc(alllen);
  if (gpgpath == NULL) {
    cleanup(ctx, key[0], in, out);
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
      cleanup(ctx, key[0], in, out);
      perror("ディレクトリを創作に失敗。");
      return;
    }
  }

  snprintf(gpgpath, alllen, "%s%s%s%s", homedir, basedir, file, ext);
  struct stat statbuf;
  if (stat(gpgpath, &statbuf) == 0) {
    free(gpgpath);
    cleanup(ctx, key[0], in, out);
    fprintf(stderr, "パスワードは既に存在しています。\n");
    return;
  }

  gpgfile = fopen(gpgpath, "wb");
  if (gpgfile == NULL) {
    perror("ファイルを開くに失敗。");
    printf("失敗したパス： %s\n", gpgpath);
    free(gpgpath);
    cleanup(ctx, key[0], in, out);
    return;
  }

  // データが保存したかどうか確認
  ssize_t encrypted_data_size = gpgme_data_seek(out, 0, SEEK_END);
  if (encrypted_data_size <= 0) {
    fprintf(stderr, "データを保存に失敗。\n");
    fclose(gpgfile);
    free(gpgpath);
    cleanup(ctx, key[0], in, out);
    return;
  }

  // 復号化したパスワードを表示する
  gpgme_data_seek(out, 0, SEEK_SET);

  char buffer[512];
  ssize_t read_bytes;

  while ((read_bytes = gpgme_data_read(out, buffer, sizeof(buffer))) > 0) {
    if (fwrite(buffer, 1, (size_t)read_bytes, gpgfile) != (size_t)read_bytes) {
      perror("パスワードを書き込みに失敗");
      free(gpgpath);
      cleanup(ctx, key[0], in, out);
      return;
    }
  }

  // 掃除
  fclose(gpgfile);
  free(gpgpath);
  cleanup(ctx, key[0], in, out);

  printf("パスワードを保存出来ました。\n");
}
