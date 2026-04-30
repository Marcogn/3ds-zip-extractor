// Minimal stand-ins for libctru types so pure modules can be built on a
// regular host PC for the unit-test suite. The corresponding C sources
// guard real `<3ds.h>` includes with `#ifndef HOST_BUILD`.
//
// Only the very small subset of types the pure modules need is provided
// here. Anything that depends on actual 3DS hardware (curl, citro2d,
// libarchive disk extraction, hidScanInput, ...) is NOT host-testable
// and is excluded from the host build.

#ifndef THREE_DS_STUBS_H
#define THREE_DS_STUBS_H

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int32_t  Result;

#endif // THREE_DS_STUBS_H
