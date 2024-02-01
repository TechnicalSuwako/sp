#include "base32.h"

int char_to_val(char c) {
  const char *base32_alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
  char *ptr = strchr(base32_alphabet, c);
  return ptr ? ptr - base32_alphabet : -1;
}

unsigned char *base32_decode(const char *encoded, size_t *out_len) {
  size_t encoded_len = strlen(encoded);
  size_t padding = 0;
  for (int i = encoded_len - 1; i >= 0 && encoded[i] == '='; --i) {
    ++padding;
  }

  *out_len = (encoded_len - padding) * 5 / 8;
  if (*out_len == 0) return NULL;

  unsigned char *decoded = malloc(*out_len);
  if (!decoded) return NULL;

  int buffer = 0, bits_left = 0, count = 0;
  for (size_t i = 0; i < encoded_len - padding; ++i) {
    int val = char_to_val(encoded[i]);
    if (val < 0) {
      free(decoded);
      return NULL;
    }

    buffer <<= 5;
    buffer |= val;
    bits_left += 5;

    if (bits_left >= 8) {
      decoded[count++] = (unsigned char) (buffer >> (bits_left - 8));
      bits_left -= 8;
    }
  }

  *out_len = count;
  return decoded;
}
