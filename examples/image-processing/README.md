# 📁 WASM Example: Image Processing

A practical example of using WASM for image processing.

## Files

```
examples/image-processing/
├── README.md
├── index.html
├── image-processor.wat   # Optional: hand-written WAT
└── c-version/            # Optional: C implementation
    ├── main.c
    ├── build.bat
    └── build.sh
```

## Quick Demo

Open `index.html` in your browser to see WASM image processing in action!

## What This Demonstrates

1. **Shared Memory** - Passing image pixel data to WASM
2. **Batch Processing** - Processing all pixels in a single call
3. **Performance** - WASM vs JavaScript comparison
4. **Real-world Use Case** - Image manipulation

## The Algorithm

### Grayscale Conversion

```
gray = 0.299 * R + 0.587 * G + 0.114 * B
```

This weighted formula (ITU-R BT.601) matches human perception of brightness.

### Sepia Effect

```
newR = min(255, 0.393*R + 0.769*G + 0.189*B)
newG = min(255, 0.349*R + 0.686*G + 0.168*B)
newB = min(255, 0.272*R + 0.534*G + 0.131*B)
```

### Invert

```
newR = 255 - R
newG = 255 - G
newB = 255 - B
```

## Implementation Notes

- Images are RGBA format (4 bytes per pixel)
- Alpha channel is preserved (not modified)
- Processing is done in-place in shared memory
- Functions receive pointer and length (bytes)

## C Implementation

```c
#include <emscripten.h>
#include <stdint.h>

EMSCRIPTEN_KEEPALIVE
void grayscale(uint8_t* pixels, int width, int height) {
    int size = width * height * 4;
    for (int i = 0; i < size; i += 4) {
        float r = (float)pixels[i];
        float g = (float)pixels[i + 1];
        float b = (float)pixels[i + 2];

        uint8_t gray = (uint8_t)(0.299f * r + 0.587f * g + 0.114f * b);

        pixels[i] = gray;
        pixels[i + 1] = gray;
        pixels[i + 2] = gray;
    }
}
```
