#ifndef BASE32_H
#define BASE32_H

#include <stdlib.h>

unsigned char *base32_decode(const char *encoded, size_t *out_len);

#endif
