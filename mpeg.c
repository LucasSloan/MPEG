#include "readrgb.h"


int length[10] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512};

  int width, height, components;
  unsigned* image;

  image = read_texture(argv[1], &width, &height, &components);

  RGB* image = *argb;
  RGB pixel;

  /* Allocate memory for the JPEG color space, both
   * precisely as floats and later as rounded integers. */
  float* Y = malloc(sizeof(float) * width * height);
  float* Cb = malloc(sizeof(float) * width * height/4);
  float* Cr = malloc(sizeof(float) * width * height/4);
  int32_t* Yout = malloc(sizeof(int32_t) * width * height);
  int32_t* Cbout = malloc(sizeof(int32_t) * width * height/4);
  int32_t* Crout = malloc(sizeof(int32_t) * width * height/4);

  gettimeofday(&tv1, 0);
  /* JPEG uses a non-RGB color space.  Y stores greyscale
   * information, while Cb and Cr store color offsets. */
  for (int row = 0; row < *height; row++) {
    for (int col = 0; col < *width; col++) {
      pixel = image[row * *width + col];
      Y[row * width + col] = 0.299*pixel.red + 0.587*pixel.green + 0.114*pixel.blue;
      Cb[row*2/2 * width + col*2/2] += 128 - 0.168736*pixel.red - 0.331264*pixel.green + 0.5*pixel.blue;
      Cr[row*2/2 * width + col*2/2] += 128 + 0.5*pixel.red - 0.418688*pixel.green - 0.081312*pixel.blue;
    }
  }
  gettimeofday(&tv2, 0);

  time = tv2.tv_sec - tv1.tv_sec + 1e-6 * (tv2.tv_usec - tv1.tv_usec);

  printf("Transform colorspace: %f seconds.\n", time);

  float cosvals[8][8];

  /* Calculating cosines is expensive, and there
   * are only 64 cosines that need to be calculated
   * so precompute them and cache. */
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      cosvals[i][j] = cos(PI/8.0*(i+0.5d)*j);
    }
  }

  __m256 quant;
  float lquant[8][8], cquant[8][8], qrow[8];

  /* JPEG uses a quantization table to reduce the number
   * of bits necessary to store the DCT frequencies as
   * well as zero out frequencies that aren't important.
   * The standard form of these tables is in a zigzag, which
   * needs to be undone so that the AVX instructions can make
   * use of them. */
  for (int rrow = 0;  rrow < 8; rrow++) {
    for (int ccol = 0; ccol < 8; ccol++) {
      int pos = zigzag[rrow*8 + ccol];
      lquant[pos/8][pos%8] = s_std_lum_quant[rrow*8+ccol];
      cquant[pos/8][pos%8] = s_std_croma_quant[rrow*8+ccol];
    }
  }

  gettimeofday(&tv1, 0);
  /* Separate the parallel from the for, so each processor gets its
   * own copy of the buffers and variables. */
