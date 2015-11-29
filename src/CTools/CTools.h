/**
  Copyright (c) 2012-2015 "Bordeaux INP, Bertrand LE GAL"
  [bertrand.legal@ims-bordeaux.fr     ]
  [http://legal.vvv.enseirb-matmeca.fr]

  This file is part of Fast_LDPC_C_decoder_for_ARM15.

  Fast_LDPC_C_decoder_for_ARM15 is free software: you can redistribute it and/or modify

  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
