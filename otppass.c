#include <stdio.h>
#include <string.h>
#include <time.h>
#include <openssl/hmac.h>

unsigned long generate_totp(const unsigned char* secret, size_t keylen) {
  unsigned long timestep = time(NULL) / 30;
  unsigned char hmacres[20];

  HMAC(EVP_sha1(), secret, keylen, (unsigned char*)&timestep, sizeof(timestep), hmacres, NULL);

  int offset = hmacres[19] & 0xF;
  unsigned long trunhash = (hmacres[offset] & 0x7F) << 24 |
                           (hmacres[offset + 1] & 0xFF) << 16 |
                           (hmacres[offset + 2] & 0xFF) << 8 |
                           (hmacres[offset + 3] & 0xFF);
  
  unsigned long otp = trunhash % 1000000;
  return otp;
}

void otppass(char* file) {
  const char* secret = file;
  unsigned long otp = generate_totp((unsigned char*)secret, strlen(secret));
  printf("%06lu\n", otp);
}
