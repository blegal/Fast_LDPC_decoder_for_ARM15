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

#include "CDecoder_float_AVX.h"

CDecoder_float_AVX::CDecoder_float_AVX()
{
	var_nodes    = new __m256[NOEUD];
    avxVar_mesgs = new __m256[MESSAGE];
}

CDecoder_float_AVX::~CDecoder_float_AVX()
{
    delete var_nodes;
    delete avxVar_mesgs;
}
