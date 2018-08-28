#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>
#include "ops.h"

#define PATHNAME_SIZE 512

void DetectDirectory(char *argv1, char *argv2, char *imdir, char *resdir, char *curdir){
  realpath(argv1, imdir);
  realpath(argv2, resdir);
  getcwd(curdir, PATHNAME_SIZE);
  
  struct stat stat_buffer;
  if(stat(imdir, &stat_buffer) == 0 && stat(resdir, &stat_buffer) == 0){
    fprintf(stderr, "[\x1b[32mOK\x1b[39m]\n");
    fprintf(stderr, "\tImage Directory\n\t\t%s\n", imdir);
    fprintf(stderr, "\tResult Directory\n\t\t%s\n", resdir);
    fprintf(stderr, "\tCurrent Directory\n\t\t%s\n", curdir);
  }else{
    fprintf(stderr, "[\x1b[31mNG\x1b[39m]\n");
    exit(1);
  }
  return;
}

