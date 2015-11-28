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

#ifdef __AVX2__
#ifndef __CDecoder_fixed_AVX__
#define __CDecoder_fixed_AVX__

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <xmmintrin.h>
#include <tmmintrin.h>
#include <smmintrin.h>
#include <immintrin.h>

#include "./Constantes/constantes_sse.h"
#include "./CDecoder_fixed.h"

class CDecoder_fixed_AVX : public CDecoder_fixed{
protected:
    __m256i* var_nodes;
    __m256i* var_mesgs;

public:
    CDecoder_fixed_AVX();
    virtual ~CDecoder_fixed_AVX();
    void decode(float var_nodes[], char Rprime_fix[], int nombre_iterations);
};

#endif
#endif
