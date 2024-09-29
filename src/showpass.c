#include <locale.h>

#include "common.h"
#include "showpass.h"

void clean_up(
  gpgme_ctx_t ctx,
  gpgme_data_t in,
  gpgme_data_t out,
  FILE *gpgfile,
  char *gpgpath
) {
  if (gpgfile) fclose(gpgfile);
  if (gpgpath) free(gpgpath);
  gpgme_data_release(in);
  gpgme_data_release(out);
  gpgme_release(ctx);
}

const char *showpass(char *file) {
  char *lang = getlang();

  gpgme_ctx_t ctx;
  gpgme_error_t err;
  gpgme_data_t in = NULL, out = NULL;
  char *gpgpath = NULL;
  FILE *gpgfile;

  // GPGMEライブラリを設置
  setlocale(LC_ALL, "");
  gpgme_check_version(NULL);
  gpgme_set_locale(NULL, LC_CTYPE, setlocale(LC_CTYPE, NULL));

  // GPGMEを創作
  err = gpgme_new(&ctx);
  if (err) {
    if (strncmp(lang, "en", 2) == 0)
      fprintf(stderr, "Failed to generate GPGME: %s\n", gpgme_strerror(err));
    else fprintf(stderr, "GPGMEを創作に失敗：%s\n", gpgme_strerror(err));
    return NULL;
  }

  // OpenPGPプロトコールを設定
  gpgme_set_protocol(ctx, GPGME_PROTOCOL_OpenPGP);
  if (err) {
    if (strncmp(lang, "en", 2) == 0)
      fprintf(stderr, "Failed to set OpenPGP protocol: %s\n", gpgme_strerror(err));
    else fprintf(stderr, "OpenPGP付トロコールの設置に失敗：%s\n", gpgme_strerror(err));
    clean_up(ctx, in, out, gpgfile, gpgpath);
    return NULL;
  }

  // 暗号化したタイルを開く
  char *basedir = getbasedir(1);
  char *ext = ".gpg";
  int alllen = snprintf(NULL, 0, "%s%s%s", basedir, file, ext) + 1;
  gpgpath = malloc(alllen);
  if (gpgpath == NULL) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Failed to allocate memeory");
    else perror("メモリを割当に失敗");
    return NULL;
  }

  snprintf(gpgpath, alllen, "%s%s%s", basedir, file, ext);

  gpgfile = fopen(gpgpath, "rb");
  if (gpgfile == NULL) {
    if (strncmp(lang, "en", 2) == 0) {
      perror("Failed to open file");
    } else {
      perror("ファイルを開くに失敗");
    }
    free(gpgpath);
    return NULL;
  }

  // ファイルからinデータオブジェクトを創作
  if (gpgme_data_new_from_stream(&in, gpgfile) != GPG_ERR_NO_ERROR) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Failed to generate the GPGME data object");
    else perror("GPGMEデータオブジェクトを創作に失敗");
    clean_up(ctx, in, out, gpgfile, gpgpath);
    return NULL;
  }

  // outデータオブジェクトを創作
  if (gpgme_data_new(&out) != GPG_ERR_NO_ERROR) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Failed to generate the GPGME data object");
    else perror("GPGMEデータオブジェクトを創作に失敗");
    clean_up(ctx, in, out, gpgfile, gpgpath);
    return NULL;
  }

  // データオブジェクトを創作
  gpgme_data_new(&out);

  // 復号化して
  err = gpgme_op_decrypt(ctx, in, out);
  if (err) {
    if (strncmp(lang, "en", 2) == 0)
      fprintf(stderr, "Failed to decrypt: %s (source: %s)\n",
          gpgme_strerror(err), gpgme_strsource(err));
    else fprintf(stderr, "復号化に失敗： %s (元： %s)\n",
          gpgme_strerror(err), gpgme_strsource(err));

    // 掃除
    clean_up(ctx, in, out, gpgfile, gpgpath);
    return NULL;
  }

  // 復号化したパスワードを表示する
  gpgme_data_seek(out, 0, SEEK_SET);
  size_t bufsize = 512;
  size_t totsize = 0;
  char buffer[512];
  char *res = malloc(bufsize);
  if (res == NULL) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Failed to allocate memory");
    else perror("メモリを役割に失敗");
    clean_up(ctx, in, out, gpgfile, gpgpath);
    return NULL;
  }

  ssize_t read_bytes;

  while ((read_bytes = gpgme_data_read(out, buffer, sizeof(buffer) - 1)) > 0) {
    if (totsize + read_bytes >= bufsize) {
      bufsize *= 2;
      res = realloc(res, bufsize);
      if (res == NULL) {
        perror("Failed to reallocate memory");
        clean_up(ctx, in, out, gpgfile, gpgpath);
        return NULL;
      }
    }
    memcpy(res + totsize, buffer, read_bytes);
    totsize += read_bytes;
  }

  res[totsize] = '\0';

  // 掃除
  clean_up(ctx, in, out, gpgfile, gpgpath);
  return res;
}
