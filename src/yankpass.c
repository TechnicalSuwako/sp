#include <locale.h>
#include <unistd.h>

#include "common.h"
#include "yankpass.h"
#include "showpass.h"

void yankpass(char *file, int copyTimeout) {
  if (copyTimeout > 300) copyTimeout = 300;
  char *lang = getlang();

  int isGay = (getenv("WAYLAND_DISPLAY") != NULL);

  // Xセッションではない場合（例えば、SSH、TTY等）、showpass()を実行して
  if (getenv("DISPLAY") == NULL && getenv("WAYLAND_DISPLAY") == NULL) {
    if (strncmp(lang, "en", 2) == 0)
      puts("There is no X or Wayland session, so running 'sp -s'.");
    else puts("X又はWaylandセッションではありませんので、「sp -s」を実行します。");
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

  // CTRL + Cを押す場合
  signal(SIGINT, handle_sigint);

  // OpenPGPプロトコールを設定
  gpgme_set_protocol(ctx, GPGME_PROTOCOL_OpenPGP);

  // 暗号化したタイルを開く
  char *basedir = getbasedir(1);
  char* ext = ".gpg";
  int alllen = snprintf(NULL, 0, "%s%s%s", basedir, file, ext) + 1;
  char* gpgpath = malloc(alllen);
  if (gpgpath == NULL) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Failed to allocate memory");
    else perror("メモリを割当に失敗");
    return;
  }

  snprintf(gpgpath, alllen, "%s%s%s", basedir, file, ext);
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

  // xclip又はwl-copyを準備して
  char *cmd;
  if (isGay) cmd = "wl-copy";
  else cmd = "xclip -selection clipboard";

  FILE *pipe = popen(cmd, "w");
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

  // 何（デフォルトは45）秒後、クリップボードから削除する
  if (strncmp(lang, "en", 2) == 0)
    printf(
      "%s\n%s%d%s\n",
      "Added password to the clipboard.",
      "After ",
      copyTimeout,
      " seconds it'll be deleted from the clipboard."
    );
  else
    printf(
      "%s\n%d%s\n",
      "パスワードをクリップボードに追加しました。",
      copyTimeout,
      "秒後はクリップボードから取り消されます。"
    );
  sleep(copyTimeout);
  if (isGay) system("echo -n \"\" | wl-copy");
  else system("echo -n \"\" | xclip -selection clipboard");

  // 掃除
  fclose(gpgfile);
  free(gpgpath);
  gpgme_data_release(in);
  gpgme_data_release(out);
  gpgme_release(ctx);
}
