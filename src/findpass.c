#include "common.h"
#include "findpass.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

List findFullpaths;
List findDispaths;

void findpass(const char *dpath, const char *txt) {
  List fpaths;
  initList(&fpaths);
  initList(&findFullpaths);
  initList(&findDispaths);
  scanDir(dpath, dpath, &fpaths, &findFullpaths, &findDispaths);

  for (size_t i = 0; i < findDispaths.size; i++) {
    if (strcasestr(getElement(&findDispaths, i), txt) != NULL) {
      printf("%s\n", getElement(&findDispaths, i));
    }
  }

  freeList(&fpaths);
}
