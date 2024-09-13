#include "common.h"
#include <string.h>

char *getlang() {
  char *lang = NULL;

  lang = getenv("SP_LANG");
  if (lang == NULL) lang = "ja";

  return lang;
}

int mkdir_r(const char *path, mode_t mode) {
  char tmp[256];
  char *p = NULL;
  size_t len;

  snprintf(tmp, sizeof(tmp), "%s", path);

  len = strlen(tmp);
  if (tmp[len - 1] == '/') {
    tmp[len - 1] = 0; // 最後の「/」文字を取り消す
  }

  for (p = tmp + 1; *p; p++) {
    if (*p == '/') {
      *p = 0; // 最後の「/」文字を取り消す
      if (mkdir(tmp, mode) != 0 && errno != EEXIST) return -1;
      *p = '/'; // また追加
    }
  }

  if (mkdir(tmp, mode) != 0 && errno != EEXIST) {
    return -1;
  }

  return 0;
}

void initList(List *list) {
  list->head = NULL;
  list->tail = NULL;
  list->size = 0;
}

void addElement(List *list, const char *data) {
  Node *newNode = (Node *)malloc(sizeof(Node));
  if (!newNode) {
    return;
  }

  newNode->data = strdup(data);
  newNode->next = NULL;

  if (list->tail) {
    list->tail->next = newNode;
  } else {
    list->head = newNode;
  }

  list->tail = newNode;
  list->size++;
}

char *getElement(List *list, size_t idx) {
  if (idx >= list->size) return NULL;

  Node *current = list->head;
  for (size_t i = 0; i < idx; i++)
    current = current->next;

  return current->data;
}

void rmElement(List *list, size_t idx) {
  if (idx >= list->size) return;

  Node *current = list->head;
  Node *previous = NULL;

  if (idx == 0) {
    list->head = current->next;
    if (list->size == 1) list->tail = NULL;
  } else {
    for (size_t i = 0; i < idx; i++) {
      previous = current;
      current = current->next;
    }
    previous->next = current->next;
    if (idx == list->size - 1) {
      list->tail = previous;
    }
  }

  free(current->data);
  free(current);
  list->size--;
}

void freeList(List *list) {
  Node *current = list->head;

  while (current) {
    Node *next = current->next;
    free(current->data);
    free(current);
    current = next;
  }

  list->head = NULL;
  list->tail = NULL;
  list->size = 0;
}
