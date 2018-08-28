#ifndef _CLASS_H_
#define _CLASS_H_

#include <stdint.h>

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
} color_t;

typedef struct {
  uint8_t g;
} gray_t;

typedef struct {
  char *imname;
  char *keyname;
  char *outname;
  uint32_t width;
  uint32_t height;
  uint16_t color_type;  // 1..rgb;  2..grayscale;
  color_t **map;        // if grayscale, not use
  gray_t **graymap;     // if rgb, not use
} Image;

typedef struct _Keypoint{
  double x;
  double y;
  
  int oct;
  int scale;
  struct _Keypoint *next;
} Keypoint;

#endif //_CLASS_H_
