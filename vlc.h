/* vlc.h, variable length code tables (used by routines in putvlc.c)        */

/* Copyright (C) 1994, MPEG Software Simulation Group. All Rights Reserved. */

/*
 * Disclaimer of Warranty
 *
 * These software programs are available to the user without any license fee or
 * royalty on an "as is" basis.  The MPEG Software Simulation Group disclaims
 * any and all warranties, whether express, implied, or statuary, including any
 * implied warranties or merchantability or of fitness for a particular
 * purpose.  In no event shall the copyright-holder be liable for any
 * incidental, punitive, or consequential damages of any kind whatsoever
 * arising from the use of these programs.
 *
 * This disclaimer of warranty extends to the user of these programs and user's
 * customers, employees, agents, transferees, successors, and assigns.
 *
 * The MPEG Software Simulation Group does not represent or warrant that the
 * programs furnished hereunder are free of infringement of any third-party
 * patents.
 *
 * Commercial implementations of MPEG-1 and MPEG-2 video, including shareware,
 * are subject to royalty fees to patent holders.  Many of these patents are
 * general enough such that they are unavoidable regardless of implementation
 * design.
 *
 */

/* type definitions for variable length code table entries */

/*typedef struct
{
  unsigned char code;
  char len;
} VLCtable;*/

/* for codes longer than 8 bits (excluding leading zeroes) */
typedef struct
{
  uint16_t code; /* right justified */
  uint8_t len;
} VLCtable;


/* data from ISO/IEC 13818-2 DIS, Annex B, variable length code tables */

/* Table B-1, variable length codes for macroblock_address_increment
 *
 * indexed by [macroblock_address_increment-1]
 * 'macroblock_escape' is treated elsewhere
 */

static VLCtable addrinctab[33]=
{
  {0x01,1},  {0x03,3},  {0x02,3},  {0x03,4},
  {0x02,4},  {0x03,5},  {0x02,5},  {0x07,7},
  {0x06,7},  {0x0b,8},  {0x0a,8},  {0x09,8},
  {0x08,8},  {0x07,8},  {0x06,8},  {0x17,10},
  {0x16,10}, {0x15,10}, {0x14,10}, {0x13,10},
  {0x12,10}, {0x23,11}, {0x22,11}, {0x21,11},
  {0x20,11}, {0x1f,11}, {0x1e,11}, {0x1d,11},
  {0x1c,11}, {0x1b,11}, {0x1a,11}, {0x19,11},
  {0x18,11}
};


/* Table B-2, B-3, B-4 variable length codes for macroblock_type
 *
 * indexed by [macroblock_type]
 */

static VLCtable mbtypetab[3][32]=
{
 /* I */
 {
  {0,0}, {1,1}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0},
  {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0},
  {0,0}, {1,2}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0},
  {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}
 },
 /* P */
 {
  {0,0}, {3,5}, {1,2}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0},
  {1,3}, {0,0}, {1,1}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0},
  {0,0}, {1,6}, {1,5}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0},
  {0,0}, {0,0}, {2,5}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}
 },
 /* B */
 {
  {0,0}, {3,5}, {0,0}, {0,0}, {2,3}, {0,0}, {3,3}, {0,0},
  {2,4}, {0,0}, {3,4}, {0,0}, {2,2}, {0,0}, {3,2}, {0,0},
  {0,0}, {1,6}, {0,0}, {0,0}, {0,0}, {0,0}, {2,6}, {0,0},
  {0,0}, {0,0}, {3,6}, {0,0}, {0,0}, {0,0}, {2,5}, {0,0}
 }
};


/* Table B-5 ... B-8 variable length codes for macroblock_type in
 *  scalable sequences
 *
 * not implemented
 */

/* Table B-9, variable length codes for coded_block_pattern
 *
 * indexed by [coded_block_pattern]
 */

static VLCtable cbptable[64]=
{
  {0x01,9}, {0x0b,5}, {0x09,5}, {0x0d,6}, 
  {0x0d,4}, {0x17,7}, {0x13,7}, {0x1f,8}, 
  {0x0c,4}, {0x16,7}, {0x12,7}, {0x1e,8}, 
  {0x13,5}, {0x1b,8}, {0x17,8}, {0x13,8}, 
  {0x0b,4}, {0x15,7}, {0x11,7}, {0x1d,8}, 
  {0x11,5}, {0x19,8}, {0x15,8}, {0x11,8}, 
  {0x0f,6}, {0x0f,8}, {0x0d,8}, {0x03,9}, 
  {0x0f,5}, {0x0b,8}, {0x07,8}, {0x07,9}, 
  {0x0a,4}, {0x14,7}, {0x10,7}, {0x1c,8}, 
  {0x0e,6}, {0x0e,8}, {0x0c,8}, {0x02,9}, 
  {0x10,5}, {0x18,8}, {0x14,8}, {0x10,8}, 
  {0x0e,5}, {0x0a,8}, {0x06,8}, {0x06,9}, 
  {0x12,5}, {0x1a,8}, {0x16,8}, {0x12,8}, 
  {0x0d,5}, {0x09,8}, {0x05,8}, {0x05,9}, 
  {0x0c,5}, {0x08,8}, {0x04,8}, {0x04,9},
  {0x07,3}, {0x0a,5}, {0x08,5}, {0x0c,6}
};


