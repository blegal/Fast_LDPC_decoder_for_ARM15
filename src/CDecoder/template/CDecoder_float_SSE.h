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

#ifndef __LDPC_float_Decoder_SSE__
#define __LDPC_float_Decoder_SSE__

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <xmmintrin.h>

#include "./Constantes/constantes_sse.h"
#include "./CDecoder_float.h"

class CDecoder_float_SSE : public CDecoder_float{
protected:
    __m128 *var_nodes;
    __m128 *sseVar_mesgs;

public:
    CDecoder_float_SSE();
    virtual ~CDecoder_float_SSE();
};

#endif
