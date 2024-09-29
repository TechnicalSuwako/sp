#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <dirent.h>

#include "vulnpass.h"
#include "showpass.h"
#include "common.h"

List vulnFullpaths;
List vulnDispaths;

void vulnpass(const char *dpath) {
  char *lang = getlang();

  // pwndサーバに接続
  int sock;
  struct sockaddr_in srv;
  struct addrinfo hints, *addr;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  int status = getaddrinfo("076.moe", NULL, &hints, &addr);
  if (status != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    return;
  }

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    const char *ero = strncmp(lang, "en", 2) == 0 ?
      "Failed to create socket" : "ソケットを作成に失敗";
    perror(ero);
    return;
  }

  srv.sin_addr = ((struct sockaddr_in *)(addr->ai_addr))->sin_addr;
  srv.sin_family = AF_INET;
  srv.sin_port = htons(9951);

  freeaddrinfo(addr);

  if (connect(sock, (struct sockaddr *)&srv, sizeof(srv)) < 0) {
    const char *ero = strncmp(lang, "en", 2) == 0 ? "Failed to connect" : "接続に失敗";
    perror(ero);
    close(sock);
    return;
  }

  // パスワードをスキャンして
  List fpaths;
  initList(&fpaths);
  initList(&vulnFullpaths);
  initList(&vulnDispaths);
  scanDir(dpath, dpath, &fpaths, &vulnFullpaths, &vulnDispaths);
  int vulncount = 0;

  puts(strncmp(lang, "en", 2) == 0 ?
      "Checking, please wait for a while..." : "確認中。暫くお待ち下さい・・・");

  for (size_t i = 0; i < vulnDispaths.size; i++) {
    const char *pass = showpass(getElement(&vulnDispaths, i));
    if (!pass) continue;
    char res[256];
    int reslen = 0;

    if (send(sock, pass, strlen(pass), 0) < 0) {
      const char *ero = strncmp(lang, "en", 2) == 0 ?
        "Failed to send" : "送信に失敗";
      perror(ero);
      close(sock);
      freeList(&fpaths);
      return;
    }

    reslen = recv(sock, res, sizeof(res) -1, 0);
    if (reslen < 0) {
      const char *ero = strncmp(lang, "en", 2) == 0 ?
        "Failed to retreive" : "受取に失敗";
      perror(ero);
      close(sock);
      freeList(&fpaths);
      return;
    }

    res[reslen] = '\0';

    if (strncmp(res, "0", 1) != 0) {
      printf("【！】");
      if (strncmp(lang, "en", 2) == 0) {
        printf("The password \"%s\" has been breached %d times.\n",
            getElement(&vulnDispaths, i), atoi(res));
      } else {
        printf("パスワード「%s」は%d回に漏洩されました。\n",
            getElement(&vulnDispaths, i), atoi(res));
      }

      vulncount++;
    }

    close(sock);

    // 再接続
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
      const char *ero = strncmp(lang, "en", 2) == 0 ?
        "Failed to create socket" : "ソケットを作成に失敗";
      perror(ero);
      return;
    }

    if (connect(sock, (struct sockaddr *)&srv, sizeof(srv)) < 0) {
      const char *ero = strncmp(lang, "en", 2) == 0 ?
        "Failed to reconnect" : "再接続に失敗";
      perror(ero);
      close(sock);
      return;
    }
  }

  if (strncmp(lang, "en", 2) == 0) {
    printf("\nTotal breached: %d\n", vulncount);
    if (vulncount > 0)
      printf("It's advised to change any of the breached "
             "passwords as soon as possible!\n");
  } else {
    printf("\n漏洩したパスワードの合計: %d\n", vulncount);
    if (vulncount > 0)
      printf("漏洩したパスワードは出来るだけ早く変更する事をお勧めします！\n");
  }

  freeList(&fpaths);
}
