// This program run Structure from Motion
//  made by Nobuyuki Koizumi.
//   $1 ... image directory
//   $2 ... result directory

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ops.h"
#include "SIFT/SIFT.h"
#include "SIFT/class.h"

#define PATHNAME_SIZE 512

int main(int argc, char *argv[]){
  if(argc != 3){
    fprintf(stderr, "[\x1b[31mError\x1b[39m]\n");
    fprintf(stderr, "\tWrong Command\n");
    fprintf(stderr, "\t`./RunSfM <image-dir> <result-dir>`\n");
    exit(1);
  }
  char ImageDir[PATHNAME_SIZE];
  char ResultDir[PATHNAME_SIZE];
  char CurrentDir[PATHNAME_SIZE];
  char ImageList[PATHNAME_SIZE];
  char KeypointList[PATHNAME_SIZE];
  
  // Detect Directory
  fprintf(stderr, "[\x1b[32mRunSfM\x1b[39m] Detect Directory --- ");
  DetectDirectory(argv[1], argv[2], ImageDir, ResultDir, CurrentDir);
  
  // Output Image & Keypoint List
  fprintf(stderr, "[\x1b[32mRunSfM\x1b[39m] Write Image & KeypointList --- ");
  snprintf(ImageList, PATHNAME_SIZE, "%s%s", ResultDir, "/ImageList.txt");
  snprintf(KeypointList, PATHNAME_SIZE, "%s%s", ResultDir, "/KeypointList.txt");
  if(OutputList(ImageDir, ImageList, KeypointList)==1){
    fprintf(stderr, "[\x1b[32mOK\x1b[39m]\n");
  }else{
    fprintf(stderr, "[\x1b[31mNG\x1b[39m]\n");
    fprintf(stderr, "\tImage Directory is Wrong or does not include image file.\n");
  }
  
  // Run SIFT
  fprintf(stderr, "[\x1b[32mRunSfM\x1b[39m] Run SIFT --- \n");
  FILE *fp, *fq;
  char imname[PATHNAME_SIZE] = {'\0'}, keyname[PATHNAME_SIZE] = {'\0'};
  fp = fopen(ImageList, "r");
  fq = fopen(KeypointList, "r");
  while(fscanf(fp, "%s", imname) != EOF && 
        fscanf(fq, "%s", keyname) != EOF ) {
    Image *img = load_jpeg(imname, keyname);
    SIFT(*img);
    free(img);
  }
  fclose(fp);
  fclose(fq);
  return 0;
}
