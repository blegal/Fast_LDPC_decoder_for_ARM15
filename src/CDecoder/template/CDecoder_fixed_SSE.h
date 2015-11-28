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

#ifndef __CDecoder_fixed_SSE__
#define __CDecoder_fixed_SSE__

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <arm_neon.h>

#include "./Constantes/constantes_sse.h"
#include "./CDecoder_fixed.h"

class CDecoder_fixed_SSE : public CDecoder_fixed{
protected:
    int8x16_t* var_nodes;
    int8x16_t* var_mesgs;

public:
    CDecoder_fixed_SSE();
    virtual ~CDecoder_fixed_SSE();
    void decode(float var_nodes[], signed char Rprime_fix[], int nombre_iterations);
};

#endif
