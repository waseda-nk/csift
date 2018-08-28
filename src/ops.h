#ifndef _OPS_H_
#define _OPS_H_

#include <stdio.h>
#include <stdlib.h>

void DetectDirectory(char *argv1, char *argv2, char *imdir, char *resdir, char *curdir);
int OutputList(char *imdir, char *imlist, char *keylist);
void strrep(const char *src, char *dst, const char *ext);

#endif //_OPS_H_
