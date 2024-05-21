#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>

#include <gpgme.h>

char *getlang();
int mkdir_r(const char *path, mode_t mode);

#endif
