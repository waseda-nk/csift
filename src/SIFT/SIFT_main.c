#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "class.h"
#include "SIFT.h"

// SIFT parameter
#define SIG0 1.6
#define S 3
#define MINW 20
#define PATHNAME_SIZE 512
#define Rth 10.0
#define TH_EDGE (Rth+1)*(Rth+1)/Rth
#define TH_POW 0.03

void extract_keypoints(Image *src, Image **L, Image **DoG, int octave, Keypoint *key){
  fprintf(stderr, "\t\t[extract keypoints]\n");
  int i, s;
  double k = pow(2, 1/(double)S);
  
  // Generate Smoothing Images
  for(i=0; i<octave; i++){
    double sig = SIG0;
    for(s=0; s<S+3; s++){
      L[i][s] = *gaussian_filter(src, sig, octave);
      sig *= k;
      
      // write jpeg
      //char outname[PATHNAME_SIZE] = {'\0'};
      //char filter[PATHNAME_SIZE] = {'\0'};
      //sprintf(filter, "_filtered_%04d_%04d_%02d.jpg", L[i][s].width, L[i][s].height, s); 
      //strrep(src->imname, outname, filter);
      //strcpy(L[i][s].outname, outname);
      //write_jpeg(L[i][s]);
    }
    down_sampling(src);
  }
  
  // Generate DoG Images
  for(i=0; i<octave; i++){
    for(s=0; s<S+2; s++){
      //DoG[i][s] = L[i][s+1] - L[i][s]
      DoG[i][s] = *difference_image(&L[i][s+1], &L[i][s]);
      
      // write jpeg
      //char outname[PATHNAME_SIZE] = {'\0'};
      //char filter[PATHNAME_SIZE] = {'\0'};
      //sprintf(filter, "_DoG_%04d_%04d_%02d.jpg", DoG[i][s].width, DoG[i][s].height, s); 
      //strrep(src->imname, outname, filter);
      //strcpy(DoG[i][s].outname, outname);
      //write_jpeg(DoG[i][s]);
    }
  }
  
  // extract keypoint
  int x, y, W, H, ds, dx, dy, flag;
  int **flg;
  flg = (int**)malloc(DoG[0][0].width * sizeof(int*));
  for(x=0; x<DoG[0][0].width; x++)
    flg[x] = (int*)malloc(DoG[0][0].height * sizeof(int));
  
  // extract maxima & minima
  for(i=0; i<octave; i++){
    W = DoG[i][0].width;
    H = DoG[i][0].height;
    // initialize flag
    for(x=0; x<W-1; x++)
      for(y=0; y<H-1; y++)
        flg[x][y]=0;
    for(s=0; s<S+1; s++){
      for(x=0; x<W-1; x++){
        for(y=0; y<H-1; y++){
          if(flg[x][y]==1) continue;
          int is_max = 1;
          int is_min = 1;
          
          // search near 26
          flag=0;
          for(ds=s-1; ds<=s+1; ds++){
            for(dx=x-1; dx<=x+1; dx++){
              for(dy=y-1; dy<=y+1; dy++){
                if((ds==s && dx==x && dy==y) || flag==1) continue;
                if(ds<0 || S<=ds) continue;
                if(dx<0 || W<=dx) continue;
                if(dy<0 || H<=dy) continue;
                if(is_max==1 && (DoG[i][s].graymap[y][x].g <= DoG[i][ds].graymap[dy][dx].g)) is_max = 0;
                if(is_min==1 && (DoG[i][s].graymap[y][x].g >= DoG[i][ds].graymap[dy][dx].g)) is_min = 0;
                if(is_max!=1 && is_min!=1) flag=1;
              }
            }
          }
          if(flag!=1){
            key->x = (double)x;
            key->y = (double)y;
            key->oct = i;
            key->scale = s;
            key->next = (Keypoint*)malloc(sizeof(Keypoint));
            key = key->next;
            flg[x][y]=1;
          }
        }
      }
    }
  }
  
  free(flg);
  return;
}

void calc_inverse(double matrix[3][3], double inverse[3][3]){
  int i, j, k;
  double buf;
  
  //initialize
  for(i=0; i<3; i++){
    for(j=0; j<3; j++) inverse[i][j]=0;
    inverse[i][i]=1;
  }
  
  for(i=0; i<3; i++){
    buf = 1/matrix[i][i];
    for(j=0; j<3; j++){
      matrix[i][j]*=buf;
      inverse[i][j]*=buf;
    }
    for(j=0; j<3; j++){
      if(i!=j){
        buf=matrix[j][i];
        for(k=0; k<3; k++){
          matrix[j][k]-=matrix[i][k]*buf;
          inverse[j][k]-=inverse[i][k]*buf;
        }
      }
    }
  }
  return;
}

