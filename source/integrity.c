#include "integrity.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

// We support two backends:
//  - 3DS / target build: mbedtls (already in portlibs)
//  - host build: small public-domain SHA-256 implementation, used only by
//    the unit tests so they don't need an extra dependency.
//
// The mbedtls path is preferred whenever available because it's the same
// implementation that ships with the 3DS firmware and curl on-device.

#if defined(HOST_BUILD) || !defined(__3DS__)

// ---- Pure-C SHA-256 (host) -----------------------------------------------
// Compact reference implementation. Not constant-time, but integrity
// verification of downloaded archives is not a side-channel scenario.

#include <stdint.h>

typedef struct {
    uint32_t state[8];
    uint64_t bitcount;
    uint8_t  buffer[64];
} sha256_ctx;

static const uint32_t K256[64] = {
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
    0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
    0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
    0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
    0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
    0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
    0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
    0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

#define ROTR(x,n) (((x) >> (n)) | ((x) << (32-(n))))

static void sha256_transform(sha256_ctx* c, const uint8_t* d) {
    uint32_t w[64], a, b, ce, dd, e, f, g, h, t1, t2;
    for (int i = 0; i < 16; i++) {
        w[i] = ((uint32_t)d[i*4] << 24) | ((uint32_t)d[i*4+1] << 16) |
               ((uint32_t)d[i*4+2] << 8) | (uint32_t)d[i*4+3];
    }
    for (int i = 16; i < 64; i++) {
        uint32_t s0 = ROTR(w[i-15],7) ^ ROTR(w[i-15],18) ^ (w[i-15] >> 3);
        uint32_t s1 = ROTR(w[i-2],17) ^ ROTR(w[i-2],19)  ^ (w[i-2] >> 10);
        w[i] = w[i-16] + s0 + w[i-7] + s1;
    }
    a = c->state[0]; b = c->state[1]; ce = c->state[2]; dd = c->state[3];
    e = c->state[4]; f = c->state[5]; g  = c->state[6]; h  = c->state[7];
    for (int i = 0; i < 64; i++) {
        uint32_t S1 = ROTR(e,6) ^ ROTR(e,11) ^ ROTR(e,25);
        uint32_t ch = (e & f) ^ (~e & g);
        t1 = h + S1 + ch + K256[i] + w[i];
        uint32_t S0 = ROTR(a,2) ^ ROTR(a,13) ^ ROTR(a,22);
        uint32_t mj = (a & b) ^ (a & ce) ^ (b & ce);
        t2 = S0 + mj;
        h = g; g = f; f = e; e = dd + t1;
        dd = ce; ce = b; b = a; a = t1 + t2;
    }
    c->state[0] += a; c->state[1] += b; c->state[2] += ce; c->state[3] += dd;
    c->state[4] += e; c->state[5] += f; c->state[6] += g;  c->state[7] += h;
}

static void sha256_init(sha256_ctx* c) {
    c->state[0] = 0x6a09e667; c->state[1] = 0xbb67ae85;
    c->state[2] = 0x3c6ef372; c->state[3] = 0xa54ff53a;
    c->state[4] = 0x510e527f; c->state[5] = 0x9b05688c;
    c->state[6] = 0x1f83d9ab; c->state[7] = 0x5be0cd19;
    c->bitcount = 0;
}

static void sha256_update(sha256_ctx* c, const uint8_t* data, size_t len) {
    size_t buf_used = (size_t)((c->bitcount >> 3) & 63);
    c->bitcount += (uint64_t)len * 8;
    if (buf_used) {
        size_t need = 64 - buf_used;
        if (len < need) {
            memcpy(c->buffer + buf_used, data, len);
            return;
        }
        memcpy(c->buffer + buf_used, data, need);
        sha256_transform(c, c->buffer);
        data += need; len -= need;
    }
    while (len >= 64) {
        sha256_transform(c, data);
        data += 64; len -= 64;
    }
    if (len) memcpy(c->buffer, data, len);
}

static void sha256_final(sha256_ctx* c, uint8_t out[32]) {
    size_t buf_used = (size_t)((c->bitcount >> 3) & 63);
    c->buffer[buf_used++] = 0x80;
    if (buf_used > 56) {
        memset(c->buffer + buf_used, 0, 64 - buf_used);
        sha256_transform(c, c->buffer);
        buf_used = 0;
    }
    memset(c->buffer + buf_used, 0, 56 - buf_used);
    uint64_t bc = c->bitcount;
    for (int i = 0; i < 8; i++) {
        c->buffer[63 - i] = (uint8_t)(bc & 0xff);
        bc >>= 8;
    }
    sha256_transform(c, c->buffer);
    for (int i = 0; i < 8; i++) {
        out[i*4]   = (uint8_t)(c->state[i] >> 24);
        out[i*4+1] = (uint8_t)(c->state[i] >> 16);
        out[i*4+2] = (uint8_t)(c->state[i] >> 8);
        out[i*4+3] = (uint8_t)(c->state[i]);
    }
}

static void compute_sha256(const void* data, size_t len, uint8_t out[32]) {
    sha256_ctx c;
    sha256_init(&c);
    sha256_update(&c, (const uint8_t*)data, len);
    sha256_final(&c, out);
}

#else
// ---- mbedtls (target) ----------------------------------------------------
#include <mbedtls/sha256.h>

static void compute_sha256(const void* data, size_t len, unsigned char out[32]) {
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    // 3rd arg: 0 = SHA-256, 1 = SHA-224. Compatible across mbedtls 2.x/3.x.
    mbedtls_sha256_starts_ret(&ctx, 0);
    mbedtls_sha256_update_ret(&ctx, (const unsigned char*)data, len);
    mbedtls_sha256_finish_ret(&ctx, out);
    mbedtls_sha256_free(&ctx);
}
#endif

static void to_hex(const unsigned char* digest, char* out_hex) {
    static const char* hex = "0123456789abcdef";
    for (int i = 0; i < SHA256_DIGEST_BYTES; i++) {
        out_hex[i*2]     = hex[digest[i] >> 4];
        out_hex[i*2 + 1] = hex[digest[i] & 0x0f];
    }
    out_hex[SHA256_DIGEST_BYTES * 2] = '\0';
}

void sha256_hex(const void* data, size_t len, char* out_hex) {
    if (out_hex == NULL) return;
    unsigned char digest[SHA256_DIGEST_BYTES];
    compute_sha256(data ? data : "", data ? len : 0, digest);
    to_hex(digest, out_hex);
}

bool sha256_hex_file(const char* path, char* out_hex) {
    if (out_hex == NULL) return false;
    out_hex[0] = '\0';
    if (path == NULL) return false;

    FILE* f = fopen(path, "rb");
    if (!f) return false;

#if defined(HOST_BUILD) || !defined(__3DS__)
    sha256_ctx ctx;
    sha256_init(&ctx);
    unsigned char buf[8192];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), f)) > 0) {
        sha256_update(&ctx, buf, n);
    }
    fclose(f);
    unsigned char digest[SHA256_DIGEST_BYTES];
    sha256_final(&ctx, digest);
#else
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts_ret(&ctx, 0);
    unsigned char buf[8192];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), f)) > 0) {
        mbedtls_sha256_update_ret(&ctx, buf, n);
    }
    fclose(f);
    unsigned char digest[SHA256_DIGEST_BYTES];
    mbedtls_sha256_finish_ret(&ctx, digest);
    mbedtls_sha256_free(&ctx);
#endif
    to_hex(digest, out_hex);
    return true;
}

bool verify_integrity(const char* path, const char* expected_hex) {
    if (path == NULL || expected_hex == NULL || expected_hex[0] == '\0') {
        return false;
    }
    char actual[SHA256_DIGEST_BYTES * 2 + 1];
    if (!sha256_hex_file(path, actual)) {
        return false;
    }
    // Case-insensitive compare against the expected digest.
    for (int i = 0; i < SHA256_DIGEST_BYTES * 2; i++) {
        char e = (char)tolower((unsigned char)expected_hex[i]);
        if (actual[i] != e) return false;
    }
    return expected_hex[SHA256_DIGEST_BYTES * 2] == '\0';
}
