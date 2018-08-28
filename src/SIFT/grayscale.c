#include <stdlib.h>
#include "class.h"
#include "SIFT.h"

void to_grayscale(Image *img){
  int x, y;
  for(y=0; y<img->height; y++){
    for(x=0; x<img->width; x++){
      img->graymap[y][x].g = 0.299*img->map[y][x].r + 0.587*img->map[y][x].g + 0.114*img->map[y][x].b;
      //img->graymap[y][x].g = 0.212656*img->map[y][x].r + 0.715158*img->map[y][x].g + 0.072186*img->map[y][x].b;
    }
    free(img->map[y]);
  }
  free(img->map);
  img->color_type = 2;
  return;
}
