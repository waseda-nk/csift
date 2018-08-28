#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "class.h"
#include "SIFT.h"

#define PATHNAME_SIZE 512

Image *allocate_image(uint32_t width, uint32_t height, int color_type){
  Image *img = NULL;
  int y;
  
  img = (Image*)calloc(1, sizeof(Image));
  
  if(color_type == 1){
    img->color_type = 1;
    img->map = (color_t**)calloc(height, sizeof(color_t*));
    for(y=0; y<height; y++)
      img->map[y] = (color_t*)calloc(width, sizeof(color_t));
  }else if(color_type == 2){
    img->color_type = 2;
  }
  img->graymap = (gray_t**)calloc(height, sizeof(gray_t*));
  for(y=0; y<height; y++)
    img->graymap[y] = (gray_t*)calloc(width, sizeof(gray_t));
  
  img->imname = (char*)malloc(sizeof(char) * PATHNAME_SIZE);
  img->keyname = (char*)malloc(sizeof(char) * PATHNAME_SIZE);
  img->outname = (char*)malloc(sizeof(char) * PATHNAME_SIZE);
  img->width = width;
  img->height = height;
  
  return img;
}

void copy_image(Image *img, Image src){
  int x, y;
  for(x=0; x<src.width; x++)
    for(y=0; y<src.height; y++)
      img->graymap[y][x].g = src.graymap[y][x].g;
  strcpy(img->imname, src.imname);
  strcpy(img->keyname, src.keyname);
  strcpy(img->outname, src.outname);
  return;
}

