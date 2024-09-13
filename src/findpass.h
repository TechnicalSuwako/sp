#ifndef FINDPASS_H
#define FINDPASS_H

#include "common.h"

void scanDir(const char *dpath, const char *rpath, List *fpaths);
void findpass(const char *dpath, const char *txt);

#endif