#pragma omp parallel
{
  float avload[8] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
  avload[0] =  sqrt(1.0/8.0);
  __m256 row0, row1, row2, row3, row4, row5, row6, row7;
  __m256 loader;
  __m256 temp;
  __m256 minus128 = _mm256_set1_ps(-128.0);
  __m256 av = _mm256_loadu_ps(&avload[0]), au1 = _mm256_broadcast_ss(&avload[0]), au2 = _mm256_broadcast_ss(&avload[1]);
  __m256 avxcos;
  __m256i integer;

  float writer[8];
  int iwriter[8];

  /* The DCT breaks the image into 8 by 8 blocks and then
   * transforms them into color frequencies. */
#pragma omp for
  for (int brow = 0; brow < *height/8; brow++) {
    for (int bcol = 0; bcol < *width/8; bcol++) {
      int head_pointer = bcol*8 + brow * 8 * *width;
      row0 = _mm256_setzero_ps();
      row1 = _mm256_setzero_ps();
      row2 = _mm256_setzero_ps();
      row3 = _mm256_setzero_ps();
      row4 = _mm256_setzero_ps();
      row5 = _mm256_setzero_ps();
      row6 = _mm256_setzero_ps();
      row7 = _mm256_setzero_ps();

      /* This pair of loops uses AVX instuctions to add the frequency
       * component from each pixel to all of the buckets at once.  Allows
       * us to do the DCT on a block in 64 iterations of a loop rather
       * than 64 iterations of 64 iterations of a loop (all 64 pixels affect
       * all 64 frequencies) */
      for (int x = 0; x < 8; x++) {
	for (int y = 0; y < 8; y++) {
	  loader = _mm256_broadcast_ss(&Y[head_pointer+x+(y * *width)]);
          loader = _mm256_add_ps(loader, minus128);
          loader = _mm256_mul_ps(loader, av);
          avxcos = _mm256_loadu_ps(&cosvals[x][0]);
	  loader = _mm256_mul_ps(loader, avxcos);

	    temp = loader;
	    avxcos = _mm256_broadcast_ss(&cosvals[y][0]);
	    temp = _mm256_mul_ps(temp, au1);
	    temp = _mm256_mul_ps(temp, avxcos);
	    row0 = _mm256_add_ps(row0, temp);

	    loader = _mm256_mul_ps(loader, au2);

	    temp = loader;
	    avxcos = _mm256_broadcast_ss(&cosvals[y][1]);
	    temp = _mm256_mul_ps(temp, avxcos);
	    row1 = _mm256_add_ps(row1, temp);

	    temp = loader;
	    avxcos = _mm256_broadcast_ss(&cosvals[y][2]);
	    temp = _mm256_mul_ps(temp, avxcos);
	    row2 = _mm256_add_ps(row2, temp);

	    temp = loader;
	    avxcos = _mm256_broadcast_ss(&cosvals[y][3]);
	    temp = _mm256_mul_ps(temp, avxcos);
	    row3 = _mm256_add_ps(row3, temp);

	    temp = loader;
	    avxcos = _mm256_broadcast_ss(&cosvals[y][4]);
	    temp = _mm256_mul_ps(temp, avxcos);
	    row4 = _mm256_add_ps(row4, temp);

	    temp = loader;
	    avxcos = _mm256_broadcast_ss(&cosvals[y][5]);
	    temp = _mm256_mul_ps(temp, avxcos);
	    row5 = _mm256_add_ps(row5, temp);

	    temp = loader;
	    avxcos = _mm256_broadcast_ss(&cosvals[y][6]);
	    temp = _mm256_mul_ps(temp, avxcos);
	    row6 = _mm256_add_ps(row6, temp);

	    temp = loader;
	    avxcos = _mm256_broadcast_ss(&cosvals[y][7]);
	    temp = _mm256_mul_ps(temp, avxcos);
	    row7 = _mm256_add_ps(row7, temp);

	}
      }

      /* Each frequency stored as a float needs to be divided by
       * the quantization value, then rounded to the nearest integer.
       * Also changes the order of the values from pixel order to
       * each 8 by 8 block stored one after another. */
      temp = _mm256_loadu_ps(&lquant[0][0]);
      row0 = _mm256_div_ps(row0, temp);
      row0 = _mm256_round_ps(row0, _MM_FROUND_TO_NEAREST_INT);
      integer = _mm256_cvttps_epi32(row0);
      _mm256_storeu_si256(Yout+((bcol/2*2) + (brow/2*2) * (*width/8)+ ((bcol%2 == 0) ? 2 : 0) + ((brow%2 == 0) ? 1 : 0))*64, integer);

      temp = _mm256_loadu_ps(&lquant[1][0]);
      row1 = _mm256_div_ps(row1, temp);
      row1 = _mm256_round_ps(row1, _MM_FROUND_TO_NEAREST_INT);
      integer = _mm256_cvttps_epi32(row1);
      _mm256_storeu_si256(Yout+8+((bcol/2*2) + (brow/2*2) * (*width/8)+ ((bcol%2 == 0) ? 2 : 0) + ((brow%2 == 0) ? 1 : 0))*64, integer);

      temp = _mm256_loadu_ps(&lquant[2][0]);
      row2 = _mm256_div_ps(row2, temp);
      row2 = _mm256_round_ps(row2, _MM_FROUND_TO_NEAREST_INT);
      integer = _mm256_cvttps_epi32(row2);
      _mm256_storeu_si256(Yout+16+((bcol/2*2) + (brow/2*2) * (*width/8)+ ((bcol%2 == 0) ? 2 : 0) + ((brow%2 == 0) ? 1 : 0))*64, integer);

      temp = _mm256_loadu_ps(&lquant[3][0]);
      row3 = _mm256_div_ps(row3, temp);
      row3 = _mm256_round_ps(row3, _MM_FROUND_TO_NEAREST_INT);
      integer = _mm256_cvttps_epi32(row3);
      _mm256_storeu_si256(Yout+24+((bcol/2*2) + (brow/2*2) * (*width/8)+ ((bcol%2 == 0) ? 2 : 0) + ((brow%2 == 0) ? 1 : 0))*64, integer);

      temp = _mm256_loadu_ps(&lquant[4][0]);
      row4 = _mm256_div_ps(row4, temp);
      row4 = _mm256_round_ps(row4, _MM_FROUND_TO_NEAREST_INT);
      integer = _mm256_cvttps_epi32(row4);
      _mm256_storeu_si256(Yout+32+((bcol/2*2) + (brow/2*2) * (*width/8)+ ((bcol%2 == 0) ? 2 : 0) + ((brow%2 == 0) ? 1 : 0))*64, integer);

      temp = _mm256_loadu_ps(&lquant[5][0]);
      row5 = _mm256_div_ps(row5, temp);
      row5 = _mm256_round_ps(row5, _MM_FROUND_TO_NEAREST_INT);
      integer = _mm256_cvttps_epi32(row5);
      _mm256_storeu_si256(Yout+40+((bcol/2*2) + (brow/2*2) * (*width/8)+ ((bcol%2 == 0) ? 2 : 0) + ((brow%2 == 0) ? 1 : 0))*64, integer);

      temp = _mm256_loadu_ps(&lquant[6][0]);
      row6 = _mm256_div_ps(row6, temp);
      row6 = _mm256_round_ps(row6, _MM_FROUND_TO_NEAREST_INT);
      integer = _mm256_cvttps_epi32(row6);
      _mm256_storeu_si256(Yout+48+((bcol/2*2) + (brow/2*2) * (*width/8))*64, integer);

      temp = _mm256_loadu_ps(&lquant[7][0]);
      row7 = _mm256_div_ps(row7, temp);
      row7 = _mm256_round_ps(row7, _MM_FROUND_TO_NEAREST_INT);
      integer = _mm256_cvttps_epi32(row7);
      _mm256_storeu_si256(Yout+56+((bcol/2*2) + (brow/2*2) * (*width/8)+ ((bcol%2 == 0) ? 2 : 0) + ((brow%2 == 0) ? 1 : 0))*64, integer);

      
    }
  }

  /* Do the same for the other color values (Cb and Cr).*/
#pragma omp for
    for (int brow = 0; brow < *height/8; brow++) {
      for (int bcol = 0; bcol < *width/8; bcol++) {
	int head_pointer = bcol*8 + brow * 8 * *width;
	row0 = _mm256_setzero_ps();
	row1 = _mm256_setzero_ps();
	row2 = _mm256_setzero_ps();
	row3 = _mm256_setzero_ps();
	row4 = _mm256_setzero_ps();
	row5 = _mm256_setzero_ps();
	row6 = _mm256_setzero_ps();
	row7 = _mm256_setzero_ps();
	for (int x = 0; x < 8; x++) {
	  for (int y = 0; y < 8; y++) {
	    loader = _mm256_broadcast_ss(&Cb[head_pointer+x+(y * *width)]);
	    loader = _mm256_add_ps(loader, minus128);
	    loader = _mm256_mul_ps(loader, av);
	    avxcos = _mm256_loadu_ps(&cosvals[x][0]);
	    loader = _mm256_mul_ps(loader, avxcos);

	    temp = loader;
	    avxcos = _mm256_broadcast_ss(&cosvals[y][0]);
	    temp = _mm256_mul_ps(temp, au1);
	    temp = _mm256_mul_ps(temp, avxcos);
	    row0 = _mm256_add_ps(row0, temp);

	    temp = loader;
	    avxcos = _mm256_broadcast_ss(&cosvals[y][1]);
	    temp = _mm256_mul_ps(temp, au2);
	    temp = _mm256_mul_ps(temp, avxcos);
	    row1 = _mm256_add_ps(row1, temp);

	    temp = loader;
	    avxcos = _mm256_broadcast_ss(&cosvals[y][2]);
	    temp = _mm256_mul_ps(temp, au2);
	    temp = _mm256_mul_ps(temp, avxcos);
	    row2 = _mm256_add_ps(row2, temp);

	    temp = loader;
	    avxcos = _mm256_broadcast_ss(&cosvals[y][3]);
	    temp = _mm256_mul_ps(temp, au2);
	    temp = _mm256_mul_ps(temp, avxcos);
	    row3 = _mm256_add_ps(row3, temp);

	    temp = loader;
	    avxcos = _mm256_broadcast_ss(&cosvals[y][4]);
	    temp = _mm256_mul_ps(temp, au2);
	    temp = _mm256_mul_ps(temp, avxcos);
	    row4 = _mm256_add_ps(row4, temp);

	    temp = loader;
	    avxcos = _mm256_broadcast_ss(&cosvals[y][5]);
	    temp = _mm256_mul_ps(temp, au2);
	    temp = _mm256_mul_ps(temp, avxcos);
	    row5 = _mm256_add_ps(row5, temp);

	    temp = loader;
	    avxcos = _mm256_broadcast_ss(&cosvals[y][6]);
	    temp = _mm256_mul_ps(temp, au2);
	    temp = _mm256_mul_ps(temp, avxcos);
	    row6 = _mm256_add_ps(row6, temp);

	    temp = loader;
	    avxcos = _mm256_broadcast_ss(&cosvals[y][7]);
	    temp = _mm256_mul_ps(temp, au2);
	    temp = _mm256_mul_ps(temp, avxcos);
	    row7 = _mm256_add_ps(row7, temp);
	  }
	}
      temp = _mm256_loadu_ps(&cquant[0][0]);
      row0 = _mm256_div_ps(row0, temp);
      row0 = _mm256_round_ps(row0, _MM_FROUND_TO_NEAREST_INT);
      integer = _mm256_cvttps_epi32(row0);
      _mm256_storeu_si256(Cbout+(bcol + brow * (*width/8))*64, integer);

      temp = _mm256_loadu_ps(&cquant[1][0]);
      row1 = _mm256_div_ps(row1, temp);
      row1 = _mm256_round_ps(row1, _MM_FROUND_TO_NEAREST_INT);
      integer = _mm256_cvttps_epi32(row1);
      _mm256_storeu_si256(Cbout+8+(bcol + brow * (*width/8))*64, integer);

      temp = _mm256_loadu_ps(&cquant[2][0]);
      row2 = _mm256_div_ps(row2, temp);
      row2 = _mm256_round_ps(row2, _MM_FROUND_TO_NEAREST_INT);
      integer = _mm256_cvttps_epi32(row2);
      _mm256_storeu_si256(Cbout+16+(bcol + brow * (*width/8))*64, integer);

      temp = _mm256_loadu_ps(&cquant[3][0]);
      row3 = _mm256_div_ps(row3, temp);
      row3 = _mm256_round_ps(row3, _MM_FROUND_TO_NEAREST_INT);
      integer = _mm256_cvttps_epi32(row3);
      _mm256_storeu_si256(Cbout+24+(bcol + brow * (*width/8))*64, integer);

      temp = _mm256_loadu_ps(&cquant[4][0]);
      row4 = _mm256_div_ps(row4, temp);
      row4 = _mm256_round_ps(row4, _MM_FROUND_TO_NEAREST_INT);
      integer = _mm256_cvttps_epi32(row4);
      _mm256_storeu_si256(Cbout+32+(bcol + brow * (*width/8))*64, integer);

      temp = _mm256_loadu_ps(&cquant[5][0]);
      row5 = _mm256_div_ps(row5, temp);
      row5 = _mm256_round_ps(row5, _MM_FROUND_TO_NEAREST_INT);
      integer = _mm256_cvttps_epi32(row5);
      _mm256_storeu_si256(Cbout+40+(bcol + brow * (*width/8))*64, integer);

      temp = _mm256_loadu_ps(&cquant[6][0]);
      row6 = _mm256_div_ps(row6, temp);
      row6 = _mm256_round_ps(row6, _MM_FROUND_TO_NEAREST_INT);
      integer = _mm256_cvttps_epi32(row6);
      _mm256_storeu_si256(Cbout+48+(bcol + brow * (*width/8))*64, integer);

      temp = _mm256_loadu_ps(&cquant[7][0]);
      row7 = _mm256_div_ps(row7, temp);
      row7 = _mm256_round_ps(row7, _MM_FROUND_TO_NEAREST_INT);
      integer = _mm256_cvttps_epi32(row7);
      _mm256_storeu_si256(Cbout+56+(bcol + brow * (*width/8))*64, integer);
      }
    }
#pragma omp for
    for (int brow = 0; brow < *height/8; brow++) {
      for (int bcol = 0; bcol < *width/8; bcol++) {
	int head_pointer = bcol*8 + brow * 8 * *width;
	row0 = _mm256_setzero_ps();
	row1 = _mm256_setzero_ps();
	row2 = _mm256_setzero_ps();
	row3 = _mm256_setzero_ps();
	row4 = _mm256_setzero_ps();
	row5 = _mm256_setzero_ps();
	row6 = _mm256_setzero_ps();
	row7 = _mm256_setzero_ps();
	for (int x = 0; x < 8; x++) {
	  for (int y = 0; y < 8; y++) {
	    loader = _mm256_broadcast_ss(&Cr[head_pointer+x+(y * *width)]);
	    loader = _mm256_add_ps(loader, minus128);
	    loader = _mm256_mul_ps(loader, av);
	    avxcos = _mm256_loadu_ps(&cosvals[x][0]);
	    loader = _mm256_mul_ps(loader, avxcos);

	    temp = loader;
	    avxcos = _mm256_broadcast_ss(&cosvals[y][0]);
	    temp = _mm256_mul_ps(temp, au1);
	    temp = _mm256_mul_ps(temp, avxcos);
	    row0 = _mm256_add_ps(row0, temp);

	    temp = loader;
	    avxcos = _mm256_broadcast_ss(&cosvals[y][1]);
	    temp = _mm256_mul_ps(temp, au2);
	    temp = _mm256_mul_ps(temp, avxcos);
	    row1 = _mm256_add_ps(row1, temp);

	    temp = loader;
	    avxcos = _mm256_broadcast_ss(&cosvals[y][2]);
	    temp = _mm256_mul_ps(temp, au2);
	    temp = _mm256_mul_ps(temp, avxcos);
	    row2 = _mm256_add_ps(row2, temp);

	    temp = loader;
	    avxcos = _mm256_broadcast_ss(&cosvals[y][3]);
	    temp = _mm256_mul_ps(temp, au2);
	    temp = _mm256_mul_ps(temp, avxcos);
	    row3 = _mm256_add_ps(row3, temp);

	    temp = loader;
	    avxcos = _mm256_broadcast_ss(&cosvals[y][4]);
	    temp = _mm256_mul_ps(temp, au2);
	    temp = _mm256_mul_ps(temp, avxcos);
	    row4 = _mm256_add_ps(row4, temp);

	    temp = loader;
	    avxcos = _mm256_broadcast_ss(&cosvals[y][5]);
	    temp = _mm256_mul_ps(temp, au2);
	    temp = _mm256_mul_ps(temp, avxcos);
	    row5 = _mm256_add_ps(row5, temp);

	    temp = loader;
	    avxcos = _mm256_broadcast_ss(&cosvals[y][6]);
	    temp = _mm256_mul_ps(temp, au2);
	    temp = _mm256_mul_ps(temp, avxcos);
	    row6 = _mm256_add_ps(row6, temp);

	    temp = loader;
	    avxcos = _mm256_broadcast_ss(&cosvals[y][7]);
	    temp = _mm256_mul_ps(temp, au2);
	    temp = _mm256_mul_ps(temp, avxcos);
	    row7 = _mm256_add_ps(row7, temp);
	  }
	}
	temp = _mm256_loadu_ps(&cquant[0][0]);
	row0 = _mm256_div_ps(row0, temp);
	row0 = _mm256_round_ps(row0, _MM_FROUND_TO_NEAREST_INT);
	integer = _mm256_cvttps_epi32(row0);
	_mm256_storeu_si256(Crout+(bcol + brow * (*width/8))*64, integer);
	
	temp = _mm256_loadu_ps(&cquant[1][0]);
	row1 = _mm256_div_ps(row1, temp);
	row1 = _mm256_round_ps(row1, _MM_FROUND_TO_NEAREST_INT);
	integer = _mm256_cvttps_epi32(row1);
	_mm256_storeu_si256(Crout+8+(bcol + brow * (*width/8))*64, integer);

	temp = _mm256_loadu_ps(&cquant[2][0]);
	row2 = _mm256_div_ps(row2, temp);
	row2 = _mm256_round_ps(row2, _MM_FROUND_TO_NEAREST_INT);
	integer = _mm256_cvttps_epi32(row2);
	_mm256_storeu_si256(Crout+16+(bcol + brow * (*width/8))*64, integer);

	temp = _mm256_loadu_ps(&cquant[3][0]);
	row3 = _mm256_div_ps(row3, temp);
	row3 = _mm256_round_ps(row3, _MM_FROUND_TO_NEAREST_INT);
	integer = _mm256_cvttps_epi32(row3);
	_mm256_storeu_si256(Crout+24+(bcol + brow * (*width/8))*64, integer);

	temp = _mm256_loadu_ps(&cquant[4][0]);
	row4 = _mm256_div_ps(row4, temp);
	row4 = _mm256_round_ps(row4, _MM_FROUND_TO_NEAREST_INT);
	integer = _mm256_cvttps_epi32(row4);
	_mm256_storeu_si256(Crout+32+(bcol + brow * (*width/8))*64, integer);

	temp = _mm256_loadu_ps(&cquant[5][0]);
	row5 = _mm256_div_ps(row5, temp);
	row5 = _mm256_round_ps(row5, _MM_FROUND_TO_NEAREST_INT);
	integer = _mm256_cvttps_epi32(row5);
	_mm256_storeu_si256(Crout+40+(bcol + brow * (*width/8))*64, integer);

	temp = _mm256_loadu_ps(&cquant[6][0]);
	row6 = _mm256_div_ps(row6, temp);
	row6 = _mm256_round_ps(row6, _MM_FROUND_TO_NEAREST_INT);
	integer = _mm256_cvttps_epi32(row6);
	_mm256_storeu_si256(Crout+48+(bcol + brow * (*width/8))*64, integer);

	temp = _mm256_loadu_ps(&cquant[7][0]);
	row7 = _mm256_div_ps(row7, temp);
	row7 = _mm256_round_ps(row7, _MM_FROUND_TO_NEAREST_INT);
	integer = _mm256_cvttps_epi32(row7);
	_mm256_storeu_si256(Crout+56+(bcol + brow * (*width/8))*64, integer);
      }
    }
  }


  free(Y);
  free(Cb);
  free(Cr);

  gettimeofday(&tv2, 0);

  time = tv2.tv_sec - tv1.tv_sec + 1e-6 * (tv2.tv_usec - tv1.tv_usec);

  printf("DCT: %f seconds.\n", time);

  int run = 0;
  int amplitude;
  int* sout;
  int counter = 8*(buffersize+bufferoverflow); //initialized to buffer size, in bits
  uint8_t* buffer = malloc(sizeof(uint8_t) * (buffersize+bufferoverflow));
  uint8_t btemp = 0;
  int lastdc[3] = {0, 0, 0};

  for (int i = 0; i < 8; i++) {
    buffer[i] = 0;
  }


