#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <jpeglib.h>
#include <math.h>
#include "class.h"
#include "SIFT.h"

#define PATHNAME_SIZE 512
#define S 3

void write_jpeg(Image img){
  fprintf(stderr, "\t%s writing .. ", img.outname);
  
  int x, y;
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  JSAMPROW buffer = NULL;
  JSAMPROW row;
  FILE *fp;
  fp = fopen(img.outname, "wb");
  if(fp == NULL || &img == NULL || (buffer = malloc(sizeof(JSAMPLE) * 3 * img.width)) == NULL){
    fprintf(stderr, "[\x1b[31mNG\x1b[39m]\n");
    fprintf(stderr, "\t\tCannot write image.\n");
    return;
  }
  
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  jpeg_stdio_dest(&cinfo, fp);
  cinfo.image_width = img.width;
  cinfo.image_height = img.height;
  if(img.color_type==1){
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
  }else if(img.color_type==2){
    cinfo.input_components = 1;
    cinfo.in_color_space = JCS_GRAYSCALE;
  }
  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, 100, TRUE);
  jpeg_start_compress(&cinfo, TRUE);
  for(y=0; y<img.height; y++){
    row = buffer;
    for(x=0; x<img.width; x++){
      if(img.color_type==1){
        *row++ = img.map[y][x].r;
        *row++ = img.map[y][x].g;
        *row++ = img.map[y][x].b;
      }else if(img.color_type==2){
        *row++ = img.graymap[y][x].g;
      }
    }
    jpeg_write_scanlines(&cinfo, &buffer, 1);
  }
  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);
  free(buffer);
  fclose(fp);
  
  fprintf(stderr, "[\x1b[32mOK\x1b[39m]\n");
  return;
}

void write_keypoint(Image img, Keypoint *key){
  fprintf(stderr, "\t%s writing .. ", img.outname);
  
  int x, y, i;
  FILE *gp;
  gp = popen("gnuplot", "w");
  if(gp == NULL || &img == NULL){
    fprintf(stderr, "[\x1b[31mNG\x1b[39m]\n");
    fprintf(stderr, "\t\tCannot write image.\n");
    return;
  }
  
  fprintf(gp, "set yrange [] reverse\n");
  fprintf(gp, "set format x ''\n");
  fprintf(gp, "set format y ''\n");
  fprintf(gp, "set size ratio %lf\n", img.height/(double)img.width);
  fprintf(gp, "set palette gray\n");
  fprintf(gp, "set xrange [0:%d]\n", img.width-1);
  fprintf(gp, "set yrange [0:%d]\n", img.height-1);
  fprintf(gp, "unset key\n");
  fprintf(gp, "unset colorbox\n");
  fprintf(gp, "plot '-' matrix with image,'-' w l\n"); 

  // plot image 
  for(y=0; y<img.height; y++){
    for(x=0; x<img.width; x++){
      fprintf(gp, "%f ", (double)img.graymap[y][x].g);
    }
    fprintf(gp, "\n");
  }
  fprintf(gp, "e\n");
  fprintf(gp, "e\n");
  
  // plot keypoint
  Keypoint *flg;
  flg = (Keypoint*)malloc(sizeof(Keypoint));
  flg = key;
  free(key);
  key = (Keypoint*)malloc(sizeof(Keypoint));
  while(flg!=NULL){
    double dlt = pow(2, flg->oct);
    double xo = dlt * flg->x;
    double yo = dlt * flg->y;
    double size = dlt * pow(2, (flg->scale - 1)/(double)S);
    fprintf(stderr, "%f\n", size);
    return;
    
    // circle
    for(i=0; i<20; i++){
      double x = xo+size*cos((i/19.0)*2*M_PI);
      double y = yo+size*sin((i/19.0)*2*M_PI);
        
      fprintf(gp, "%f %f\n", x, y);
    }
    fprintf(gp,"\n");
    
    key->oct = flg->oct;
    key->scale = flg->scale;
    key->x = flg->x;
    key->y = flg->y;
    key->next = (Keypoint*)malloc(sizeof(Keypoint));
    key = key->next;
    
    flg = flg->next;
  }
  fprintf(gp,"e\n");
  fflush(gp);
  fprintf(stderr, "[\x1b[32mOK\x1b[39m]\n");
  getchar();
  pclose(gp);  
  
  return;
}

