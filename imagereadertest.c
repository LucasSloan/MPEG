#include "readrgb.h"
#include <stdio.h>
#include <stdint.h>

int main(int argc, char* argv[]) {
  int width, height, components;
  unsigned* image;

  image = read_texture(argv[1], &width, &height, &components);

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      unsigned pixel = image[(height - i - 1) * width + j];
      uint8_t red, green, blue;
      red = pixel & 0x000000ff;
      pixel = pixel >> 8;
      green = pixel & 0x000000ff;
      pixel = pixel >> 8;
      blue = pixel & 0x000000ff;

      printf("Pixel %d %d: Red: %x Green: %x Blue: %x\n", i+1, j+1, red, green, blue);
    }
  }
}
