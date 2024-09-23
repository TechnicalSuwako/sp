#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>

#include <gpgme.h>

typedef struct Node {
  char *data;
  struct Node *next;
} Node;

typedef struct {
  Node *head;
  Node *tail;
  size_t size;
} List;

char *getbasedir(int trailing);
char *getlang();
int mkdir_r(const char *path, mode_t mode);
int tmpcopy(const char *inpath, const char *outpath);

// C言語のvector
void initList(List *list);
void addElement(List *list, const char *data);
char *getElement(List *list, size_t idx);
void rmElement(List *list, size_t idx);
void freeList(List *list);

#endif
