#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include "ops.h"

#define PATHNAME_SIZE 512

// search options 
int selects(const struct dirent *dir){
  char *buf;
  char ext[]=".jpg";
  
  if(dir->d_name[0] == '.')
    return 0;
  buf = strstr(dir->d_name, ext);
  if(buf == NULL)
    return 0;
  return 1;
}

// replace ext
void strrep(const char *src, char *dst, const char *ext){
  int n=strlen(src), i=0;
  char *p;
  p = (char*)malloc(sizeof(char)*PATHNAME_SIZE);
  strcpy(p, src);
  for(i=n-1; i>0; i--){
    if(p[i]=='.'){
      p[i]='\0';
      break;
    }
  }
  sprintf(dst, "%s%s", p, ext);
  free(p);
  return;
}

int OutputList(char *imdir, char *imlist, char *keylist){
  int i=0;
  struct dirent **list;
  FILE *fp, *fq;

  int r = scandir(imdir, &list, selects, alphasort);
  if(r==-1){
    return 0;
  }else{
    fp = fopen(imlist, "w");
    fq = fopen(keylist, "w");
    for(i=0; i<r; ++i){
      char path[PATHNAME_SIZE], path2[PATHNAME_SIZE];
      strcpy(path, imdir);
      strcat(path, "/");
      strcat(path, list[i]->d_name);
      strrep(path, path2, ".key");
      fprintf(fp, "%s\n", path);
      fprintf(fq, "%s\n", path2);
      free(list[i]);
    }
    fclose(fp);
    fclose(fq);
  }
  free(list);
  
  return 1;
}

