#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <jpeglib.h>
#include <argtable2.h>
#include <string.h>

const char *input_file;
const char *output_file;
const char *filter;
double times;
double percentage;
const char *axis;
const char *direction;

JSAMPARRAY row_pointers = NULL;
JDIMENSION width;
JDIMENSION height;
int num_components;
int quality = 75;
J_COLOR_SPACE color_space;

void negate()
{
	int x, y;
	if (color_space != JCS_RGB)
		return;

	for (y = 0; y < height; y++)
    {
      JSAMPROW row = row_pointers[y];
      for (x = 0; x < width; x++)
        {
          JSAMPROW ptr = &(row[x * 3]);

          ptr[0] = 255 - ptr[0];
          ptr[1] = 255 - ptr[1];
          ptr[2] = 255 - ptr[2];
        }
    }
}

void brightness()
{
	int x, y;
	if (color_space != JCS_RGB)
		return;

	for (y = 0; y < height; y++)
    {
      JSAMPROW row = row_pointers[y];
      for (x = 0; x < width; x++)
        {
          JSAMPROW ptr = &(row[x * 3]);

          int newr = ptr[0] + percentage * ptr[0] / 100;
          int newg = ptr[1] + percentage * ptr[1] / 100;
          int newb = ptr[2] + percentage * ptr[2] / 100;

          if(newr > 255) ptr[0]=255;
          else if (newr < 0) ptr[0] = 0;
          else ptr[0] = newr;

          if(newg > 255) ptr[1]=255;
          else if (newg < 0) ptr[1] = 0;
          else ptr[1] = newg;

          if(newb > 255) ptr[2]=255;
          else if (newb < 0) ptr[2] = 0;
          else ptr[2] = newb;
        }
    }
}

void contrast()
{
	int x, y;
	if (color_space != JCS_RGB)
		return;

	for (y = 0; y < height; y++)
    {
      JSAMPROW row = row_pointers[y];
      for (x = 0; x < width; x++)
        {
          JSAMPROW ptr = &(row[x * 3]);

          int newr = times * (ptr[0] - 127) + 127;
          int newg = times * (ptr[1] - 127) + 127;
          int newb = times * (ptr[2] - 127) + 127;

          if(newr > 255) ptr[0]=255;
          else if (newr < 0) ptr[0] = 0;
          else ptr[0] = newr;

          if(newg > 255) ptr[1]=255;
          else if (newg < 0) ptr[1] = 0;
          else ptr[1] = newg;

          if(newb > 255) ptr[2]=255;
          else if (newb < 0) ptr[2] = 0;
          else ptr[2] = newb;
        }
    }
}

void flip()
{
	int x, y;

	if (color_space != JCS_RGB)
		return;

  if (strcmp(axis, "x"))
    {
      for (y = 0; y < height; y++)
        {
          JSAMPROW row = row_pointers[y];
          for (x = 0; x*2 < width; x++)
            {
              JSAMPROW ptr = &(row[x * 3]);
              JSAMPROW ptr2 = &(row[(width-x-1) * 3]);

              int varr = ptr[0];
              int varg = ptr[1];
              int varb = ptr[2];

              ptr[0] = ptr2[0];
              ptr2[0] = varr;
              ptr[1] = ptr2[1];
              ptr2[1] = varg;
              ptr[2] = ptr2[2];
              ptr2[2] = varb;
            }
        }
    }
  else if (strcmp(axis, "y"))
    {
      for (y = 0; y*2 < height; y++)
        {
          JSAMPROW row = row_pointers[y];
          JSAMPROW row2 = row_pointers[height-y-1];
          for (x = 0; x < width; x++)
            {
              JSAMPROW ptr = &(row[x * 3]);
              JSAMPROW ptr2 = &(row2[x * 3]);

              int varr = ptr[0];
              int varg = ptr[1];
              int varb = ptr[2];

              ptr[0] = ptr2[0];
              ptr2[0] = varr;
              ptr[1] = ptr2[1];
              ptr2[1] = varg;
              ptr[2] = ptr2[2];
              ptr2[2] = varb;

            }
        }
    }
  else printf("error");
}

void rotate_pic()
{
    int x, y, h;
    int img[height][width][3];

	if (color_space != JCS_RGB)
		return;

	for (y = 0; y < height; y++){
    for (x = 0; x < width; x++){
      JSAMPROW ptr = &(row_pointers[y][x * 3]);
      img[y][x][0]=ptr[0];
      img[y][x][1]=ptr[1];
      img[y][x][2]=ptr[2];
    }
  }

    JDIMENSION temp = width;
    h = height;
    width = h;
    //height = temp;// wyrzuca naruszenie ochrony pamięci

  if (strcmp(direction, "right") == 0)
    {
      printf("right");
      for (y = 0; y < height; y++)
        {
          for (x = 0; x < width; x++)
            {
              JSAMPROW ptr = &(row_pointers[x][(h-y-1) * 3]);
              ptr[0]=img[y][x][0];
              ptr[1]=img[y][x][1];
              ptr[2]=img[y][x][2];
            }
        }
    }
  if (strcmp(direction, "left") == 0)
    {
      printf("left");
      for (y = 0; y < height; y++)
        {
          for (x = 0; x < width; x++)
            {
              JSAMPROW ptr = &(row_pointers[x][y * 3]);
              ptr[0]=img[y][x][0];
              ptr[1]=img[y][x][1];
              ptr[2]=img[y][x][2];
            }
        }
    }
}

