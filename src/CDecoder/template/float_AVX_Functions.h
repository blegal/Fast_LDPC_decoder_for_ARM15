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

#ifndef __AVX_Functions__
#define __AVX_Functions__

#define llr_from_input(v)  ((2.0 * v)/(sigB * sigB))

#include <xmmintrin.h>
#include <tmmintrin.h>
#include <smmintrin.h>
#include <immintrin.h>

#define TYPE_VEC        __m256
#define TYPE_VECi       __m256i // FOR INTEGER OPERATIONS

#define VECT_SET1(p)          (_mm256_set1_ps(p))
#define VECT_SET1i(p)         (_mm256_set1_epi32(p))    // INTEGER OPERATION
#define VECT_LOAD(ptr)      (_mm256_load_ps(ptr))
#define VECT_STORE(ptr,val) (_mm256_store_ps(ptr,val))
#define VECT_ABS(p)         (_mm256_and_ps(p,abs_mask_256))
#define VECT_ADD(a,b)           (_mm256_add_ps(a,b))
#define VECT_SUB(a,b)           (_mm256_sub_ps(a,b))
#define VECT_MUL(a,b)           (_mm256_mul_ps(a,b))
#define VECT_DIV(a,b)           (_mm256_div_ps(a,b))
#define VECT_MAX(a,b)           (_mm256_max_ps(a,b))
#define VECT_MIN(a,b)           (_mm256_min_ps(a,b))
#define VECT_MIN_1(a,min1)      (VECT_MIN(a,min1))
#define VECT_MIN_2(a,min1,min2) (VECT_MIN(min2,VECT_MAX(a, min1)))

#define VECT_ZERO()             (_mm256_setzero_ps()) // INTEGER OPERATION
#define VECT_AND(a,b)           (_mm256_and_ps(a,b))
//#define VECT_XOR(a,b)           (_mm256_xor_ps(a,b))
#define VECT_OR(a,b)            (_mm256_or_ps(a,b))

#define VECT_iAND(a,b)          (_mm256_and_si256(a,b)) // INTEGER OPERATION
#define VECT_iXOR(a,b)          (_mm256_xor_si256(a,b)) // INTEGER OPERATION
#define VECT_iOR(a,b)           (_mm256_or_si256(a,b))  // INTEGER OPERATION

#define CAST_PS_TO_SI(a)          (_mm256_castps_si256(a))
#define CAST_SI_TO_PS(a)          (_mm256_castsi256_ps(a))

#define _mm256_equal_ps(a,b)  _mm256_cmp_ps(a, b, _CMP_EQ_OQ)
#define _mm256_nequal_ps(a,b) _mm256_cmp_ps(a, b, _CMP_NEQ_UQ)



static const TYPE_VEC inv_mask_256 = CAST_SI_TO_PS( VECT_SET1i(0xFFFFFFFF) );
static const TYPE_VEC abs_mask_256 = CAST_SI_TO_PS( VECT_SET1i(0x7FFFFFFF) );
static const TYPE_VEC sig_mask_256 = CAST_SI_TO_PS( VECT_SET1i(0x80000000) );
static const TYPE_VEC mask_0_256   = VECT_SET1( 0.00 );
static const TYPE_VEC mask_0i_256  = CAST_SI_TO_PS( VECT_SET1i(0x00000000) );


static inline TYPE_VEC VECT_SIGN( TYPE_VEC a ){
    TYPE_VECi op1 = CAST_PS_TO_SI(a);            //  Convert it to an integer. There's no instruction here.
    TYPE_VECi op2 = CAST_PS_TO_SI(sig_mask_256); //  Convert it to an integer. There's no instruction here.
    TYPE_VECi op3 = VECT_iAND (op1, op2);        //  Invert all the bits
    TYPE_VECi f   = VECT_iXOR (op3, op2);      //  Invert all the bits
    TYPE_VEC  g   = CAST_SI_TO_PS(f);            //  Convert back. Again, there's no instruction here.
    return g;
}

static inline TYPE_VEC VECT_invSIGN( TYPE_VEC a, TYPE_VEC b ){
    TYPE_VECi c = CAST_PS_TO_SI(b);            //  Convert it to an integer. There's no instruction here.
    TYPE_VECi e = CAST_PS_TO_SI(a);            //  Convert it to an integer. There's no instruction here.
    TYPE_VECi m = CAST_PS_TO_SI(sig_mask_256); //  Convert it to an integer. There's no instruction here.
    TYPE_VECi x = VECT_iXOR(c, m);            //  Invert all the bits
    TYPE_VECi f = VECT_iXOR(x, e);            //  Invert all the bits
    TYPE_VEC  g = CAST_SI_TO_PS(f);            //  Convert back. Again, there's no instruction here.
    return g;
}

static inline TYPE_VEC VECT_CMOV( TYPE_VEC a, TYPE_VEC b, TYPE_VEC v1, TYPE_VEC v2){
    TYPE_VEC m1 = _mm256_equal_ps ( a, b );
    TYPE_VEC m2 = _mm256_nequal_ps( a, b );
    TYPE_VEC m3 = VECT_AND   ( m1, v1 );
    TYPE_VEC m4 = VECT_AND   ( m2, v2 );
    return VECT_OR(m3, m4);
}

static inline TYPE_VEC VECT_CMOV3( TYPE_VEC abs, TYPE_VEC min1, TYPE_VEC min2, TYPE_VEC v1, TYPE_VEC v2, TYPE_VEC v3){
    TYPE_VEC  m1 = _mm256_equal_ps ( abs, min1 );
    TYPE_VEC  m2 = _mm256_equal_ps ( abs, min2 );
    TYPE_VECi ma = VECT_iOR( CAST_PS_TO_SI(m1), CAST_PS_TO_SI(m2) );
    TYPE_VECi m3 = VECT_iXOR( ma, CAST_PS_TO_SI(inv_mask_256));
    TYPE_VEC  m4 = VECT_AND( m1, v1 );
    TYPE_VEC  m5 = VECT_AND( m2, v2 );
    TYPE_VEC  m6 = VECT_AND( CAST_SI_TO_PS(m3), v3 );
    return VECT_OR( VECT_OR(m4, m5), m6);
}

static inline TYPE_VEC VECT_XOR( TYPE_VEC a, TYPE_VEC b ){
    TYPE_VECi c = CAST_PS_TO_SI(a); //  Convert it to an integer. There's no instruction here.
    TYPE_VECi d = CAST_PS_TO_SI(b); //  Convert it to an integer. There's no instruction here.
    TYPE_VECi e = VECT_iXOR(c, d); //  Invert all the bits
    return CAST_SI_TO_PS(e);      //  Convert back. Again, there's no instruction here.
}

static inline TYPE_VEC VECT_iOR( TYPE_VEC a, TYPE_VEC b ){
    TYPE_VECi c = CAST_PS_TO_SI(a); //  Convert it to an integer. There's no instruction here.
    TYPE_VECi d = CAST_PS_TO_SI(b); //  Convert it to an integer. There's no instruction here.
    TYPE_VECi e = VECT_iOR(c, d); //  Invert all the bits
    return CAST_SI_TO_PS(e);      //  Convert back. Again, there's no instruction here.
}

static inline int VECT_XOR_REDUCE(__m256 reg){
    unsigned int *p;
    p = (unsigned int*)&reg;
    return ((p[0] | p[1] | p[2] | p[3] | p[4] | p[5] | p[6] | p[7]) != 0);
}

#endif
