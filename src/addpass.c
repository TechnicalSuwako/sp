#include "addpass.h"

void cleanup(gpgme_ctx_t ctx, gpgme_key_t key, gpgme_data_t in, gpgme_data_t out) {
  gpgme_data_release(in);
  gpgme_data_release(out);
  gpgme_release(ctx);
  gpgme_key_release(key);
}

void getpasswd(char *prompt, char *pw, size_t pwlen) {
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

int addpass(char *file) {
  char *lang = getlang();

  // パスを準備
  char *basedir = getbasedir(1);
  char *ext = ".gpg";

  char pass[256];
  char knin[256];

  int alllen = snprintf(NULL, 0, "%s%s%s", basedir, file, ext) + 1;
  char *gpgpathchk = malloc(alllen);
  if (gpgpathchk == NULL) {
    const char *ero = (strncmp(lang, "en", 2) == 0 ?
        "Failed to allocate memory" : "メモリを割当に失敗");
    fprintf(stderr, "%s\n", ero);
    return -1;
  }

  // ファイルが既に存在するかどうか確認
  snprintf(gpgpathchk, alllen, "%s%s%s", basedir, file, ext);

  if (access(gpgpathchk, F_OK) != -1) {
    if (strncmp(lang, "en", 2) == 0)
      fprintf(
        stderr,
        "Password already exist.\nTo edit, please run ' sp -e %s '.\n",
        file
      );
    else
      fprintf(
        stderr,
        "%s\n変更するには、「 sp -e %s 」を実行して下さい。\n",
        "パスワードが既に存在しています。",
        file
      );
    free(gpgpathchk);
    return -1;
  }
  free(gpgpathchk);

  // パスワードを受け取る
  if (strncmp(lang, "en", 2) == 0)
    getpasswd("Password: ", pass, sizeof(pass));
  else getpasswd("パスワード： ", pass, sizeof(pass));
  puts("");
  if (strncmp(lang, "en", 2) == 0)
    getpasswd("Password (for verification): ", knin, sizeof(knin));
  else getpasswd("パスワード（確認用）： ", knin, sizeof(knin));
  puts("");

  // パスワードが一致するかどうか確認
  if (strcmp(pass, knin) != 0) {
    const char *ero = (strncmp(lang, "en", 2) == 0 ?
        "Password does not match. Terminating..." :
        "パスワードが一致していません。終了…");
    fprintf(stderr, "%s\n", ero);
    return -1;
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
    const char *ero = (strncmp(lang, "en", 2) == 0 ?
        "Failed to generate GPGME" : "GPGMEを創作に失敗");
    fprintf(stderr, "%s: %s\n", ero, gpgme_strerror(err));
    return -1;
  }

  // GPGMEは非対話的モードに設定
  err = gpgme_set_pinentry_mode(ctx, GPGME_PINENTRY_MODE_LOOPBACK);
  if (err) {
    const char *ero = (strncmp(lang, "en", 2) == 0 ?
        "Failed to set pinentry mode" : "pinentryモードを設定に失敗");
    fprintf(stderr, "%s: %s\n", ero, gpgme_strerror(err));
    gpgme_release(ctx);
    return -1;
  }

  // パスワードからデータオブジェクトを創作
  err = gpgme_data_new_from_mem(&in, pass, strlen(pass), 0);
  if (err) {
    const char *ero = (strncmp(lang, "en", 2) == 0 ?
        "Failed to make data object" : "データオブジェクトを創作に失敗");
    fprintf(stderr, "%s: %s\n", ero, gpgme_strerror(err));
    gpgme_release(ctx);
    return -1;
  }

  gpgme_data_new(&out);

  // 鍵を受け取る
  char keypath[256];
  snprintf(keypath, sizeof(keypath), "%s%s", basedir, ".gpg-id");

  keypath[sizeof(keypath) - 1] = '\0';

  FILE* keyfile = fopen(keypath, "rb");
  if (keyfile == NULL) {
    const char *ero = (strncmp(lang, "en", 2) == 0 ?
        "Failed to open .gpg-id file" : ".gpg-idファイルを開くに失敗");
    fprintf(stderr, "%s\n", ero);
    return -1;
  }

  char *keyid = malloc(256);
  if (!fgets(keyid, 256, keyfile)) {
    const char *ero = (strncmp(lang, "en", 2) == 0 ?
        "Failed to reading key ID" : "鍵IDを読込に失敗");
    fprintf(stderr, "%s\n", ero);
    fclose(keyfile);
    free(keyid);
    return -1;
  }

  keyid[strcspn(keyid, "\n")] = 0;
  fclose(keyfile);

  err = gpgme_get_key(ctx, keyid, &key[0], 0);
  if (err) {
    const char *ero = (strncmp(lang, "en", 2) == 0 ?
        "Failed to get key" : "鍵Dを受取に失敗");
    fprintf(stderr, "%s: %s\n", ero, gpgme_strerror(err));
    free(keyid);
    return -1;
  }

  if (key[0] == NULL) {
    const char *ero = (strncmp(lang, "en", 2) == 0 ?
        "Error: Key is NULL" : "エラー：鍵はNULLです");
    fprintf(stderr, "%s\n", ero);
    free(keyid);
    return -1;
  }

  free(keyid);

  // 暗号化
  err = gpgme_op_encrypt(ctx, &key[0], GPGME_ENCRYPT_ALWAYS_TRUST, in, out);
  if (err) {
    const char *ero = (strncmp(lang, "en", 2) == 0 ?
        "Failed to encrypt" : "暗号化に失敗");
    fprintf(stderr, "%s: %s\n", ero, gpgme_strerror(err));
    cleanup(ctx, key[0], in, out);
    return -1;
  }

  // 暗号化したファイルを開く
  char *gpgpath = malloc(alllen);
  if (gpgpath == NULL) {
    const char *ero = (strncmp(lang, "en", 2) == 0 ?
        "Failed to allocate memory" : "メモリを割当に失敗");
    fprintf(stderr, "%s\n", ero);
    cleanup(ctx, key[0], in, out);
    return -1;
  }

  // ディレクトリを創作
  char dirpath[512];
  snprintf(dirpath, sizeof(dirpath), "%s%s", basedir, file);

  dirpath[sizeof(dirpath) - 1] = '\0';

  char *lastsla = strrchr(dirpath, '/');
  if (lastsla != NULL) {
    *lastsla = '\0';
    if (mkdir_r(dirpath, 0755) != 0) {
      const char *ero = (strncmp(lang, "en", 2) == 0 ?
          "Failed to create directory" : "ディレクトリを創作に失敗");
      fprintf(stderr, "%s\n", ero);
      free(gpgpath);
      cleanup(ctx, key[0], in, out);
      return -1;
    }
  }

  snprintf(gpgpath, alllen, "%s%s%s", basedir, file, ext);

  struct stat statbuf;
  if (stat(gpgpath, &statbuf) == 0) {
    const char *ero = (strncmp(lang, "en", 2) == 0 ?
        "Password already exists" : "パスワードは既に存在しています");
    fprintf(stderr, "%s\n", ero);
    free(gpgpath);
    cleanup(ctx, key[0], in, out);
    return -1;
  }

  gpgfile = fopen(gpgpath, "wb");
  if (gpgfile == NULL) {
    const char *ero = (strncmp(lang, "en", 2) == 0 ?
        "Failed to open file." : "ファイルを開くに失敗。");
    fprintf(stderr, "%s\n", ero);
    free(gpgpath);
    cleanup(ctx, key[0], in, out);
    return -1;
  }

  // データが保存したかどうか確認
  ssize_t encrypted_data_size = gpgme_data_seek(out, 0, SEEK_END);
  if (encrypted_data_size <= 0) {
    const char *ero = (strncmp(lang, "en", 2) == 0 ?
        "Failed to store the data" : "データを保存に失敗");
    fprintf(stderr, "%s\n", ero);
    fclose(gpgfile);
    free(gpgpath);
    cleanup(ctx, key[0], in, out);
    return -1;
  }

  // 復号化したパスワードを表示する
  gpgme_data_seek(out, 0, SEEK_SET);

  char buffer[512];
  ssize_t read_bytes;

  while ((read_bytes = gpgme_data_read(out, buffer, sizeof(buffer))) > 0) {
    if (fwrite(buffer, 1, (size_t)read_bytes, gpgfile) != (size_t)read_bytes) {
      const char *ero = (strncmp(lang, "en", 2) == 0 ?
          "Failed to write password" : "パスワードを書き込みに失敗");
      fprintf(stderr, "%s\n", ero);
      free(gpgpath);
      cleanup(ctx, key[0], in, out);
      return -1;
    }
  }

  // 掃除
  fclose(gpgfile);
  free(gpgpath);
  cleanup(ctx, key[0], in, out);

  const char *msg = (strncmp(lang, "en", 2) == 0 ?
      "The password got saved." : "パスワードを保存出来ました。");
  printf("%s\n", msg);

  return 0;
}
