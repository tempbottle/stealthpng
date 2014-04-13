#include "png_util.h"

void read_png_file(char *file_name) {
	char header[8]; // 8 is the maximum size that can be checked

	// Open file and test for it being a png
	FILE *fp = fopen(file_name, "rb");
	if (!fp)
		Error("File %s could not be opened for reading", file_name);
	fread(header, 1, 8, fp);
	if (png_sig_cmp((unsigned char *) header, 0, 8))
		Error("File %s is not recognized as a PNG file", file_name);


	// Initialize stuff
	png_structp png_ptr;
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
		Error("png_create_read_struct failed");

	png_infop info_ptr;
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
		Error("png_create_info_struct failed");

	if (setjmp(png_jmpbuf(png_ptr)))
		Error("Error during init_io");

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	width = png_get_image_width(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);
	color_type = png_get_color_type(png_ptr, info_ptr);
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);

	int number_of_passes;
	number_of_passes = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);


	// Read file
	if (setjmp(png_jmpbuf(png_ptr)))
		Error("Error during read_image");

	row_pointers = malloc(sizeof(png_bytep) * height);
	for (int y = 0; y < height; y++)
		row_pointers[y] = malloc(png_get_rowbytes(png_ptr, info_ptr));

	png_read_image(png_ptr, row_pointers);

	fclose(fp);
}

void write_png_file(char *file_name) {
	// Create file
	FILE *fp = fopen(file_name, "wb");
	if (!fp)
		Error("File %s could not be opened for writing", file_name);


	// Initialize stuff
	png_structp png_ptr;
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
		Error("png_create_write_struct failed");

	png_infop info_ptr;
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
		Error("png_create_info_struct failed");

	if (setjmp(png_jmpbuf(png_ptr)))
		Error("Error during init_io");

	png_init_io(png_ptr, fp);


	// Write header
	if (setjmp(png_jmpbuf(png_ptr)))
		Error("Error during writing header");

	png_set_IHDR(png_ptr, info_ptr, width, height,
		bit_depth, color_type, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);


	// Write bytes
	if (setjmp(png_jmpbuf(png_ptr)))
		Error("Error during writing bytes");

	png_write_image(png_ptr, row_pointers);


	// End write
	if (setjmp(png_jmpbuf(png_ptr)))
		Error("Error during end of write");

	png_write_end(png_ptr, NULL);

	fclose(fp);
}

void png_clean() {
	for (int y = 0; y < height; y++)
		free(row_pointers[y]);
	free(row_pointers);
}
