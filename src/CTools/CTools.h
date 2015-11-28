#ifndef CLASS_CTools
#define CLASS_CTools

#include <xmmintrin.h>
#include <emmintrin.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern void sse_trans(float   const *inp, float   *out, int nrows, int ncols);
extern void sse_trans(uint8_t const *inp, uint8_t *out, int nrows, int ncols);

extern void sse_trans_float (float *A, float *B, int m);
extern void sse_itrans_float(float *A, float *B, int n);
extern void sse_itrans_and_hard_decision(float *A, float *B, int n);

extern void test_float_transpose();
extern void uchar_transpose_sse(__m128i *src, __m128i *dst, int n);
extern void uchar_itranspose_sse(__m128i *src, __m128i *dst, int n);
extern void test_transpose();

//
// FONCTIONS POUR TRANSPOSER UNE MATRICE TYPE SSE LORSQUE LA TAILLE DE LA
// MATRICE N'EST PAS MODULO 16 !
//
extern void x86_trans_16d(unsigned char *src, unsigned char *dst, int n);
extern void x86_itrans_16d(unsigned char *src, unsigned char *dst, int n);
extern void x86_itrans_and_hard_decision_16d(unsigned char *src, unsigned char *dst, int n);

#endif // CLASS_CTools
