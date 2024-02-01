#include <stdio.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <gpgme.h>

#include "base32.h"
#include "otppass.h"

unsigned char* extract_secret(const char* otpauth_url, size_t* decoded_len) {
  const char* secret_start = strstr(otpauth_url, "secret=");
  if (!secret_start) {
    fprintf(stderr, "OTPAuth URLの中に、シークレットを見つけられませんでした。\n");
    return NULL;
  }
  secret_start += 7;

  const char* secret_end = strchr(secret_start, '&');
  if (!secret_end) {
    if (secret_start[0] != '\0') {
      secret_end = secret_start + strlen(secret_start);
    } else {
      secret_end = secret_start;
    }
  }

  if (secret_end < secret_start) {
    fprintf(stderr, "不正なシークレットの距離。\n");
    return NULL;
  }

  size_t secret_len = secret_end - secret_start;
  char* secret_encoded = (char*)malloc(secret_len + 1);

  if (secret_encoded == NULL) {
    fprintf(stderr, "メモリの役割に失敗。\n");
    return NULL;
  }

  strncpy(secret_encoded, secret_start, secret_len);
  secret_encoded[secret_len] = '\0';

  unsigned char* secret_decoded = base32_decode(secret_encoded, decoded_len);
  free(secret_encoded);

  if (!secret_decoded) {
    fprintf(stderr, "BASE32の復号化に失敗。\n");
    return NULL;
  }

  return secret_decoded;
}

uint32_t generate_totp(const char *secret, uint64_t counter) {
  counter = htobe64(counter);

  unsigned char hash[SHA_DIGEST_LENGTH];
  HMAC(EVP_sha1(), secret, strlen(secret), (unsigned char *)&counter, sizeof(counter), hash, NULL);

  int offset = hash[SHA_DIGEST_LENGTH - 1] & 0x0F;
  uint32_t truncated_hash =
    (hash[offset] & 0x7F)     << 24 |
    (hash[offset + 1] & 0xFF) << 16 |
    (hash[offset + 2] & 0xFF) << 8 |
    (hash[offset + 3] & 0xFF);

  return truncated_hash % 1000000;
}

void otppass(char* file) {
  gpgme_ctx_t ctx;
  gpgme_error_t err;
  gpgme_data_t in, out;
  size_t secret_len;

  gpgme_check_version(NULL);
  err = gpgme_new(&ctx);
  if (err) {
    fprintf(stderr, "GPGMEを創作に失敗\n");
    exit(1);
  }

  err = gpgme_data_new_from_file(&in, file, 1);
  if (err) {
    fprintf(stderr, "GPGファイルを読込に失敗\n");
    exit(1);
  }

  err = gpgme_data_new(&out);
  if (err) {
    fprintf(stderr, "GPGデータを読込に失敗\n");
    exit(1);
  }

  err = gpgme_op_decrypt(ctx, in, out);
  if (err) {
    fprintf(stderr, "GPGを復号化に失敗\n");
    exit(1);
  }

  char* secret = gpgme_data_release_and_get_mem(out, &secret_len);
  if (!secret) {
    fprintf(stderr, "GPGを受取に失敗\n");
    exit(1);
  }

  size_t decoded_len;
  unsigned char* secret_decoded = extract_secret(secret, &decoded_len);
  if (!secret_decoded) {
    fprintf(stderr, "シークレットの抽出またはデコードに失敗しました\n");
    free(secret);
    exit(1);
  }

  time_t current_time = time(NULL);
  uint64_t counter = current_time / 30;
  uint32_t otp = generate_totp((const char*)secret_decoded, counter);
  printf("%06d\n", otp);

  gpgme_data_release(in);
  gpgme_release(ctx);
  free(secret);
  free(secret_decoded);
}