/* Table B-10, variable length codes for motion_code
 *
 * indexed by [abs(motion_code)]
 * sign of motion_code is treated elsewhere
 */

static VLCtable motionvectab[17]=
{
  {0x01,1},  {0x01,2},  {0x01,3},  {0x01,4},
  {0x03,6},  {0x05,7},  {0x04,7},  {0x03,7},
  {0x0b,9},  {0x0a,9},  {0x09,9},  {0x11,10},
  {0x10,10}, {0x0f,10}, {0x0e,10}, {0x0d,10},
  {0x0c,10}
};


/* Table B-11, variable length codes for dmvector
 *
 * treated elsewhere
 */

/* Table B-12, variable length codes for dct_dc_size_luminance
 *
 * indexed by [dct_dc_size_luminance]
 */

static sVLCtable DClumtab[12]=
{
  {0x0004,3}, {0x0000,2}, {0x0001,2}, {0x0005,3}, {0x0006,3}, {0x000e,4},
  {0x001e,5}, {0x003e,6}, {0x007e,7}, {0x00fe,8}, {0x01fe,9}, {0x01ff,9}
};


/* Table B-13, variable length codes for dct_dc_size_chrominance
 *
 * indexed by [dct_dc_size_chrominance]
 */

static sVLCtable DCchromtab[12]=
{
  {0x0000,2}, {0x0001,2}, {0x0002,2}, {0x0006,3}, {0x000e,4}, {0x001e,5},
  {0x003e,6}, {0x007e,7}, {0x00fe,8}, {0x01fe,9}, {0x03fe,10},{0x03ff,10}
};


/* Table B-14, DCT coefficients table zero
 *
 * indexed by [run][level-1]
 * split into two tables (dct_code_tab1, dct_code_tab2) to reduce size
 * 'first DCT coefficient' condition and 'End of Block' are treated elsewhere
 * codes do not include s (sign bit)
 */

static VLCtable dct_code_tab1[2][40]=
{
 /* run = 0, level = 1...40 */
 {
  {0x0003, 2}, {0x0004, 4}, {0x0005, 5}, {0x0006, 7},
  {0x0026, 8}, {0x0021, 8}, {0x000a,10}, {0x001d,12},
  {0x0018,12}, {0x0013,12}, {0x0010,12}, {0x001a,13},
  {0x0019,13}, {0x0018,13}, {0x0017,13}, {0x001f,14},
  {0x001e,14}, {0x001d,14}, {0x001c,14}, {0x001b,14},
  {0x001a,14}, {0x0019,14}, {0x0018,14}, {0x0017,14},
  {0x0016,14}, {0x0015,14}, {0x0014,14}, {0x0013,14},
  {0x0012,14}, {0x0011,14}, {0x0010,14}, {0x0018,15},
  {0x0017,15}, {0x0016,15}, {0x0015,15}, {0x0014,15},
  {0x0013,15}, {0x0012,15}, {0x0011,15}, {0x0010,15}
 },
 /* run = 1, level = 1...18 */
 {
  {0x0003, 3}, {0x0006, 6}, {0x0025, 8}, {0x000c,10},
  {0x001b,12}, {0x0016,13}, {0x0015,13}, {0x001f,15},
  {0x001e,15}, {0x001d,15}, {0x001c,15}, {0x001b,15},
  {0x001a,15}, {0x0019,15}, {0x0013,16}, {0x0012,16},
  {0x0011,16}, {0x0010,16}, {0x0000, 0}, {0x0000, 0},
  {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0},
  {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0},
  {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0},
  {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0},
  {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}
 }
};

