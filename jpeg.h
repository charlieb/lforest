#ifndef JPEG_H
#define JEPG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jpeglib.h>

void write_jpeg(unsigned char *image, int width, int height, char *filename);

#endif
