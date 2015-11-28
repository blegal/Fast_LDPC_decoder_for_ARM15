/*
 *  ldcp_decoder.h
 *  ldpc3
 *
 *  Created by legal on 02/04/11.
 *  Copyright 2011 ENSEIRB. All rights reserved.
 *
 */

/*----------------------------------------------------------------------------*/

#include "./CDecoder.h"

CDecoder::CDecoder()
{
    fast_stop = false;
    nb_iters  = 0;
}

CDecoder::~CDecoder()
{
}

void CDecoder::setSigmaChannel(float _SigB)
{
    sigB = _SigB;
}

void CDecoder::setEarlyTerm(bool _early)
{
    fast_stop = _early;
}

void CDecoder::setNumberOfIterations(int _value)
{
    nb_iters = _value;
}
