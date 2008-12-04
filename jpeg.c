#include "jpeg.h"

void init_jpeg(struct jpeg_compress_struct *jpeg, struct jpeg_error_mgr *jerr,
							 FILE *dest_file, int width, int height)
{
	jpeg->err = jpeg_std_error(jerr);
	jpeg_create_compress(jpeg);
	jpeg_stdio_dest(jpeg, dest_file);
	
	jpeg->image_width = width;
	jpeg->image_height = height;
  jpeg->input_components = 3;
  jpeg->in_color_space = JCS_RGB;

	jpeg_set_defaults(jpeg);
	jpeg_set_quality(jpeg, 100, TRUE);

}

void write_jpeg(unsigned char *image, int width, int height, char *filename)
{
	struct jpeg_compress_struct jpeg;
	struct jpeg_error_mgr jerr;
	FILE *file = NULL;
	JSAMPROW *rows;

	int j;

	file = fopen(filename, "wb");
	if(NULL == file) {
		printf("Failed to open %s\n", filename);
		exit(1);
	}
		
	rows = malloc(height * sizeof(JSAMPLE*));
	if(NULL == rows) {
		printf("Failed to allocate row memory\n");
		exit(1);
	}

	init_jpeg(&jpeg, &jerr, file, width, height);
	jpeg_start_compress(&jpeg, TRUE);
 
	for(j = 0; j < height; ++j)
		rows[j] = &image[j * width * 3];
	jpeg_write_scanlines(&jpeg, rows, height);

	jpeg_finish_compress(&jpeg);
	fclose(file);
	jpeg_destroy_compress(&jpeg);	
}
