#!/usr/bin/env bash
# Generate small archive fixtures for host-side unit tests.
# Idempotent: only writes files when they don't already exist.
set -euo pipefail

cd "$(dirname "$0")/fixtures"

# 1. Tiny ZIP — minimal local file header for a single empty file "a".
#    Layout: PK\x03\x04 + version+flags+compression+date+time+crc+sizes(0)+name(1)+extra(0) + name "a"
#    + central dir + EOCD. We craft this by hand for repeatability and to
#    avoid depending on `zip` being installed.
python3 - <<'PY'
import struct, os

def write_zip(path):
    name = b"a"
    local = struct.pack("<IHHHHHIIIHH",
        0x04034b50,  # local file header signature
        20,          # version needed
        0,           # flags
        0,           # compression: stored
        0, 0,        # mod time, mod date
        0,           # crc32 (empty)
        0, 0,        # compressed, uncompressed
        len(name),   # filename length
        0)           # extra length
    local += name

    central = struct.pack("<IHHHHHHIIIHHHHHII",
        0x02014b50,  # central dir signature
        20, 20,      # version made/needed
        0, 0,        # flags, compression
        0, 0,        # time, date
        0,           # crc
        0, 0,        # compressed, uncompressed
        len(name),   # filename length
        0, 0,        # extra, comment
        0,           # disk start
        0,           # internal attr
        0,           # external attr
        0)           # relative offset
    central += name

    eocd = struct.pack("<IHHHHIIH",
        0x06054b50,  # EOCD signature
        0, 0,        # disk numbers
        1, 1,        # entries (this disk / total)
        len(central),# central dir size
        len(local),  # central dir offset
        0)           # comment length

    with open(path, "wb") as f:
        f.write(local + central + eocd)

if not os.path.exists("tiny.zip"):
    write_zip("tiny.zip")

# 2. Tiny TAR (ustar), single empty file "a".
def write_tar(path):
    name = b"a"
    block = bytearray(512)
    block[0:len(name)] = name
    # mode "0000644 ", uid/gid "0000000 ", size "00000000000 ", mtime, chksum (8 spaces),
    # typeflag '0' (regular file), linkname (empty), magic "ustar\x0000".
    def field(s, sz):
        s = s.encode("ascii")
        return s + b"\x00" * (sz - len(s))
    block[100:108] = field("0000644", 8)
    block[108:116] = field("0000000", 8)
    block[116:124] = field("0000000", 8)
    block[124:136] = field("00000000000", 12)  # size
    block[136:148] = field("00000000000", 12)  # mtime
    block[148:156] = b" " * 8                   # chksum placeholder
    block[156:157] = b"0"                       # typeflag
    block[257:263] = b"ustar\x00"
    block[263:265] = b"00"
    # compute checksum
    chksum = sum(block) & 0o777777
    s = ("%06o" % chksum).encode("ascii") + b"\x00 "
    block[148:156] = s

    with open(path, "wb") as f:
        f.write(block)
        f.write(b"\x00" * 1024)  # two empty terminator blocks

if not os.path.exists("tiny.tar"):
    write_tar("tiny.tar")

# 3. Tiny GZIP (gzip header on empty content). We hand-craft the minimum.
if not os.path.exists("tiny.gz"):
    # gzip(empty) using zlib
    import gzip
    with gzip.open("tiny.gz", "wb") as f:
        f.write(b"")

# 4. BZIP2 (just header marker is enough for detection).
if not os.path.exists("tiny.bz2"):
    # `BZh` is the magic; the rest is real bzip2 stream of empty input.
    import bz2
    with open("tiny.bz2", "wb") as f:
        f.write(bz2.compress(b""))

# 5. XZ (header magic check only).
if not os.path.exists("tiny.xz"):
    import lzma
    with open("tiny.xz", "wb") as f:
        f.write(lzma.compress(b""))

# 6. ZSTD — write only the magic + minimal frame header bytes; detect logic
#    only sniffs the first 4 bytes so we don't need a real ZSTD encoder.
if not os.path.exists("tiny.zst"):
    with open("tiny.zst", "wb") as f:
        f.write(b"\x28\xB5\x2F\xFD" + b"\x00" * 16)

# 7. 7z magic.
if not os.path.exists("tiny.7z"):
    with open("tiny.7z", "wb") as f:
        f.write(b"\x37\x7A\xBC\xAF\x27\x1C" + b"\x00" * 16)

# 8. RAR magic.
if not os.path.exists("tiny.rar"):
    with open("tiny.rar", "wb") as f:
        f.write(b"Rar!\x1a\x07" + b"\x00" * 16)

# 9. Random "not an archive".
if not os.path.exists("not_an_archive.bin"):
    with open("not_an_archive.bin", "wb") as f:
        f.write(b"hello world this is plain text\n" * 8)

# 10. Tiny too-short file (<4 bytes) and 3-byte file.
if not os.path.exists("three_bytes.bin"):
    with open("three_bytes.bin", "wb") as f:
        f.write(b"abc")

if not os.path.exists("empty.bin"):
    open("empty.bin", "wb").close()

print("fixtures ready")
PY
