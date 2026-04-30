#ifndef INTEGRITY_H
#define INTEGRITY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SHA256_DIGEST_BYTES 32

// Compute SHA-256 of `data`/`len` and write the lowercase hex digest
// (64 chars + NUL) to `out_hex`. `out_hex` must have room for at least
// 65 bytes.
//
// On the 3DS target this uses mbedtls (already in portlibs); on host
// builds it uses the same vendored constants in pure C so the unit
// tests don't need any extra dependency.
void sha256_hex(const void* data, size_t len, char* out_hex);

// Compute SHA-256 of an open file (read in chunks). Returns true on
// success; on failure `out_hex[0]` is set to NUL.
bool sha256_hex_file(const char* path, char* out_hex);

// Verify the SHA-256 of `path` against `expected_hex` (64 lowercase hex
// chars). Returns true on match.
bool verify_integrity(const char* path, const char* expected_hex);

#endif // INTEGRITY_H
