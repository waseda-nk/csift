#ifndef _SIFT_H_
#define _SIFT_H_

#include "class.h"

void SIFT(Image img);

Image *allocate_image(uint32_t width, uint32_t height, int color_type);
void copy_image(Image *img, Image src);
Image *load_jpeg(const char *imname, const char *keyname);
void write_jpeg(Image img);
void write_keypoint(Image img, Keypoint *key);

Image *gaussian_filter(Image *src, double sig, double octave);
Image *difference_image(Image *A, Image *B);
void down_sampling(Image *src);
void to_grayscale(Image *img);

#endif //_SIFT_H_
