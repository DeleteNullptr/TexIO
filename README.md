# TexIO
Basic Image IO

Headers for reading basic images.
Loads pixel data in chunks from disk into user allocated memory.
The goal is to be able to stream image data into CPU/GPU shared memory with low latency.

### TGA
- 24/32 bits per pixel
- Uncompressed I/O
- RLE I
