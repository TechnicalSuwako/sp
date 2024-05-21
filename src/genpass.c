#include "common.h"
#include "genpass.h"

void genpass(int count, bool issecure) {
  char *lang = getlang();

  const char *charset_risky =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  const char *charset_secure =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!\"#$%&'()=~-^\\|_@`[{]};:+*<>,./?";
  const char *charset = issecure ? charset_secure : charset_risky;

  FILE *fp = fopen("/dev/random", "rb");
  if (fp == NULL) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Could not opening /dev/random");
    else perror("/dev/randomを開けられませんでした");
    exit(EXIT_FAILURE);
  }

  char password[count + 1];
  for (int i = 0; i < count; i++) {
    unsigned char key;
    fread(&key, sizeof(key), 1, fp);
    password[i] = charset[key % strlen(charset)];
  }

  password[count] = '\0';
  fclose(fp);

  printf("%s\n", password);
}
