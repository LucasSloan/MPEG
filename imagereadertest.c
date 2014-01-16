#include "readrgb.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
  int width, height, components;
  unsigned* image;

  image = read_texture(argv[1], &width, &height, &components);

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      printf("Pixel %d %d: %x\n", i, j, image[i*width + j]);
    }
  }
}
