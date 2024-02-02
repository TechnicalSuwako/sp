#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <string.h>

#include <gpgme.h>

#include "yankpass.h"
#include "showpass.h"

void yankpass(char* file) {
  char *lang = getenv("SP_LANG");

  // Xセッションではない場合（例えば、SSH、TTY、Gayland等）、showpass()を実行して
  if (getenv("DISPLAY") == NULL) { 
    if (lang != NULL && strncmp(lang, "en", 2) == 0) puts("There is no X session, so executing 'sp -s'.");
    else puts("Xセッションではありませんので、「sp -s」を実行します。");
    showpass(file);
    return;
  }

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
    if (lang != NULL && strncmp(lang, "en", 2) == 0) fprintf(stderr, "Failed to generating GPGME: %s\n", gpgme_strerror(err));
    else fprintf(stderr, "GPGMEを創作に失敗：%s\n", gpgme_strerror(err));
    return;
  }

  // OpenPGPプロトコールを設定
  gpgme_set_protocol(ctx, GPGME_PROTOCOL_OpenPGP);

  // 暗号化したタイルを開く
  char* homedir = getenv("HOME");
  if (homedir == NULL) {
    if (lang != NULL && strncmp(lang, "en", 2) == 0) perror("Failed to getting home directory");
    else perror("ホームディレクトリを受取に失敗");
    return;
  }

  char* basedir = "/.local/share/sp/";
  char* ext = ".gpg";
  int alllen = snprintf(NULL, 0, "%s%s%s%s", homedir, basedir, file, ext) + 1;
  char* gpgpath = malloc(alllen);
  if (gpgpath == NULL) {
    if (lang != NULL && strncmp(lang, "en", 2) == 0) perror("Failed to allocating memory");
    else perror("メモリを割当に失敗");
    return;
  }

  snprintf(gpgpath, alllen, "%s%s%s%s", homedir, basedir, file, ext);
  gpgfile = fopen(gpgpath, "rb");
  if (gpgfile == NULL) {
    if (lang != NULL && strncmp(lang, "en", 2) == 0) {
      perror("Failed to opening the file");
      fprintf(stderr, "Failed path: %s\n", gpgpath);
    } else {
      perror("ファイルを開くに失敗");
      fprintf(stderr, "失敗したパス： %s\n", gpgpath);
    }
    free(gpgpath);
    return;
  }
  gpgme_data_new_from_stream(&in, gpgfile);

  // データオブジェクトを創作
  gpgme_data_new(&out);

  // 復号化して
  err = gpgme_op_decrypt(ctx, in, out);
  if (err) {
    if (lang != NULL && strncmp(lang, "en", 2) == 0) fprintf(stderr, "Failed to decryption: %s\n", gpgme_strerror(err));
    else fprintf(stderr, "復号化に失敗： %s\n", gpgme_strerror(err));

    // 掃除
    fclose(gpgfile);
    free(gpgpath);
    gpgme_data_release(in);
    gpgme_data_release(out);
    gpgme_release(ctx);
    return;
  }

  // xclipを準備して
  FILE *pipe = popen("xclip -selection clipboard", "w");
  if (pipe == NULL) {

    // 掃除
    fclose(gpgfile);
    free(gpgpath);
    gpgme_data_release(in);
    gpgme_data_release(out);
    gpgme_release(ctx);
    if (lang != NULL && strncmp(lang, "en", 2) == 0) perror("Could not found a clipboard");
    else perror("クリップボードを見つけられませんでした");
    return;
  }

  // 復号化したパスワードを表示する
  gpgme_data_seek(out, 0, SEEK_SET);

  char buffer[512];
  ssize_t read_bytes;

  while ((read_bytes = gpgme_data_read(out, buffer, sizeof(buffer))) > 0) {
    if (buffer[read_bytes - 1] == '\n') {
        read_bytes--;
    }
    fwrite(buffer, 1, read_bytes, pipe);
  }

  pclose(pipe);

  // 45秒後、クリップボードから削除する
  if (lang != NULL && strncmp(lang, "en", 2) == 0) puts("Added password to the clipboard.\nI will take it away from the clipboard after 45 second");
  else puts("パスワードをクリップボードに追加しました。\n45秒後はクリップボードから取り消されます");
  sleep(45);
  system("echo -n | xclip -selection clipboard");

  // 掃除
  fclose(gpgfile);
  free(gpgpath);
  gpgme_data_release(in);
  gpgme_data_release(out);
  gpgme_release(ctx);
}
