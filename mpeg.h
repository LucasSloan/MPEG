sequence_header(){
  sequence_header_code(32);
  horizontal_size(12);
  vertical_size(12);
  pel_aspect_ratio(4);
  picture_rate(4);
  bit_rate(18);
  marker_bit(1);
  vbv_buffer_size(10);
  constrained_parameters_flag(1);
  load_intra_quantizer_matix(1);
  if (load_intra_quantizer_matix)
    intra_quantizer_matrix[0..63];
  load_non_intra_quantizer_matix(1);
  if (load_non_intra_quantizer_matix)
    non_intra_quantizer_matrix[0..63];
  next_start_code();
  if (nextbits(32)==extension_start_code) {
    extension_start_code(32);
    while (nextbits(24) != 0x000001) {
      user_data(8);
    }
    next_start_code();
  }
}

group_of_pictures(){
  group_start_code(32);
  time_code(25);
  closed_gop(1);
  broken_link(1);
  next_start_code();
  if (nextbits(32) == extension_start_code){
    extension_start_code(32);
    while (nextbits(24) != 0x000001){
      group_extension_data(8);
    }
    next_start_code();
  }
  do {
    picture();
  } while (nextbits(32) == picture_start_code);
}

picture(){
  picture_start_code(32);
  temporal_reference(10);
  picture_coding_type(3);
  vbv_delay(16);
  if ((picture_coding_type == 2)||(picture_coding_type == 3)){
    full_pel_forward_vector(1);
    forward_f_code(3);
  }
  if (picture_coding_type == 3){
    full_pel_backward_vector(1);
    backward_f_code(3);
  }
  while (nextbits(1) == '1'){
    extra_bit_picture(1);
    extra_information_picture(8);
  }
  extra_bit_picture(1);
  next_start_code();
  if (nextbits(32) == extension_start_code){
    extension_start_code(32);
    while (nextbits(24) != 0x000001){
      picture_extension_data(8);
    }
    next_start_code();
  }
  if (nextbits(32) == user_data_start_code){
    user_data_start_code(32);
    while (nextbits(24) != 0x000001){
      user_data(8);
    }
    next_start_code();
  }
  do {
    slice();
  } while (nextbits(32) == slice_start_code);
}

slice(){
  slice_start_code(32);
  quantizer_scale(5);
  while (nextbits(1) == '1') {
    extra_bit_slice(1);
    extra_information_slice(8);
  }
  extra_bit_slice(1);
  do {
    macroblock();
  } while (nextbits(23) != 0);
  next_start_code();
}

macroblock(){
  while (nextbits(11) == '00000001111')
    macroblock_stuffing(11);
  while (nextbits(11) == '00000001000')
    macroblock_escape(11);
  macroblock_address_increment(1-11);
  macroblock_type(1-6);
  if (macroblock_quant)
    quantizer_scale(5);
  if (macroblock_motion_forward){
    motion_horizontal_forward_code(1-11);
    if ((forward_f != 1)&&(motion_horizontal_forward_code != 0))
      motion_horizontal_forward_r(1-6);
    motion_vertical_forward_code(1-11);
    if ((forward_f != 1)&&(motion_vertical_forward_code != 0))
      motion_vertical_forward_r(1-6);
  }
  if (macroblock_motion_backward){
    motion_horizontal_backward_code(1-11);
    if ((backward_f != 1)&&(motion_horizontal_backward_code != 0))
      motion_horiziontal_backward_r(1-6);
    motion_vertical_backward_code(1-11);
    if ((backward_f != 1) && (motion_vertical_backward_r(1-6)))
	motion_vertical_backward_r(1-6);
  }
  if (macro_block_pattern)
    coded_block_pattern(3-9);
  for (i=0; i<6; i++)
    block(i);
  if (picture_coding_type == 4)
    end_of_macroblock(1);
}

block(i){
  if (pattern_code[i]){
    if (macroblock_intra){
      if (i<4){
	dct_dc_size_luminance(2-7);
	if (dc_size_luminance != 0)
	  dct_dc_differential(1-8);
      }
      else{
	dct_dc_size_chrominance(2-7);
	if (dc_size_chrominance != 0)
	  dct_dc_differential(1-8);
      }
    }
    else{
      dct_coeff_first(2-28);
    }
    if (picture_coding_type != 4){
      while (nextbits(2) != '10')
	dct_coeff_next(3-28);
      end_of_block(2);
    }
  }
}

uint8_t default_intra_matrix[64] = {
  8, 16, 19, 22, 26, 27, 29, 34,
  16, 16, 22, 24, 27, 29, 34, 37,
  19, 22, 26, 27, 29, 34, 34, 38,
  22, 22, 26, 27, 29, 34, 37, 40,
  22, 26, 27, 29, 32, 35, 40, 48,
  26, 27, 29, 32, 35, 40, 48, 58,
  26, 27, 29, 34, 38, 46, 56, 69,
  27, 29, 35, 38, 46, 56, 69, 83 };

uint8_t  block_sizes[32][41];

uint32_t block_codes[32][41];

block_sizes[0][1] = 3;

block_sizes[0][2] = 5;
3 = 6;
