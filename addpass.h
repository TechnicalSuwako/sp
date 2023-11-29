#ifndef ADDPASS_H
#define ADDPASS_H

#include <sys/stat.h>

int mkdir_r(const char *path, mode_t mode);
void addpass(char* file);

#endif
