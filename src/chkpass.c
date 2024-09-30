#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

#include "chkpass.h"
#include "showpass.h"
#include "common.h"

List chkFullpaths;
List chkDispaths;
List dupPasswd;
List dupPath;

int vulncount = 0;
int weaklencount = 0;
int weakcharcount = 0;
int duppasscount = 0;

size_t minimumlen = 12;
size_t recommendlen = 64;

const char *spchar = "!@#$%^&*()-_=+[]{}|;:'\",.<>?/\\`~";

void chkLenPass(const char *path, const char *pass, char *lang) {
  if (strlen(pass) > 0 && strlen(pass) < minimumlen) {
    printf("【E】");
    if (strncmp(lang, "en", 2) == 0) {
      printf("The password \"%s\" is too short, minimum length should be "
          "%ld characters, recommended is %ld characters.\n",
          path, minimumlen, recommendlen);
    } else {
      printf("パスワード「%s」は短すぎます。最短パスワードの長さは%ld文字ですが、"
          "勧めが%ld文字です。\n",
          path, minimumlen, recommendlen);
    }

    weaklencount++;
    vulncount++;
  } else if (strlen(pass) >= minimumlen && strlen(pass) < recommendlen) {
    printf("【W】");
    if (strncmp(lang, "en", 2) == 0) {
      printf("The password \"%s\" is long enough, but for optimal security,"
          "%ld characters is recommended.\n",
          path, recommendlen);
    } else {
      printf("パスワード「%s」の長さは十分ですが、最強のセキュリティには%ld文字が"
          "勧めします。\n",
          path, recommendlen);
    }
  }
}

void chkCharPass(const char *path, const char *pass, char *lang) {
  int isUpper = 0;
  int isLower = 0;
  int isDigit = 0;
  int isSpecial = 0;

  for (size_t j = 0; j < strlen(pass); j++) {
    if (isupper(pass[j])) isUpper = 1;
    else if (islower(pass[j])) isLower = 1;
    else if (isdigit(pass[j])) isDigit = 1;
    else if (strchr(spchar, pass[j])) isSpecial = 1;
  }

  if (isUpper == 0 || isLower == 0 || isDigit == 0 || isSpecial == 0) {
    printf("【E】");
    if (strncmp(lang, "en", 2) == 0) {
      printf("The password \"%s\" is too weak! A strong password contains at least "
          "1 uppercase, 1 lowercase, 1 digit, and 1 special character.\n",
          path);
    } else {
      printf("パスワード「%s」は弱すぎます！強いパスワードは最大1大文字、1小文字、"
          "1数字、及び1特別な文字が含みます。\n",
          path);
    }

    weakcharcount++;
    vulncount++;
  }
}

void chkDupPass(const char *path, const char *pass, char *lang) {
  for (size_t k = 0; k < dupPasswd.size; k++) {
    if (strncmp(getElement(&dupPasswd, k), pass, strlen(pass)) == 0) {
      printf("【E】");
      if (strncmp(lang, "en", 2) == 0) {
        printf("The password \"%s\" is the same as \"%s\". "
            "For security, please keep passwords unique!\n",
            path, getElement(&dupPath, k));
      } else {
        printf("パスワード「%s」は「%s」と一致しています。"
            "セキュリティの為、各パスワードはユニークにする様にして下さい！\n",
            path, getElement(&dupPath, k));
      }

      duppasscount++;
      vulncount++;
    }
  }

  addElement(&dupPath, path);
  addElement(&dupPasswd, pass);
}

void chkpass(const char *dpath, const char *mode) {
  char *lang = getlang();

  if (strncmp(mode, "all", 3) != 0 && strncmp(mode, "length", 6) != 0 &&
      strncmp(mode, "strength", 8) != 0 && strncmp(mode, "duplicate", 9) != 0) {
    if (strncmp(lang, "en", 2) == 0)
      printf("Invalid mode. Possible modes: all, length, strength, duplicate\n");
    else
      printf("不正なモード。可能なモード： all（全部）、length（長さ）、"
          "strength（強さ）、duplicate（同じパスワードの確認）\n");
    return;
  }

  // パスワードをスキャンして
  List fpaths;
  initList(&fpaths);
  initList(&chkFullpaths);
  initList(&chkDispaths);
  initList(&dupPasswd);
  initList(&dupPath);
  scanDir(dpath, dpath, &fpaths, &chkFullpaths, &chkDispaths);

  puts(strncmp(lang, "en", 2) == 0 ?
      "Checking, please wait for a while..." : "確認中。暫くお待ち下さい・・・");

  for (size_t i = 0; i < chkDispaths.size; i++) {
    const char *pass = showpass(getElement(&chkDispaths, i));
    if (!pass) continue;
    if (strstr(pass, "otpauth://totp/")) continue;

    if (strncmp(mode, "all", 3) == 0) {
      // 全部を確認する
      chkLenPass(getElement(&chkDispaths, i), pass, lang);
      chkCharPass(getElement(&chkDispaths, i), pass, lang);
      chkDupPass(getElement(&chkDispaths, i), pass, lang);
    } else if (strncmp(mode, "length", 6) == 0) {
      // 長さの確認
      chkLenPass(getElement(&chkDispaths, i), pass, lang);
    } else if (strncmp(mode, "strength", 8) == 0) {
      // 強さの確認
      chkCharPass(getElement(&chkDispaths, i), pass, lang);
    } else if (strncmp(mode, "duplicate", 9) == 0) {
      // 複数回同じパスワードの確認
      chkDupPass(getElement(&chkDispaths, i), pass, lang);
    } else {
    }
  }

  if (strncmp(lang, "en", 2) == 0) {
    if (strncmp(mode, "all", 3) == 0 || strncmp(mode, "length", 6) == 0)
      printf("\nShort password count: %d\n", weaklencount);
    if (strncmp(mode, "all", 3) == 0 || strncmp(mode, "strength", 8) == 0)
      printf("\nWeak password count: %d\n", weakcharcount);
    if (strncmp(mode, "all", 3) == 0 || strncmp(mode, "duplicate", 9) == 0)
      printf("\nDuplicate password count: %d\n", duppasscount);

    printf("\nTotal weak passwords: %d\n", vulncount);
    if (vulncount > 0)
      printf("It's advised to change any of the weak "
             "passwords as soon as possible!\n");
  } else {
    if (strncmp(mode, "all", 3) == 0 || strncmp(mode, "length", 6) == 0)
      printf("\n短いパスワード数: %d\n", weaklencount);
    if (strncmp(mode, "all", 3) == 0 || strncmp(mode, "strength", 8) == 0)
      printf("\n弱いパスワード数: %d\n", weakcharcount);
    if (strncmp(mode, "all", 3) == 0 || strncmp(mode, "duplicate", 9) == 0)
      printf("\n同じパスワード数: %d\n", duppasscount);

    printf("\n不安のパスワードの合計: %d\n", vulncount);
    if (vulncount > 0)
      printf("不安的なパスワードは出来るだけ早く変更する事をお勧めします！\n");
  }

  freeList(&fpaths);
  freeList(&dupPath);
  freeList(&dupPasswd);
}
