/*
 *  ldcp_decoder.h
 *  ldpc3
 *
 *  Created by legal on 02/04/11.
 *  Copyright 2011 ENSEIRB. All rights reserved.
 *
 */

/*----------------------------------------------------------------------------*/

//#include "../shared/genere_par_java_simu.h"

#ifndef __SSE_Functions__
#define __SSE_Functions__

#define llr_from_input(v)  ((2.0 * v)/(sigB * sigB))

#include <xmmintrin.h>

static const __m128 inv_mask_128 = _mm_castsi128_ps(_mm_set1_epi32(0xFFFFFFFF));
static const __m128 abs_mask_128 = _mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF));
static const __m128 sig_mask_128 = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));
static const __m128 mask_0       = _mm_set1_ps( 0.00 );

#define SSE_4F_LOAD(ptr)      (_mm_load_ps(ptr))
#define SSE_4F_STORE(ptr,val) (_mm_store_ps(ptr,val))
#define SSE_4F_ABS(p)         (_mm_and_ps(p,abs_mask_128))

#define SSE_4F_ADD(a,b)           (_mm_add_ps(a,b))
#define SSE_4F_SUB(a,b)           (_mm_sub_ps(a,b))
#define SSE_4F_MUL(a,b)           (_mm_mul_ps(a,b))
#define SSE_4F_DIV(a,b)           (_mm_div_ps(a,b))
#define SSE_4F_MAX(a,b)           (_mm_max_ps(a,b))
#define SSE_4F_MIN(a,b)           (_mm_min_ps(a,b))
#define SSE_4F_MIN_1(a,min1)      (SSE_4F_MIN(a,min1))
#define SSE_4F_MIN_2(a,min1,min2) (SSE_4F_MIN(min2,SSE_4F_MAX(a, min1)))

#define SSE_4F_AND(a,b)           (_mm_and_ps(a,b))
#define SSE_4F_OR(a,b)           (_mm_or_ps(a,b))

static inline __m128 SSE_4F_SIGN( __m128 a ){
    __m128i op1 = _mm_castps_si128(a);            //  Convert it to an integer. There's no instruction here.
    __m128i op2 = _mm_castps_si128(sig_mask_128); //  Convert it to an integer. There's no instruction here.
    __m128i op3 = _mm_and_si128(op1, op2);        //  Invert all the bits
    __m128i f   = _mm_xor_si128  (op3, op2);      //  Invert all the bits
    __m128  g   = _mm_castsi128_ps(f);            //  Convert back. Again, there's no instruction here.
    return g;
}

static inline __m128 SSE_4F_invSIGN( __m128 a, __m128 b ){
    __m128i c = _mm_castps_si128(b);            //  Convert it to an integer. There's no instruction here.
    __m128i e = _mm_castps_si128(a);            //  Convert it to an integer. There's no instruction here.
    __m128i m = _mm_castps_si128(sig_mask_128); //  Convert it to an integer. There's no instruction here.
    __m128i x = _mm_xor_si128(c, m);            //  Invert all the bits
    __m128i f = _mm_xor_si128(x, e);            //  Invert all the bits
    __m128  g = _mm_castsi128_ps(f);            //  Convert back. Again, there's no instruction here.
    return g;
}

static inline __m128 SSE_4F_CMOV( __m128 a, __m128 b, __m128 v1, __m128 v2){
    __m128 m1 = _mm_cmpeq_ps ( a, b );
    __m128 m2 = _mm_cmpneq_ps( a, b );
    __m128 m3 = _mm_and_ps   ( m1, v1 );
    __m128 m4 = _mm_and_ps   ( m2, v2 );
    return _mm_or_ps(m3, m4);
}

static inline __m128 SSE_4F_CMOV3( __m128 abs, __m128 min1, __m128 min2, __m128 v1, __m128 v2, __m128 v3){
    __m128  m1 = _mm_cmpeq_ps ( abs, min1 );
    __m128  m2 = _mm_cmpeq_ps ( abs, min2 );
    __m128i ma = _mm_or_si128 ( _mm_castps_si128(m1), _mm_castps_si128(m2) );
    __m128i m3 = _mm_xor_si128( ma, _mm_castps_si128(inv_mask_128));
    __m128  m4 = _mm_and_ps   ( m1, v1 );
    __m128  m5 = _mm_and_ps   ( m2, v2 );
    __m128  m6 = _mm_and_ps   ( _mm_castsi128_ps(m3), v3 );
    return _mm_or_ps( _mm_or_ps(m4, m5), m6);
}

static inline __m128 SSE_4F_XOR( __m128 a, __m128 b ){
    __m128i c = _mm_castps_si128(a); //  Convert it to an integer. There's no instruction here.
    __m128i d = _mm_castps_si128(b); //  Convert it to an integer. There's no instruction here.
    __m128i e = _mm_xor_si128(c, d); //  Invert all the bits
    return _mm_castsi128_ps(e);      //  Convert back. Again, there's no instruction here.
}

static inline int SSE_4F_XOR_REDUCE(__m128 reg){
    unsigned int *p;
    p = (unsigned int*)&reg;
    return ((p[0] | p[1] | p[2] | p[3]) != 0);
}

#if defined(__ICC) || defined(__INTEL_COMPILER)
#else
static inline __m128 _mm_atanh_ps (__m128 y){
    __m128 r;
    float *a = (float *)&y;
    float *b = (float *)&r;
    b[0] = atanhf(a[0]);
    b[1] = atanhf(a[1]);
    b[2] = atanhf(a[2]);
    b[3] = atanhf(a[3]);
    return r;
}
#endif

#if defined(__ICC) || defined(__INTEL_COMPILER)
#else
static inline __m128 _mm_tanh_ps (__m128 y){
    __m128 r;
    float *a = (float *)&y;
    float *b = (float *)&r;
    b[0] = tanhf(a[0]);
    b[1] = tanhf(a[1]);
    b[2] = tanhf(a[2]);
    b[3] = tanhf(a[3]);
    return r;
}
#endif

static inline double i_bp(float sum){
    float v = logf( (expf(sum) + 1.0) / (expf(sum) - 1.0) );
    return v;
}

static inline __m128 _mm_bp_ps (__m128 y){
    __m128 r;
    float *a = (float *)&y;
    float *b = (float *)&r;
    b[0] = i_bp(a[0]);
    b[1] = i_bp(a[1]);
    b[2] = i_bp(a[2]);
    b[3] = i_bp(a[3]);
    return r;
}

static inline double i_bp(float _a, float _b){
    return i_bp( i_bp(_a) + i_bp(_b) );
}

static inline __m128 i_bp(__m128 _a, __m128 _b){
    return _mm_bp_ps( SSE_4F_ADD(_mm_bp_ps(_a), _mm_bp_ps(_b)) );
}

#endif
