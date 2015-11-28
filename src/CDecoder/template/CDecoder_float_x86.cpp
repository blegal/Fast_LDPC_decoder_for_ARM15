/*
 *  ldcp_decoder.h
 *  ldpc3
 *
 *  Created by legal on 02/04/11.
 *  Copyright 2011 ENSEIRB. All rights reserved.
 *
 */

/*----------------------------------------------------------------------------*/

#include "./CDecoder_float_x86.h"

CDecoder_float_x86::CDecoder_float_x86()
{
    var_mesgs   = new float[MESSAGE];    
}

CDecoder_float_x86::~CDecoder_float_x86()
{
    delete var_mesgs;
}
