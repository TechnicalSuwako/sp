#ifndef CHKPASS_H
#define CHKPASS_H

#include <stdlib.h>

void chkpass(const char *dpath, const char *mode);

extern int vulncount;
extern int weaklencount;
extern int weakcharcount;
extern int duppasscount;

extern size_t minimumlen;
extern size_t recommendlen;

extern const char *spchar;

#endif