uint16_t dc_lum_code[9] = {0x0004, 0x0000, 0x0001, 0x0005, 0x0006, 0x000e, 0x001e, 0x003e, 0x007e};
uint16_t dc_chroma_code[9] = {0x0000, 0x0001, 0x0002, 0x0006, 0x000e, 0x001e, 0x003e, 0x007e, 0x00fe};

uint8_t dc_lum_size[9] = {3, 2, 2, 3, 3, 4, 5, 6, 7};
uint8_t dc_chroma_size[9] = {2, 2, 2, 3, 4, 5, 6, 7, 8};

uint16_t* code;
uint8_t* size;

multibitwriter(&counter, buffer, &btemp, fp, 0x0000, 16);
multibitwriter(&counter, buffer, &btemp, fp, 0x0100, 16); //picture start code
multibitwriter(&counter, buffer, &btemp, fp, picnum%1024, 10); //picture temporal reference
multibitwriter(&counter, buffer, &btemp, fp, 0x0001, 3); //picture coded as an intra picture
multibitwriter(&counter, buffer, &btemp, fp, 0x0FFF, 1); //FIX ME vbv_delay deals with buffering of encoded data, presumably should set with some estimate of bit rate, set it fairly high on the assumption modern computers have big ass memory stores


multibitwriter(&counter, buffer, &btemp, fp, 0x0000, 16);
multibitwriter(&counter, buffer, &btemp, fp, 0x0101, 16); //Slice start code
multibitwriter(&counter, buffer, &btemp, fp, 0x0001, 5); //Quantization scale
multibitwriter(&counter, buffer, &btemp, fp, 0x0000, 1); // no extra info in MPEG1
  for (int z = 0; z < *height * *width; z += 64) {
    for (int w = 0; w < 6; w++) {
      multibitwriter(&counter, buffer, &btemp, fp, 0x0001, 1); //Macroblock increment
      multibitwriter(&counter, buffer, &btemp, fp, 0x0001, 1); //Macroblock type
      //multibitwriter(&counter, buffer, &btemp, fp, 0x0001, 1); possible quant value later
      if (w < 4) {
	sout = &Yout[z*4+w*64];
	code = &dc_lum_code;
	size = &dc_lum_size;
      } else if (w == 4) {
        sout = &Cbout[z];
	code = &dc_chroma_code;
	size = &dc_chroma_size;
      } else if (w == 5) {
        sout = &Crout[z];
	code = &dc_chroma_code;
	size = &dc_chroma_size;
      }
      for (int j = 0; j < 8; j++) {
	if ((sout[0] - lastdc[w]) < length[j] && (sout[0] - lastdc[w]) > -length[j]) {
          multibitwriter(&counter, buffer, &btemp, fp, code[j], size[j]);
	  if ((sout[0] - lastdc[w]) > 0)
	    amplitude = (1<<(j-1)) + ((sout[0] - lastdc[w]) - length[j-1]);
	  else
	    amplitude = length[j] + (sout[0] - lastdc[w]) - 1;
          multibitwriter(&counter, buffer, &btemp, fp, amplitude, j);
	  lastdc[w] = sout[0];
	  break;
	}
      }
      for (int i = 1; i < 64; i++) {
	if (sout[zigzag[i]] == 0) {
	  run++;
	  if (i == 63 && run > 0) {
	    multibitwriter(&counter, buffer, &btemp, fp,0x0002, 0x02);
	    run = 0;
	  }
	} else {
	  write_ac(&counter, buffer, &btemp, fp, run, sout[zigzag[i]]);
	  run = 0;
	}
      }
    }
  }
multibitwriter(&counter, buffer, &btemp, fp, 0x0000, counter%8); //byte align at the end of a slice
  /* Flush the bits remaining on the buffer,
   * put an end of file marker and close the file. */
  //finishfilemulti(&counter, buffer, btemp, fp);
