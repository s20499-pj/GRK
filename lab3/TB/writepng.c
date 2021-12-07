/*
 * Copyright 2002-2008 Guillaume Cottenceau, 2015 Aleksander Denisiuk
 *
 * This software may be freely redistributed under the terms
 * of the X11 license.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define PNG_DEBUG 3
#include <png.h>

#define OUT_FILE "initials.png"
#define WIDTH 601
#define HEIGHT 601
#define COLOR_TYPE PNG_COLOR_TYPE_RGB
#define BIT_DEPTH 8

void abort_(const char *s, ...)
{
    va_list args;
    va_start(args, s);
    vfprintf(stderr, s, args);
    fprintf(stderr, "\n");
    va_end(args);
    abort();
}

int x, y;

int width, height;
png_byte color_type;
png_byte bit_depth;

png_structp png_ptr;
png_infop info_ptr;
int number_of_passes;
png_bytep *row_pointers;

void create_png_file()
{
    width = WIDTH;
    height = HEIGHT;
    bit_depth = BIT_DEPTH;
    color_type = COLOR_TYPE;

    row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * height);
    for (y = 0; y < height; y++)
        row_pointers[y] = (png_byte *)malloc(width * bit_depth * 3);
}

void write_png_file(char *file_name)
{
    /* create file */
    FILE *fp = fopen(file_name, "wb");
    if (!fp)
        abort_("[write_png_file] File %s could not be opened for writing", file_name);

    /* initialize stuff */
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr)
        abort_("[write_png_file] png_create_write_struct failed");

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
        abort_("[write_png_file] png_create_info_struct failed");

    if (setjmp(png_jmpbuf(png_ptr)))
        abort_("[write_png_file] Error during init_io");

    png_init_io(png_ptr, fp);

    /* write header */
    if (setjmp(png_jmpbuf(png_ptr)))
        abort_("[write_png_file] Error during writing header");

    png_set_IHDR(png_ptr, info_ptr, width, height,
                 bit_depth, color_type, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

    /* write bytes */
    if (setjmp(png_jmpbuf(png_ptr)))
        abort_("[write_png_file] Error during writing bytes");

    png_write_image(png_ptr, row_pointers);

    /* end write */
    if (setjmp(png_jmpbuf(png_ptr)))
        abort_("[write_png_file] Error during end of write");

    png_write_end(png_ptr, NULL);

    /* cleanup heap allocation */
    for (y = 0; y < height; y++)
        free(row_pointers[y]);
    free(row_pointers);

    fclose(fp);
}

void write_pixel(int x, int y, png_byte cr, png_byte cg, png_byte cb)
{
    png_byte *row = row_pointers[y];
    png_byte *ptr = &(row[x * 3]);
    ptr[0] = cr;
    ptr[1] = cg;
    ptr[2] = cb;
}

png_byte *get_pixel(int x, int y, int offset)
{
  if (x >= WIDTH || y >= HEIGHT || x < 0 || y < 0)
    return NULL;
  png_byte *row = row_pointers[y];
  png_byte *ptr = &(row[x * 3]);
  return &ptr[offset];
}

