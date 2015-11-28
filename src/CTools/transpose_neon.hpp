#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <arm_neon.h>

#define trans_TYPE  uint8x16_t

extern void uchar_transpose_neon(trans_TYPE *src, trans_TYPE *dst, int n);

extern void uchar_transpose_neon(unsigned char *src, unsigned char *dst, int n);

extern void uchar_itranspose_neon(trans_TYPE *src, trans_TYPE *dst, int n);

extern void uchar_itranspose_neon(unsigned char *src, unsigned char *dst, int n);

