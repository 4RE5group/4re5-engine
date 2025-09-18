#include "engine.h"
#include "unistd.h"


/*
	Load texture to Texture struct from BMP file.
*/
int		__ARESengine__loadTexture(Texture *out, const char *path)
{
	int fd = open(path, O_RDONLY);
    if (fd == -1) {
        __builtin_printf("error: Texture '%s' not found or couldn't access it\n", path);
        return 1;
    }

    // read BMP header to get width, height, and starting offset
    unsigned char header[54]; // BMP header is 54 bytes
    if (read(fd, header, 54) != 54)
	{
        __builtin_printf("error: could not read BMP header\n");
        close(fd);
        return 1;
    }
	// save texture infos
	out->width = *(int*)&header[18];
    out->height = *(int*)&header[22];
    out->bytes_per_pixel = header[28] / 8;
    uint32_t starting_offset = *(uint32_t*)&header[10];

	__builtin_printf("width=%d, height=%d, bytes_per_pixel=%d, starting_offset=%u\n",
                      out->width, out->height, out->bytes_per_pixel, starting_offset);

	out->pixels = (unsigned int*)malloc(out->width * out->height);
	if (!out->pixels)
	{
        __builtin_printf("error: could not allocate memory for texture\n");
        close(fd);
        return (1);
    }

	if(lseek(fd, starting_offset, SEEK_SET) == -1)
	{
		__builtin_printf("error: could not find starting offset inside BMP file\n");
        close(fd);
        return (1);
	}
	// index of the first pixel of the last row of image
	size_t	i = (out->width * (out->height - 1)) * out->bytes_per_pixel;
	size_t	row_len = out->width * out->bytes_per_pixel;
	// BMP is row by row from the bottom one to the top one, so fill up the buffer from end
	while (i > 0)
	{
		size_t j = 0;
		// read 3 bytes to pixel buffer
		while (j < row_len && read(fd, (char *)&out->pixels[i+j], out->bytes_per_pixel) == out->bytes_per_pixel)
		{
			j+=out->bytes_per_pixel;
		}
		i -= row_len; // go to next row
	}

	close(fd);
	return (0);
}