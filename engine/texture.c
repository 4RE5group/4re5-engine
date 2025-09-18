#include "engine.h"
#include "unistd.h"


/*
	Load texture to Texture struct from BMP file.
*/
int __ARESengine__loadTexture(Texture *out, const char *path)
{
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        __builtin_printf("error: could not open BMP file '%s'\n", path);
        return 1;
    }

    // Read BMP header
    unsigned char header[54];
    if (read(fd, header, 54) != 54) {
        __builtin_printf("error: could not read BMP header\n");
        close(fd);
        return 1;
    }

    // Extract metadata (little-endian)
    out->width = *(int*)&header[18];
    out->height = *(int*)&header[22];
    out->bytes_per_pixel = header[28] / 8; // 24-bit = 3, 32-bit = 4
    uint32_t starting_offset = *(uint32_t*)&header[10];

    // Allocate pixel buffer with correct size
    out->pixels = NULL;
    out->pixels = malloc(out->width * out->height * sizeof(unsigned int));
    if (!out->pixels) {
        __builtin_printf("error: could not allocate pixel buffer\n");
        close(fd);
        return 1;
    }

    // Seek to pixel data
    if (lseek(fd, starting_offset, SEEK_SET) == -1) {
        __builtin_printf("error: could not seek to pixel data\n");
        free(out->pixels);
        out->pixels = NULL;
        close(fd);
        return 1;
    }

    // Calculate row padding (BMP rows are 4-byte aligned)
    int row_padding = (4 - (out->width * out->bytes_per_pixel) % 4) % 4;
    unsigned char *row_buffer = NULL;
    row_buffer = malloc(out->width * out->bytes_per_pixel);
    if (!row_buffer) {
        __builtin_printf("error: could not allocate row buffer\n");
        free(out->pixels);
        out->pixels = NULL;
        close(fd);
        return 1;
    }

    // Read rows from bottom to top
    for (int y = out->height - 1; y >= 0; y--) {
        if (read(fd, row_buffer, out->width * out->bytes_per_pixel) != (ssize_t)(out->width * out->bytes_per_pixel)) {
            __builtin_printf("error: failed to read pixel row %d\n", y);
            free(row_buffer);
            free(out->pixels);
            out->pixels = NULL;
            close(fd);
            return 1;
        }

        // Skip padding bytes
        if (row_padding > 0) {
            unsigned char padding[4];
            if (read(fd, padding, row_padding) != row_padding) {
                __builtin_printf("error: failed to skip padding bytes\n");
                free(row_buffer);
                free(out->pixels);
                out->pixels = NULL;
                close(fd);
                return 1;
            }
        }

        // Copy row to pixel buffer
        size_t row_start = y * out->width;
        for (int x = 0; x < out->width; x++) {
            size_t pixel_index = row_start + x;

            // Handle different pixel formats
            if (out->bytes_per_pixel == 3) { // 24-bit BMP (BGR)
                out->pixels[pixel_index] = (row_buffer[x * 3 + 2] << 16) |  // R
                                           (row_buffer[x * 3 + 1] << 8)  |  // G
                                           (row_buffer[x * 3]);            // B
            } else if (out->bytes_per_pixel == 4) { // 32-bit BMP (BGRA)
                out->pixels[pixel_index] = (row_buffer[x * 4 + 2] << 16) |  // R
                                           (row_buffer[x * 4 + 1] << 8)  |  // G
                                           (row_buffer[x * 4])      |  // B
                                           (row_buffer[x * 4 + 3] << 24); // A
            }
        }
    }

    // Free row buffer
    free(row_buffer);
    row_buffer = NULL;
    close(fd);
    return 0;
}
