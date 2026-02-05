#!/usr/bin/env python3
import struct
# Create a simple 48x48 PNG icon manually
# PNG signature
png_sig = b'\x89PNG\r\n\x1a\n'
# IHDR chunk (48x48, 8-bit RGB)
width = 48
height = 48
ihdr_data = struct.pack('>IIBBBBB', width, height, 8, 2, 0, 0, 0)
ihdr_crc = 0x9c73ff4b  # Pre-calculated CRC for this IHDR
def make_chunk(chunk_type, data):
    import zlib
    length = struct.pack('>I', len(data))
    chunk = chunk_type + data
    crc = struct.pack('>I', zlib.crc32(chunk) & 0xffffffff)
    return length + chunk + crc
# Create pixel data (blue background with white center)
pixels = []
for y in range(height):
    row = [0]  # Filter type: None
    for x in range(width):
        # Blue background (#3B5998)
        if 8 <= x < 40 and 8 <= y < 40:
            # White center
            row.extend([255, 255, 255])
        else:
            # Blue background
            row.extend([0x3B, 0x59, 0x98])
    pixels.extend(row)
import zlib
idat_data = zlib.compress(bytes(pixels), 9)
# Build PNG
with open('icon.png', 'wb') as f:
    f.write(png_sig)
    f.write(make_chunk(b'IHDR', ihdr_data))
    f.write(make_chunk(b'IDAT', idat_data))
    f.write(make_chunk(b'IEND', b''))
print("Icon created successfully!")
