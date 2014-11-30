// vim: set fdm=marker:

// steg -- a tool for embedding data into PNG images with minimal alteration to
// the image content.

/*
 * Copyright (c) 2014 Erim Secla < alcesmire AT gmail DOT com >
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * 
 * I acknowledge my code and practices isn't the best. If you feel like
 * something is broken, if you miss a feature or similar feel free to submit a
 * patch at the github page:
 *    https://github.com/Alcesmire/steg
 * Thank you!
 * 
 */







#include "steg.h"

/**
 * Reverse the hiding, basically.
 **/
int main(int argc, char *argv[]) // {{{1
{
  if ( argc < 2) givehelp();
  
  if ( !strcmp(argv[1], "hide") )
  {
    if (argc != 5) givehelp();
    
    printf("Will try to hide your data.\n");
    
    const char* src = argv[2];
    const char* secret = argv[3];
    const char* dst = argv[4];
    
    Image image;
    decodePng(&image, src);
    hideData(secret,image);
    printf("Image processed.\n");
    encodePng(dst,image);
    free(image.data);
  }
  else if ( !strcmp(argv[1], "reveal") )
  {
    if (argc != 4) givehelp();
    
    printf("Will extract what could be data from file.\n");
    
    const char* src = argv[2];
    const char* secret = argv[3];
    
    Image image;
    decodePng(&image, src);
    revealData(secret,image);
    printf("Image processed.\n");
    free(image.data);
  }
  else if ( !strcmp(argv[1], "diff") )
  {
    if (argc != 5) givehelp();
    
    printf("Will extract what could be data from file.\n");
    
    const char* src1 = argv[2];
    const char* src2 = argv[3];
    const char* dst = argv[4];
    
    Image image1;
    Image image2;
    decodePng(&image1, src1);
    decodePng(&image2, src2);
    
    diffImages(image1, image2);
    printf("Image processed.\n");
    encodePng(dst,image1);
    free(image1.data);
    free(image2.data);
  }
  else
    givehelp();
  
  return 0;
}

void decodePng(Image* image, const char* filename) // {{{1
{
  unsigned error;
  unsigned char* png;
  size_t pngsize;

  lodepng_load_file(&png, &pngsize, filename);
  printf("Image loaded.\n");
  
  unsigned char* data;
  unsigned int width;
  unsigned int height;
  
  error = lodepng_decode32(&data, &width, &height, png, pngsize);
  if (error)
  {
    printf("error %u: %s\n", error, lodepng_error_text(error));
    exit(EXIT_FAILURE);
  }
  
  image->data = data;
  image->width = width;
  image->height = height;
  
  printf("Image decoded.\n");

  free(png);
}

/**
 * For encoding png files
 **/
void encodePng(const char* filename, Image image) // {{{1
{
  unsigned char* png;
  size_t pngsize;
  
  unsigned error = lodepng_encode32(&png, &pngsize, image.data, image.width, image.height);
  printf("Image encoded.\n");
  if(!error) lodepng_save_file(png, pngsize, filename);
  printf("Image saved.\n");

  /*if there's an error, display it*/
  if(error) {
    printf("error %u: %s\n", error, lodepng_error_text(error));
    exit(EXIT_FAILURE);
  }

  free(png);
}

void hideData(const char* filename, Image image) // {{{1
{
  FILE *fp = fopen(filename, "rb");
  int numPixels = image.height*image.width;
  
  // Find file size in bytes
  fseek(fp, 0L, SEEK_END);
  int secretSize = ftell(fp);
  fseek(fp, 0L, SEEK_SET);
  
  // Will your secret fit?
  int capacity = 3*(image.width*image.height/8) - 4;
  
  if (secretSize > capacity) {
    printf("The file you're trying to hide (%d bytes) is larger than the capacity of your image (%d bytes).\n", secretSize, capacity);
    exit(EXIT_FAILURE);
  }
  
  // Write the size, backwards to make reading a bit easier
  int i,index;
  for(i = 31; i >= 0; --i)
  {
    index = 4 * (i%numPixels) + i/numPixels;
    image.data[index] = (image.data[index] & 0b11111110) | secretSize&1;
    secretSize >>= 1;
  }
  
  // Write data byte by byte
  char data;
  int bitIndex = 32;
  while(fread(&data, 1, 1, fp))
  {
    for(i = 0; i < 8; ++i, ++bitIndex)
    {
      index = 4 * (bitIndex % numPixels) + bitIndex/numPixels;
      image.data[index] = (image.data[index] & 0b11111110) | data&1;
      data >>= 1;
    }
  }
  
  fclose(fp);
}

/**
 * For decoding png files
 **/
void revealData(const char* filename, Image image) // {{{1
{
  FILE *fp = fopen(filename, "wb");
  int numPixels = image.height*image.width;
  
  // Read the size
  int i,index;
  int secretSize = 0;
  for(i = 0; i < 32; ++i)
  {
    index = 4 * (i%numPixels) + i/numPixels;
    secretSize <<= 1;
    secretSize |= image.data[index] & 1;
  }
  
  // Read data byte by byte
  char data;
  int bitIndex = 32;
  while(secretSize--)
  {
    data = 0;
    for(i = 0; i < 8; ++i, ++bitIndex)
    {
      index = 4 * (bitIndex % numPixels) + bitIndex/numPixels;
      data += (image.data[index]&1)<<i;
    }
    fwrite(&data,1,1,fp);
  }
  fclose(fp);
}

/**
 * Show the changed channels between two images
 **/
void diffImages(Image a, Image b) // {{{1
{
  if (a.height != b.height || a.width != b.width)
  {
    printf("Your images have different dimensions, they can't be diffed.\n");
    exit(EXIT_FAILURE);
  }
  int size = 4*a.width*a.height;
  
  int i;
  for (i = 0; i < size; i += 4)
  {
    a.data[i]   = a.data[i]   != b.data[i]   ? 255 : 0;
    a.data[i+1] = a.data[i+1] != b.data[i+1] ? 255 : 0;
    a.data[i+2] = a.data[i+2] != b.data[i+2] ? 255 : 0;
  }
}
void givehelp() // {{{1
{
  printf("Usage:\n\tsteg hide original.png secret sneaky.png\n\tsteg reveal sneaky.png secret\n\tsteg diff original.png sneaky.png diff.png\n");
  exit(EXIT_FAILURE);
}