static VLCtable dct_code_tab2[30][5]=
{
  /* run = 2...31, level = 1...5 */
  {{0x0005, 4}, {0x0004, 7}, {0x000b,10}, {0x0014,12}, {0x0014,13}},
  {{0x0007, 5}, {0x0024, 8}, {0x001c,12}, {0x0013,13}, {0x0000, 0}},
  {{0x0006, 5}, {0x000f,10}, {0x0012,12}, {0x0000, 0}, {0x0000, 0}},
  {{0x0007, 6}, {0x0009,10}, {0x0012,13}, {0x0000, 0}, {0x0000, 0}},
  {{0x0005, 6}, {0x001e,12}, {0x0014,16}, {0x0000, 0}, {0x0000, 0}},
  {{0x0004, 6}, {0x0015,12}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x0007, 7}, {0x0011,12}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x0005, 7}, {0x0011,13}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x0027, 8}, {0x0010,13}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x0023, 8}, {0x001a,16}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x0022, 8}, {0x0019,16}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x0020, 8}, {0x0018,16}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x000e,10}, {0x0017,16}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x000d,10}, {0x0016,16}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x0008,10}, {0x0015,16}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x001f,12}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x001a,12}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x0019,12}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x0017,12}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x0016,12}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x001f,13}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x001e,13}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x001d,13}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x001c,13}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x001b,13}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x001f,16}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x001e,16}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x001d,16}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x001c,16}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x001b,16}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}}
};


/* Table B-15, DCT coefficients table one
 *
 * indexed by [run][level-1]
 * split into two tables (dct_code_tab1a, dct_code_tab2a) to reduce size
 * 'End of Block' is treated elsewhere
 * codes do not include s (sign bit)
 */

static VLCtable dct_code_tab1a[2][40]=
{
 /* run = 0, level = 1...40 */
 {
  {0x0002, 2}, {0x0006, 3}, {0x0007, 4}, {0x001c, 5},
  {0x001d, 5}, {0x0005, 6}, {0x0004, 6}, {0x007b, 7},
  {0x007c, 7}, {0x0023, 8}, {0x0022, 8}, {0x00fa, 8},
  {0x00fb, 8}, {0x00fe, 8}, {0x00ff, 8}, {0x001f,14},
  {0x001e,14}, {0x001d,14}, {0x001c,14}, {0x001b,14},
  {0x001a,14}, {0x0019,14}, {0x0018,14}, {0x0017,14},
  {0x0016,14}, {0x0015,14}, {0x0014,14}, {0x0013,14},
  {0x0012,14}, {0x0011,14}, {0x0010,14}, {0x0018,15},
  {0x0017,15}, {0x0016,15}, {0x0015,15}, {0x0014,15},
  {0x0013,15}, {0x0012,15}, {0x0011,15}, {0x0010,15}
 },
 /* run = 1, level = 1...18 */
 {
  {0x0002, 3}, {0x0006, 5}, {0x0079, 7}, {0x0027, 8},
  {0x0020, 8}, {0x0016,13}, {0x0015,13}, {0x001f,15},
  {0x001e,15}, {0x001d,15}, {0x001c,15}, {0x001b,15},
  {0x001a,15}, {0x0019,15}, {0x0013,16}, {0x0012,16},
  {0x0011,16}, {0x0010,16}, {0x0000, 0}, {0x0000, 0},
  {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0},
  {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0},
  {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0},
  {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0},
  {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}
 }
};

static VLCtable dct_code_tab2a[30][5]=
{
  /* run = 2...31, level = 1...5 */
  {{0x0005, 5}, {0x0007, 7}, {0x00fc, 8}, {0x000c,10}, {0x0014,13}},
  {{0x0007, 5}, {0x0026, 8}, {0x001c,12}, {0x0013,13}, {0x0000, 0}},
  {{0x0006, 6}, {0x00fd, 8}, {0x0012,12}, {0x0000, 0}, {0x0000, 0}},
  {{0x0007, 6}, {0x0004, 9}, {0x0012,13}, {0x0000, 0}, {0x0000, 0}},
  {{0x0006, 7}, {0x001e,12}, {0x0014,16}, {0x0000, 0}, {0x0000, 0}},
  {{0x0004, 7}, {0x0015,12}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x0005, 7}, {0x0011,12}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x0078, 7}, {0x0011,13}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x007a, 7}, {0x0010,13}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x0021, 8}, {0x001a,16}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x0025, 8}, {0x0019,16}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x0024, 8}, {0x0018,16}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x0005, 9}, {0x0017,16}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x0007, 9}, {0x0016,16}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x000d,10}, {0x0015,16}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x001f,12}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x001a,12}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x0019,12}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x0017,12}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x0016,12}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x001f,13}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x001e,13}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x001d,13}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x001c,13}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x001b,13}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x001f,16}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x001e,16}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x001d,16}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x001c,16}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}},
  {{0x001b,16}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}, {0x0000, 0}}
};

