#ifndef ku_png_h
#define ku_png_h

#include "ku_bitmap.c"
#include <stdio.h>

void ku_png_get_size(char* path, int* width, int* height);
void ku_png_load_into(char* path, ku_bitmap_t* bitmap);
void ku_png_write(char* filename, ku_bitmap_t* bitmap);

#endif

#if __INCLUDE_LEVEL__ == 0

#define PNG_DEBUG 3
#include "ku_draw.c"
#include "mt_log.c"
#include "mt_memory.c"
#include <png.h>
#include <string.h>

void ku_png_get_size(char* path, int* width, int* height)
{
    unsigned char header[8];

    /* open file and test for it being a png */
    FILE* fp = fopen(path, "rb");

    if (fp)
    {
	int res = fread(header, 1, 8, fp);
	if (res == 0) mt_log_error("fread error");

	if (png_sig_cmp(header, 0, 8) == 0)
	{
	    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	    if (png_ptr)
	    {
		png_infop info_ptr = png_create_info_struct(png_ptr);

		if (info_ptr)
		{
		    if (setjmp(png_jmpbuf(png_ptr)) == 0)
		    {
			png_init_io(png_ptr, fp);
			png_set_sig_bytes(png_ptr, 8);

			png_read_info(png_ptr, info_ptr);

			*width  = png_get_image_width(png_ptr, info_ptr);
			*height = png_get_image_height(png_ptr, info_ptr);
		    }
		    else mt_log_error("png init io failed");

		    png_destroy_info_struct(png_ptr, &info_ptr);
		}
		else mt_log_error("png_create_info_struct failed");

		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	    }
	    else mt_log_error("png_create_read_struct failed");
	}
	else mt_log_error("Not a PNG file");

	fclose(fp);
    }
    else mt_log_error("Cannot open %s for read", path);
}

void ku_png_load_into(char* path, ku_bitmap_t* bitmap)
{
    unsigned char header[8];

    /* open file and test for it being a png */
    FILE* fp = fopen(path, "rb");

    if (fp)
    {
	int res = fread(header, 1, 8, fp);
	if (res == 0) mt_log_error("fread error");

	if (png_sig_cmp(header, 0, 8) == 0)
	{
	    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	    if (png_ptr)
	    {
		png_infop info_ptr = png_create_info_struct(png_ptr);

		if (info_ptr)
		{
		    if (setjmp(png_jmpbuf(png_ptr)) == 0)
		    {
			png_init_io(png_ptr, fp);
			png_set_sig_bytes(png_ptr, 8);

			png_read_info(png_ptr, info_ptr);

			int width  = png_get_image_width(png_ptr, info_ptr);
			int height = png_get_image_height(png_ptr, info_ptr);

			png_bytep* row_pointers;

			png_read_update_info(png_ptr, info_ptr);

			/* read file */
			if (setjmp(png_jmpbuf(png_ptr)) == 0)
			{
			    int    y;
			    size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

			    row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);

			    for (y = 0; y < height; y++) row_pointers[y] = (png_byte*) malloc(rowbytes);

			    png_read_image(png_ptr, row_pointers);

			    ku_bitmap_t* rawbm = ku_bitmap_new(width, height);

			    for (y = 0; y < height; y++) memcpy((uint8_t*) rawbm->data + y * width * 4, row_pointers[y], rowbytes);

			    for (y = 0; y < height; y++) free(row_pointers[y]);
			    free(row_pointers);

			    if (bitmap->w == rawbm->w && bitmap->h == rawbm->h)
				ku_draw_insert(bitmap, rawbm, 0, 0);
			    else
				ku_draw_scale(rawbm, bitmap);

			    REL(rawbm);
			}
			else mt_log_error("Cannot read PNG");
		    }
		    else mt_log_error("png init io failed");

		    png_destroy_info_struct(png_ptr, &info_ptr);
		}
		else mt_log_error("png_create_info_struct failed");

		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	    }
	    else mt_log_error("png_create_read_struct failed");
	}
	else mt_log_error("Not a PNG file");

	fclose(fp);
    }
    else mt_log_error("Cannot open %s for read", path);
}

void ku_png_write(char* filename, ku_bitmap_t* bitmap)
{
    int y;

    FILE* fp = fopen(filename, "wb");

    if (fp)
    {
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (png_ptr)
	{
	    png_infop info_ptr = png_create_info_struct(png_ptr);

	    if (info_ptr)
	    {
		if (!setjmp(png_jmpbuf(png_ptr)))
		{
		    png_init_io(png_ptr, fp);

		    png_set_IHDR(
			png_ptr,
			info_ptr,
			bitmap->w,
			bitmap->h,
			8,
			PNG_COLOR_TYPE_RGBA,
			PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT,
			PNG_FILTER_TYPE_DEFAULT);
		    png_write_info(png_ptr, info_ptr);

		    size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

		    png_bytep* row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * bitmap->h);

		    for (y = 0; y < bitmap->h; y++) row_pointers[y] = (png_byte*) malloc(rowbytes);

		    for (y = 0; y < bitmap->h; y++) memcpy(row_pointers[y], (uint8_t*) bitmap->data + y * bitmap->w * 4, rowbytes);

		    if (!row_pointers) abort();

		    png_write_image(png_ptr, row_pointers);
		    png_write_end(png_ptr, NULL);

		    for (y = 0; y < bitmap->h; y++) free(row_pointers[y]);
		    free(row_pointers);
		}
		else mt_log_error("cannot set jump");
	    }
	    else mt_log_error("Cannot create info struct");

	    png_destroy_write_struct(&png_ptr, &info_ptr);
	}
	else mt_log_error("png create write failed");

	fclose(fp);
    }
    else mt_log_error("Cannot open %s for read", filename);
}

#endif
