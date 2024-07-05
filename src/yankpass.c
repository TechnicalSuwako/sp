#include <locale.h>
#include <unistd.h>

#include "common.h"
#include "yankpass.h"
#include "showpass.h"

void yankpass(char *file) {
  char *lang = getlang();

  // Xセッションではない場合（例えば、SSH、TTY、Gayland等）、showpass()を実行して
  if (getenv("DISPLAY") == NULL) { 
    if (strncmp(lang, "en", 2) == 0)
      puts("There is no X session, so running 'sp -s'.");
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
    if (strncmp(lang, "en", 2) == 0)
      fprintf(stderr, "Failed to generate GPGME: %s\n", gpgme_strerror(err));
    else fprintf(stderr, "GPGMEを創作に失敗：%s\n", gpgme_strerror(err));
    return;
  }

  // OpenPGPプロトコールを設定
  gpgme_set_protocol(ctx, GPGME_PROTOCOL_OpenPGP);

  // 暗号化したタイルを開く
  char* homedir = getenv("HOME");
  if (homedir == NULL) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Failed to get home directory");
    else perror("ホームディレクトリを受取に失敗");
    return;
  }

#if defined(__HAIKU__)
  char *basedir = "/config/settings/sp/"
#else
  char* basedir = "/.local/share/sp/";
#endif
  char* ext = ".gpg";
  int alllen = snprintf(NULL, 0, "%s%s%s%s", homedir, basedir, file, ext) + 1;
  char* gpgpath = malloc(alllen);
  if (gpgpath == NULL) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Failed to allocate memory");
    else perror("メモリを割当に失敗");
    return;
  }

  snprintf(gpgpath, alllen, "%s%s%s%s", homedir, basedir, file, ext);
  gpgfile = fopen(gpgpath, "rb");
  if (gpgfile == NULL) {
    if (strncmp(lang, "en", 2) == 0) {
      perror("Failed to open the file");
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
    if (strncmp(lang, "en", 2) == 0)
      fprintf(stderr, "Failed to decrypt: %s\n", gpgme_strerror(err));
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
    if (strncmp(lang, "en", 2) == 0)
      perror("Could not find a clipboard");
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
  if (strncmp(lang, "en", 2) == 0)
    printf(
      "%s\n%s\n",
      "Added password to the clipboard.",
      "After 45 seconds it'll be deleted from the clipboard."
    );
  else
    printf(
      "%s\n%s\n",
      "パスワードをクリップボードに追加しました。",
      "45秒後はクリップボードから取り消されます。"
    );
  sleep(45);
  system("echo -n | xclip -selection clipboard");

  // 掃除
  fclose(gpgfile);
  free(gpgpath);
  gpgme_data_release(in);
  gpgme_data_release(out);
  gpgme_release(ctx);
}
