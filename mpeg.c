int length[10] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512};


uint16_t dc_lum_code[9] = {0x0004, 0x0000, 0x0001, 0x0005, 0x0006, 0x000e, 0x001e, 0x003e, 0x007e};
uint16_t dc_chroma_code[9] = {0x0000, 0x0001, 0x0002, 0x0006, 0x000e, 0x001e, 0x003e, 0x007e, 0x00fe};

uint8_t dc_lum_size[9] = {3, 2, 2, 3, 3, 4, 5, 6, 7};
uint8_t dc_chroma_size[9] = {2, 2, 2, 3, 4, 5, 6, 7, 8};

uint16_t* code;
uint8_t* size;

  for (int z = 0; z < *height * *width; z += 64) {
    for (int w = 0; w < 6; w++) {
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
  /* Flush the bits remaining on the buffer,
   * put an end of file marker and close the file. */
  //finishfilemulti(&counter, buffer, btemp, fp);