void flood(int x, int y, png_byte cr, png_byte cg, png_byte cb)
{

  png_byte *check_r = get_pixel(x, y, 0);
  png_byte *check_g = get_pixel(x, y, 1);
  png_byte *check_b = get_pixel(x, y, 2);
  if (check_r == NULL || check_g == NULL || check_b == NULL)
    return;



  if (*check_r != cr || *check_g != cg || *check_b != cb)
    {
      write_pixel(x,y,cr,cg,cb);
      flood(x + 1, y, cr,cg,cb);
      flood(x - 1, y, cr,cg,cb);
      flood(x, y + 1, cr,cg,cb);
      flood(x, y - 1, cr,cg,cb);
    }
}
void bresenham(int i1, int j1, int i2, int j2, png_byte cr, png_byte cg, png_byte cb)
{
  int i, j, b, p, m;

  if(((i2 < i1) && (i1 - i2 > j2 - j1)) ||
     ((j2 < j1) && (i2 - i1 < j1 - j2))){
      i = i1;
      i1 = i2;
      i2 = i;

      j = j1;
      j1 = j2;
      j2 = j;
    }

  if (j2 - j1 < i2 - i1){
      if(j2 - j1 > 0) m = 2 * (j2 - j1);
      else m = 2 * (j1 - j2);
      b = 0;
      write_pixel(i1, j1, cr, cg, cb);
      j = j1;
      p = i2 - i1;
      for (i = i1 + 1; i <= i2; i++){
          b += m;
          if (b > p){
              if(j2 - j1 > 0) j += 1;
              else j -= 1;
              b -= 2 * p;
          }
          write_pixel(i, j, cr, cg, cb);
        }
    }
  else{
      if(i2 - i1 > 0) m = 2 * (i2 - i1);
      else m = 2 * (i1 - i2);
      b = 0;
      write_pixel(i1, j1, cr, cg, cb);
      i = i1;
      p = j2 - j1;
      for (j = j1 + 1; j <= j2; j++){
          b += m;
          if (b > p){
              if(i2 - i1 > 0) i += 1;
              else i -= 1;
              b -= 2 * p;
            }
          write_pixel(i, j, cr, cg, cb);
        }
    }
}

void circle(int x, int y, int r, png_byte cr, png_byte cg, png_byte cb)
{
  int i = 0;
  int j = r;
  int d = 5 - 4 * r;

  while (i < j){
      if (d > 0){
          j--;
          d += 8 * (i - j) + 20;
        }
      else d += 8 * i + 12;
      write_pixel(x+i, y+j, cr, cg, cb);
      write_pixel(x+i, y-j, cr, cg, cb);
      write_pixel(x-i, y+j, cr, cg, cb);
      write_pixel(x-i, y-j, cr, cg, cb);
      write_pixel(x+j, y+i, cr, cg, cb);
      write_pixel(x+j, y-i, cr, cg, cb);
      write_pixel(x-j, y+i, cr, cg, cb);
      write_pixel(x-j, y-i, cr, cg, cb);
      i++;
    }
}



void process_file(void)
{
  /*for (y = 0; y < height; y++)
    {
      png_byte *row = row_pointers[y];
      for (x = 0; x < width; x++)
        {
          png_byte *ptr = &(row[x * 3]);
          ptr[0] = ptr[1] = ptr[2] = 127;
        }
  */
  for(int i = 1; i<50000; i++){
    bresenham(rand()%600,rand()%600,rand()%600,rand()%600,rand()%256,rand()%256,rand()%256);
    }

  circle(300,300,300,255,255,255);


  bresenham(90,120,510,120,0,0,0);
  bresenham(510,120,510,180,0,0,0);
  bresenham(510,180,330,180,0,0,0);
  bresenham(330,180,330,570,0,0,0);
  bresenham(330,570,270,570,0,0,0);
  bresenham(270,570,270,180,0,0,0);
  bresenham(270,180,90,180,0,0,0);
  bresenham(90,180,90,120,0,0,0);

  bresenham(360,210,360,570,0,0,0);
  bresenham(360,570,428,570,0,0,0);
  bresenham(428,570,510,480,0,0,0);
  bresenham(510,480,450,390,0,0,0);
  bresenham(450,390,510,300,0,0,0);
  bresenham(510,300,450,210,0,0,0);
  bresenham(450,210,360,210,0,0,0);

  bresenham(405,255,435,255,0,0,0);
  bresenham(435,255,465,300,0,0,0);
  bresenham(465,300,435,345,0,0,0);
  bresenham(435,345,405,345,0,0,0);
  bresenham(405,345,405,255,0,0,0);

  bresenham(405,435,435,435,0,0,0);
  bresenham(435,435,465,480,0,0,0);
  bresenham(465,480,435,525,0,0,0);
  bresenham(435,525,405,525,0,0,0);
  bresenham(405,525,405,435,0,0,0);

  flood(0,0,255,255,255);
  flood(0,600,255,255,255);
  flood(600,0,255,255,255);
  flood(600,600,255,255,255);
  flood(300,300,0,0,0);
  flood(500,300,0,0,0);
}

int main(int argc, char **argv)
{
    create_png_file();
    process_file();
    write_png_file(OUT_FILE);

    return 0;
}
