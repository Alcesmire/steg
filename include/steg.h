/*
 * Copyright (c) 2014 Erim Secla < alcesmire AT gmail DOT com >
 * Licence: GPLv3 (see steg.c)
 */


#ifndef STEN_H
#define STEN_H

#include "lodepng.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct Image Image;
struct Image {
  unsigned char* data;
  unsigned int width, height;
};

int main(int argc, char *argv[]);
void encodePng(const char* filename, Image image);
void decodePng(Image* image, const char* filename);
void hideData(const char* filename, Image image);
void revealData(const char* filename, Image image);
void diffImages(Image a, Image b);
void givehelp();

#endif