void process_file()
{
	if (strcmp(filter, "negate") == 0) negate();
	else if (strcmp(filter, "contrast") == 0) contrast();
	else if (strcmp(filter, "brightness") == 0) brightness();
	else if (strcmp(filter, "flip") == 0) flip();
	else if (strcmp(filter, "rotate") == 0) rotate_pic();
}

void abort_(const char *s, ...)
{
	va_list args;
	va_start(args, s);
	vfprintf(stderr, s, args);
	fprintf(stderr, "\n");
	va_end(args);
	abort();
}

void read_jpeg_file(const char *filename)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	int y;

	FILE *infile = fopen(filename, "rb");

	if (!infile)
    {
      abort_("Error opening input jpeg file %s!\n", filename);
    }
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, infile);
	jpeg_read_header(&cinfo, TRUE);

	jpeg_start_decompress(&cinfo);
	width = cinfo.output_width;
	height = cinfo.output_height;
	num_components = cinfo.out_color_components;
	color_space = cinfo.out_color_space;

	size_t rowbytes = width * num_components;

	row_pointers = (JSAMPARRAY)malloc(sizeof(j_common_ptr) * height);
	for (y = 0; y < height; y++)
    {
      row_pointers[y] = (JSAMPROW)malloc(rowbytes);
    }
	y = 0;
	JSAMPARRAY tmp = row_pointers;
	while (cinfo.output_scanline < cinfo.image_height)
    {
      y = jpeg_read_scanlines(&cinfo, tmp, 1);
      tmp += y;
    }
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(infile);

}

void write_jpeg_file(const char *filename)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	int y;
	JSAMPARRAY tmp;

	FILE *outfile = fopen(filename, "wb");

	if (!outfile)
    {
      abort_("Error opening output jpeg file %s!\n", filename);
    }
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = num_components;
	cinfo.in_color_space = color_space;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE);
	jpeg_start_compress(&cinfo, TRUE);
	tmp = row_pointers;
	while (cinfo.next_scanline < cinfo.image_height)
    {
      y = jpeg_write_scanlines(&cinfo, tmp, 1);
      tmp += y;
    }
	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
	fclose(outfile);

	for (y = 0; y < height; y++)
    {
      free(row_pointers[y]);
    }
	free(row_pointers);
}

int main(int argc, char **argv)
{
	// Options
	struct arg_file *input_file_arg = arg_file1("i", "input-file", "<input>", "Input JPEG File");
	struct arg_file *output_file_arg = arg_file1("o", "out-file", "<output>", "Output JPEG File");
	struct arg_str *filter_arg = arg_str1("f", "filter", "<filter>", "Filter");
	struct arg_dbl *times_arg = arg_dbl0("t", "times", "<times>", "Multiplyer");
	struct arg_dbl *percentage_arg = arg_dbl0("k", "percentage", "<percentage>", "Percentage");
	struct arg_lit *help = arg_lit0("h", "help", "print this help and exit");
	struct arg_str *flip_arg = arg_str0("a", "axis", "<axis>", "Axis");
	struct arg_str *rotate_arg = arg_str0("d", "direction", "<direction>", "Direction");
	struct arg_end *end = arg_end(10); // maksymalna liczba błędów 10

	int nerrors;

	void *argtable[] = {input_file_arg, output_file_arg, filter_arg, times_arg, percentage_arg, flip_arg, rotate_arg, help, end};

	if (arg_nullcheck(argtable) != 0)
		printf("error: insufficient memory\n");

	times_arg->dval[0] = 1;
	percentage_arg->dval[0] = 1;
	flip_arg->sval[0] = "";
	rotate_arg->sval[0] = "";

	nerrors = arg_parse(argc, argv, argtable);

	if (nerrors == 0)
    {
      input_file = input_file_arg->filename[0];
      output_file = output_file_arg->filename[0];
      filter = filter_arg->sval[0];
      times = times_arg->dval[0];
      percentage = percentage_arg->dval[0];
      axis = flip_arg->sval[0];
      direction = rotate_arg->sval[0];
    }
	else
    {
      arg_print_errors(stderr, end, "point");
      arg_print_glossary(stderr, argtable, " %-25s %s\n");
      arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
      return 1;
    }

	if (help->count > 0)
    {
      printf("Usage: point");
      arg_print_syntax(stdout, argtable, "\n");
      arg_print_glossary(stdout, argtable, "  %-25s %s\n");
      arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
      return 0;
    }

	read_jpeg_file(input_file);
	process_file();
	write_jpeg_file(output_file);
	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
	return 0;
}
