#include <openssl/hmac.h>
#include <openssl/sha.h>

#include "base32.h"
#include "common.h"
#include "otppass.h"

unsigned char *extract_secret(const char *otpauth_url, size_t *decoded_len) {
  char *lang = getlang();

  const char *secret_start = strstr(otpauth_url, "secret=");
  if (!secret_start) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Failed to find secret in the OTPAuth URL");
    else perror("OTPAuth URLの中に、シークレットを見つけられませんでした");
    return NULL;
  }
  secret_start += 7;

  const char *secret_end = strchr(secret_start, '&');
  if (!secret_end) {
    if (secret_start[0] != '\0') {
      secret_end = secret_start + strlen(secret_start);
    } else {
      secret_end = secret_start;
    }
  }

  if (secret_end < secret_start) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Incorrect secret range");
    else perror("不正なシークレットの距離");
    return NULL;
  }

  size_t secret_len = secret_end - secret_start;
  char *secret_encoded = (char *)malloc(secret_len + 1);

  if (secret_encoded == NULL) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Failed to allocate memory");
    else perror("メモリの役割に失敗");
    return NULL;
  }

  strncpy(secret_encoded, secret_start, secret_len);
  secret_encoded[secret_len] = '\0';

  unsigned char *secret_decoded = base32_decode(secret_encoded, decoded_len);
  free(secret_encoded);

  if (!secret_decoded) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Failed to decrypt BASE32");
    else perror("BASE32の復号化に失敗");
    return NULL;
  }

  return secret_decoded;
}

#if defined(__HAIKU__)
uint64_t htobe64(uint64_t counter) {
  uint64_t res = 0;
  uint8_t *dest = (uint8_t *)&res;
  uint8_t *src = (uint8_t *)&counter;

  dest[0] = src[7];
  dest[1] = src[6];
  dest[2] = src[5];
  dest[3] = src[4];
  dest[4] = src[3];
  dest[5] = src[2];
  dest[6] = src[1];
  dest[7] = src[0];

  return res;
}
#endif

uint32_t generate_totp(const char *secret, uint64_t counter) {
  counter = htobe64(counter);

  unsigned char hash[SHA_DIGEST_LENGTH];
  HMAC(
    EVP_sha1(),
    secret,
    strlen(secret),
    (unsigned char *)&counter,
    sizeof(counter),
    hash,
    NULL
  );

  int offset = hash[SHA_DIGEST_LENGTH - 1] & 0x0F;
  uint32_t truncated_hash =
    (hash[offset] & 0x7F)     << 24 |
    (hash[offset + 1] & 0xFF) << 16 |
    (hash[offset + 2] & 0xFF) << 8 |
    (hash[offset + 3] & 0xFF);

  return truncated_hash % 1000000;
}

void otppass(char *file) {
  char *lang = getlang();

  gpgme_ctx_t ctx;
  gpgme_error_t err;
  gpgme_data_t in, out;
  size_t secret_len;

  gpgme_check_version(NULL);
  err = gpgme_new(&ctx);
  if (err) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Failed to generate the GPG");  
    else perror("GPGを創作に失敗"); 
    exit(1);
  }

  err = gpgme_data_new_from_file(&in, file, 1);
  if (err) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Failed to read the GPG file");
    else perror("GPGファイルを読込に失敗");
    exit(1);
  }

  err = gpgme_data_new(&out);
  if (err) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Failed to read the GPG data");
    else perror("GPGデータを読込に失敗");
    exit(1);
  }

  err = gpgme_op_decrypt(ctx, in, out);
  if (err) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Failed to decrypt the GPG");
    else perror("GPGを復号化に失敗");
    exit(1);
  }

  char *secret = gpgme_data_release_and_get_mem(out, &secret_len);
  if (!secret) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Failed to get the GPG");
    else perror("GPGを受取に失敗");
    exit(1);
  }

  secret[secret_len] = '\0';

  size_t decoded_len;
  unsigned char *secret_decoded = extract_secret(secret, &decoded_len);
  if (!secret_decoded) {
    if (strncmp(lang, "en", 2) == 0)
      perror("Failed to decode or export secret");
    else perror("シークレットの抽出又はデコードに失敗しました");
    free(secret);
    exit(1);
  }

  secret_decoded[decoded_len] = '\0';

  time_t current_time = time(NULL);
  uint64_t counter = current_time / 30;
  uint32_t otp = generate_totp((const char *)secret_decoded, counter);

  gpgme_data_release(in);
  gpgme_release(ctx);
  gpgme_free(secret);
  free(secret_decoded);

  printf("%06d\n", otp);
}
