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

#ifndef __LDPC_float_Decoder_AVX__
#define __LDPC_float_Decoder_AVX__

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <xmmintrin.h>
#include <tmmintrin.h>
#include <smmintrin.h>
#include <immintrin.h>

#include "./Constantes/constantes_sse.h"
#include "./CDecoder_float.h"

class CDecoder_float_AVX : public CDecoder_float{
protected:
    __m256 *var_nodes;
    __m256 *avxVar_mesgs;

public:
    CDecoder_float_AVX();
    virtual ~CDecoder_float_AVX();
};

#endif
