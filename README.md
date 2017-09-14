## TexIO
### Basic Image IO

Headers for reading basic images.
Loads pixel data in chunks from disk into user allocated memory.
The goal is to be able to stream image data into CPU/GPU shared memory with low latency.

### Notes
- 24-bit color formats are padded to 32-bit when loading. (GPU's prefer fiting data into 32-bits)
32-bit formats that dont contain useful data in the Alpha channel will be saved out as 24-bits.
As a result of this padding you need to check both the PixelFormat and NumChannels members of ImageDesc.
The format and pixelsize indicate the size of the pixel data on the device.
The NumChannels value is used to determine the actual size/format on disk.

### Supported Formats
#### TGA
- 24/32 bits per pixel
- Uncompressed I/O
- RLE I
