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

#include "CDecoder_float_SSE.h"

CDecoder_float_SSE::CDecoder_float_SSE()
{
	var_nodes    = new __m128[NOEUD];
    sseVar_mesgs = new __m128[MESSAGE];
}

CDecoder_float_SSE::~CDecoder_float_SSE()
{
    delete var_nodes;
    delete sseVar_mesgs;
}
