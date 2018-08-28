#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <jpeglib.h>
#include "class.h"
#include "SIFT.h"

#define PATHNAME_SIZE 512

void read_jpeg_error(struct jpeg_decompress_struct cinfo){
  jpeg_destroy_decompress(&cinfo);
  fprintf(stderr, "[\x1b[31mNG\x1b[39m]\n");
  fprintf(stderr, "\t\tWrong Image.\n");
  return;
}

Image *load_jpeg(const char *imname, const char *keyname){
  fprintf(stderr, "\t%s loading .. ", imname);
  
  int stride;
  uint32_t x, y;
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  Image *img = NULL;
  JSAMPROW buffer = NULL;
  JSAMPROW row;
  FILE *fp;
  fp = fopen(imname, "rb");
  if(fp == NULL){
    fprintf(stderr, "[\x1b[31mNG\x1b[39m]\n");
    fprintf(stderr, "\t\tWrong Image.\n");
    return img;
  }
  
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, fp);
  if(jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK){
    read_jpeg_error(cinfo);
    return img;
  }
  
  jpeg_start_decompress(&cinfo);
  int color_type = 0;
  if(cinfo.out_color_space == JCS_RGB)
    color_type = 1;
  else if(cinfo.out_color_space == JCS_GRAYSCALE)
    color_type = 2;
  else{
    read_jpeg_error(cinfo);
    return img;
  }
  if((img = allocate_image(cinfo.output_width, cinfo.output_height, color_type))==NULL){
    read_jpeg_error(cinfo);
    return img;
  }

  stride = sizeof(JSAMPLE) * cinfo.output_width * cinfo.output_components;
  if((buffer = calloc(stride, 1))==NULL){
    read_jpeg_error(cinfo);
    return img;
  }
  
  strcpy(img->imname, imname);
  strcpy(img->keyname, keyname);
  char outname[PATHNAME_SIZE] = {'\0'};
  strrep(img->imname, outname, "_gray.jpg");
  strcpy(img->outname, outname);
  
  for (y=0; y<img->height; y++) {
    jpeg_read_scanlines(&cinfo, &buffer, 1);
    row = buffer;
    for (x=0; x<img->width; x++) {
      if(cinfo.out_color_space == JCS_RGB){
        img->map[y][x].r = *row++;
        img->map[y][x].g = *row++;
        img->map[y][x].b = *row++;
        img->map[y][x].a = 0xff;
      }else if(cinfo.out_color_space == JCS_GRAYSCALE){
        img->graymap[y][x].g = *row++;
      }
    }
  }
  free(buffer);
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  fclose(fp);
  
  if(img->color_type == 1)
    to_grayscale(img);
  
  fprintf(stderr, "[\x1b[32mOK\x1b[39m]\n");
  return img;
}
