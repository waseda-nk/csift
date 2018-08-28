#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "class.h"
#include "SIFT.h"

Image *gaussian_filter(Image *src, double sig, double octave){
  int Wm, Rm, x, y, i, p;
  double div, sum, *msk;
  Image *tmp, *dst;
  if((tmp = allocate_image(src->width, src->height, src->color_type))==NULL)
    return;
  if((dst = allocate_image(src->width, src->height, src->color_type))==NULL)
    return;
  
  Wm = (int)(ceil(3.0*sig)*2+1);
  Rm = (Wm-1)/2;
  sig = 2 * sig * sig;
  div = sqrt(sig*M_PI);
  
  // filter
  msk = (double*)malloc(sizeof(double)*Wm);
  for(x=0; x<Wm; x++){
    p = (x-Rm)*(x-Rm);
    msk[x] = exp(-p/sig)/div;
  }
  
  // vertical
  for(y=0; y<src->height; y++){
    for(x=0; x<src->width; x++){
      sum = 0;
      for(i=0; i<Wm; i++){
        p = y+i-Rm;
        if(p<0 || src->height<=p){
          sum += msk[i]*0;  //zero padding
        }else{
          sum += msk[i]*src->graymap[p][x].g;
        }
      }
      tmp->graymap[y][x].g = sum;
    }
  }
  
  // horizontal
  for(y=0; y<src->height; y++){
    for(x=0; x<src->width; x++){
      sum = 0;
      for(i=0; i<Wm; i++){
        p = x+i-Rm;
        if(p<0 || src->width<=p){
          sum += msk[i]*0;  //zero padding
        }else{
          sum += msk[i]*tmp->graymap[y][p].g;
        }
      }
      dst->graymap[y][x].g = sum;
    }
  }
  
  free(tmp);
  
  return dst;
}

void down_sampling(Image *src){
  int x, y;
  Image *tmp = NULL;
  if((tmp = allocate_image(src->width, src->height, src->color_type))==NULL)
    return;
  copy_image(tmp, *src);
  free(src);
  if((src = allocate_image(tmp->width/2, tmp->height/2, tmp->color_type))==NULL)
    return;
  
  for(y=0; y<src->height; y++)
    for(x=0; x<src->width; x++)
      src->graymap[y][x].g = tmp->graymap[2*y][2*x].g;
  strcpy(src->imname, tmp->imname);
  strcpy(src->keyname, tmp->keyname);
  strcpy(src->outname, tmp->outname);
  return;
}

Image *difference_image(Image *A, Image *B){
  int W, H, x, y;
  Image *dst = NULL;
  W = (A->width < B->width) ? A->width : B->width;
  H = (A->height < B->height) ? A->height : B->height;
  
  if((dst = allocate_image(W, H, A->color_type))==NULL)
    return;
    
  for(y=0; y<H; y++)
    for(x=0; x<W; x++)
      dst->graymap[y][x].g = A->graymap[y][x].g - B->graymap[y][x].g;
  strcpy(dst->imname, A->imname);
  strcpy(dst->keyname, A->keyname);
  strcpy(dst->outname, A->outname);
  
  return dst;
}
