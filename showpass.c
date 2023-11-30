#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <locale.h>

#include <gpgme.h>

#include "showpass.h"

void clean_up(gpgme_ctx_t ctx, gpgme_data_t in, gpgme_data_t out, FILE* gpgfile, char* gpgpath) {
  if (gpgfile) fclose(gpgfile);
  if (gpgpath) free(gpgpath);
  gpgme_data_release(in);
  gpgme_data_release(out);
  gpgme_release(ctx);
}

void showpass(char* file) {
  gpgme_ctx_t ctx;
  gpgme_error_t err;
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

  // OpenPGPプロトコールを設定
  gpgme_set_protocol(ctx, GPGME_PROTOCOL_OpenPGP);

  // 暗号化したタイルを開く
  char* homedir = getenv("HOME");
  if (homedir == NULL) {
    perror("ホームディレクトリを受取に失敗。");
    return;
  }

  char* basedir = "/.local/share/sp/";
  char* ext = ".gpg";
  int alllen = snprintf(NULL, 0, "%s%s%s%s", homedir, basedir, file, ext) + 1;
  char* gpgpath = malloc(alllen);
  if (gpgpath == NULL) {
    perror("メモリを割当に失敗。");
    return;
  }

  sprintf(gpgpath, "%s%s%s%s", homedir, basedir, file, ext);
  gpgfile = fopen(gpgpath, "rb");
  if (gpgfile == NULL) {
    perror("ファイルを開くに失敗。");
    printf("失敗したパス： %s\n", gpgpath);
    free(gpgpath);
    return;
  }

  if (gpgme_data_new_from_stream(&in, gpgfile) != GPG_ERR_NO_ERROR || gpgme_data_new(&out) != GPG_ERR_NO_ERROR) {
    fprintf(stderr, "GPGMEデータオブジェクトを創作に失敗。\n");
    clean_up(ctx, in, out, gpgfile, gpgpath);
    return;
  }

  // データオブジェクトを創作
  gpgme_data_new(&out);

  // 復号化して
  err = gpgme_op_decrypt(ctx, in, out);
  if (err) {
    fprintf(stderr, "復号化に失敗： %s\n", gpgme_strerror(err));

    // 掃除
    clean_up(ctx, in, out, gpgfile, gpgpath);
    return;
  }

  // 復号化したパスワードを表示する
  gpgme_data_seek(out, 0, SEEK_SET);
  char buffer[512];
  ssize_t read_bytes;
  bool islastnl = false;

  while ((read_bytes = gpgme_data_read(out, buffer, sizeof(buffer) - 1)) > 0) {
    fwrite(buffer, 1, read_bytes, stdout);
    if (buffer[read_bytes - 1] == '\n') {
      islastnl = true;
    }
  }

  if (!islastnl) {
    putchar('\n');
  }

  // 掃除
  clean_up(ctx, in, out, gpgfile, gpgpath);
}