void localize_keypoints(Image **DoG, Keypoint *key){
  fprintf(stderr, "\t\t[localize keypoints]\n");
  Keypoint *flg;
  flg = (Keypoint*)malloc(sizeof(Keypoint));
  flg = key;
  free(key);
  key = (Keypoint*)malloc(sizeof(Keypoint));
  
  double mD[3][3], iD[3][3], xD[3], X[3];
  int i=0;
  while(flg != NULL){
    fprintf(stderr, "%i  ", i); 
    int o = flg->oct;
    int s = flg->scale;
    int x = (int)flg->x;
    int y = (int)flg->y;
    
    if(x-2<0 || x+2>=DoG[o][s].width || y-2<0 || y+2>=DoG[o][s].height){
      continue;
    }
    double Dxx = (DoG[o][s].graymap[y][x-2].g + DoG[o][s].graymap[y][x+2].g - 2*DoG[o][s].graymap[y][x].g);
    double Dyy = (DoG[o][s].graymap[y-2][x].g + DoG[o][s].graymap[y+2][x].g - 2*DoG[o][s].graymap[y][x].g);
    double Dxy = (DoG[o][s].graymap[y-1][x-1].g - DoG[o][s].graymap[y+1][x-1].g - DoG[o][s].graymap[y-1][x+1].g + DoG[o][s].graymap[y+1][x+1].g);
    
    double trc = Dxx + Dyy;
    double det = Dxx * Dyy - Dxy * Dxy;
    
    // delete keypoints
    if(trc*trc/det >= TH_EDGE){
      continue;
    }
    
    // delete low contrast keypoint
    int sm1 = (s-1<0) ? 0 : s-1;
    int sm2 = (s-2<0) ? 0 : s-2;
    int sp1 = (s+1>=S+2) ? S+1 : s+1;
    int sp2 = (s+2>=S+2) ? S+1 : s+2;
    
    // subpixel estimation
    double Dx = DoG[o][s].graymap[y][x-1].g - DoG[o][s].graymap[y][x+1].g;
    double Dy = DoG[o][s].graymap[y-1][x].g - DoG[o][s].graymap[y+1][x].g;
    double Ds = DoG[o][sm1].graymap[y][x].g - DoG[o][sp1].graymap[y][x].g;
    
    double Dss = DoG[o][sm2].graymap[y][x].g - DoG[o][sp2].graymap[y][x].g + 2*DoG[o][s].graymap[y][x].g;
    double Dxs = DoG[o][sm1].graymap[y][x-1].g - DoG[o][sm1].graymap[y][x+1].g - DoG[o][sp1].graymap[y][x-1].g - DoG[o][sp1].graymap[y][x+1].g;
    double Dys = DoG[o][sm1].graymap[y-1][x].g - DoG[o][sm1].graymap[y+1][x].g - DoG[o][sp1].graymap[y-1][x].g - DoG[o][sp1].graymap[y+1][x].g;
    
    mD[0][0]=Dxx; mD[0][1]=Dxy; mD[0][2]=Dxs;
    mD[1][0]=Dxy; mD[1][1]=Dyy; mD[1][2]=Dys;
    mD[2][0]=Dxs; mD[2][1]=Dys; mD[2][2]=Dss;    
    xD[0]=-Dx; xD[1]=-Dy; xD[2]=-Ds;
    calc_inverse(mD, iD);
    
    X[0]=iD[0][0]*xD[0]+iD[0][1]*xD[1]+iD[0][2]*xD[2];
    X[1]=iD[1][0]*xD[0]+iD[1][1]*xD[1]+iD[1][2]*xD[2];
    X[2]=iD[2][0]*xD[0]+iD[2][1]*xD[1]+iD[2][2]*xD[2];
    
    double Dpow = fabs(DoG[o][s].graymap[y][x].g+(xD[0]*X[0]+xD[1]*X[1]+xD[2]*X[2])/2);
    
    if(Dpow < TH_POW){
      continue;
    }
    
    key->oct = flg->oct;
    key->scale = flg->scale;
    key->x = flg->x;
    key->y = flg->y;
    key->next = (Keypoint*)malloc(sizeof(Keypoint));
    key = key->next;
    
    flg = flg->next;
    i++;
  }
  return;
}

void SIFT(Image img){
  int W, i, octave;
  Image *src = NULL;
  if((src = allocate_image(img.width, img.height, img.color_type))==NULL)
    return;
  copy_image(src, img);
  // decide octave
  W = (src->width < src->height) ? src->width : src->height;
  for(octave=0; W>MINW; octave++)
    W/=2;
  
  // buffer
  Image **L;
  L = (Image**)calloc(octave, sizeof(Image*));
  for(i=0; i<octave; i++)
    L[i] = (Image*)calloc(S+3, sizeof(Image));
  
  Image **DoG;
  DoG = (Image**)calloc(octave, sizeof(Image*));
  for(i=0; i<octave; i++)
    DoG[i] = (Image*)calloc(S+2, sizeof(Image));
  
  // extract keypoint
  Keypoint *key, *start;
  key = (Keypoint*)malloc(sizeof(Keypoint));
  start = key;
  extract_keypoints(src, L, DoG, octave, key);
  key = start;
  
  // localize keypoints
  //localize_keypoints(DoG, key);
  write_keypoint(img, key);
  
  // free
  for(i=0; i<octave; i++){
    free(L[i]);
    free(DoG[i]);
  }
  free(L);
  free(DoG);
  
  return;
}