void multibitwriter(int* counter, uint8_t* buffer,  uint8_t* temp, FILE* fp, uint16_t input, uint8_t size) {
  if (size < (*counter%8 == 0 ? 8 : *counter%8)) {
    *temp = *temp + (input << ((*counter%8 == 0 ? 8 : *counter%8) - size));
    *counter = *counter - size;
    return;
  } else if (size == (*counter%8 == 0 ? 8 : *counter%8)) {
    *temp = *temp + (input << ((*counter%8 == 0 ? 8 : *counter%8) - size));
    buffer[(buffersize+bufferoverflow) - ((*counter%8 == 0) ? *counter/8 : *counter/8 + 1)] = *temp;
    *counter = *counter - size;
    *temp = 0;
  } else {
    *temp += (input >> (size - (*counter%8 == 0 ? 8 : *counter%8)));
    buffer[(buffersize+bufferoverflow) - ((*counter%8 == 0) ? *counter/8 : *counter/8 + 1)] = *temp;
    input = input & ~(0xffff << (size - (*counter%8 == 0 ? 8 : *counter%8)));
    size = size - (*counter%8 == 0 ? 8 : *counter%8);
    *counter = *counter - (*counter%8 == 0 ? 8 : *counter%8);
    if (size < 8) {
      *temp = input << (8 - size);
      *counter -= size;
    } else if (size == 8) {
      *temp = input << (8 - size);
      buffer[(buffersize+bufferoverflow) - ((*counter%8 == 0) ? *counter/8 : *counter/8 + 1)] = *temp;
      *counter -= size;
      *temp = 0;
    } else {
      *temp = input >> (size - 8);
      buffer[(buffersize+bufferoverflow) - ((*counter%8 == 0) ? *counter/8 : *counter/8 + 1)] = *temp;
      input = input & ~(0xffff << (size - 8));
      size -= 8;
      *temp = input << (8 - size);
      buffer[((buffersize+bufferoverflow) + 1) - ((*counter%8 == 0) ? *counter/8 : *counter/8 + 1)] = *temp;
      *counter = *counter - 8 - size;
    }
  }
      
  if ((*counter) <= (bufferoverflow*8)) {
    fwrite(buffer, sizeof(uint8_t), buffersize, fp);
    for (int i = 0; i < bufferoverflow; i++)
      buffer[i] = buffer[i+buffersize];
    for (int i = bufferoverflow; i < (buffersize+bufferoverflow); i++)
      buffer[i] = 0;
    *counter += buffersize*8;
  }
}


void write_ac(int* counter, uint8_t* buffer, uint8_t* temp, FILE* fp, int run, int level) {
  int abs_level = (level >= 0) ? level : -level;
  if (run < 2) {
    if (dct_code_tab1[run][abs_level-1].size != 0) {
      multibitwriter(counter, buffer, temp, fp, dct_code_tab1[run][abs_level-1].code, dct_code_tab1[run][abs_level-1].size);
      multibitwriter(counter, buffer, temp, fp, (level >= 0) ? 0x0000 : 0x0001, 1);
    } else {
      multibitwriter(counter, buffer, temp, fp, 0x0001, 6); 
      multibitwriter(counter, buffer, temp, fp, run, 6); 
      if (level > 127)
	multibitwriter(counter, buffer, temp, fp, 0x0000, 8); 
      if (level < -127)
	multibitwriter(counter, buffer, temp, fp, 0x00ff, 8); 
      multibitwriter(counter, buffer, temp, fp, level & 0x00ff, 8); 
    }
  } else if (run < 32) {
    if (dct_code_tab2[run-2][abs_level-1].code != 0) {
      multibitwriter(counter, buffer, temp, fp, dct_code_tab2[run-2][abs_level-1].code, dct_code_tab1[run-2][abs_level-1].size);
      multibitwriter(counter, buffer, temp, fp, (level >= 0) ? 0x0000 : 0x0001, 1);
    } else {
      multibitwriter(counter, buffer, temp, fp, 0x0001, 6); 
      multibitwriter(counter, buffer, temp, fp, run, 6); 
      if (level > 127)
	multibitwriter(counter, buffer, temp, fp, 0x0000, 8); 
      if (level < -127)
	multibitwriter(counter, buffer, temp, fp, 0x00ff, 8); 
      multibitwriter(counter, buffer, temp, fp, level & 0x00ff, 8); 
    }
  } else {
    multibitwriter(counter, buffer, temp, fp, 0x0001, 6); 
    multibitwriter(counter, buffer, temp, fp, run, 6); 
    if (level > 127)
      multibitwriter(counter, buffer, temp, fp, 0x0000, 8); 
    if (level < -127)
      multibitwriter(counter, buffer, temp, fp, 0x00ff, 8); 
    multibitwriter(counter, buffer, temp, fp, level & 0x00ff, 8); 
  }
}
