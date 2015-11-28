/*
 *  ldcp_decoder.h
 *  ldpc3
 *
 *  Created by legal on 02/04/11.
 *  Copyright 2011 ENSEIRB. All rights reserved.
 *
 */

/*----------------------------------------------------------------------------*/

#include "./CDecoder_fixed.h"

CDecoder_fixed::CDecoder_fixed()
{

}

CDecoder_fixed::~CDecoder_fixed()
{

}

void CDecoder_fixed::setVarRange(int min, int max)
{
    vSAT_NEG_VAR = min;
    vSAT_POS_VAR = max;
}

void CDecoder_fixed::setMsgRange(int min, int max)
{
    vSAT_NEG_MSG = min;
    vSAT_POS_MSG = max;

}
