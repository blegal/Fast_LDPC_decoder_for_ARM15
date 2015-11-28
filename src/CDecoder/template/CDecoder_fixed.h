/*
 *  ldcp_decoder.h
 *  ldpc3
 *
 *  Created by legal on 02/04/11.
 *  Copyright 2011 ENSEIRB. All rights reserved.
 *
 */

/*----------------------------------------------------------------------------*/

#ifndef __CDecoder_fixed__
#define __CDecoder_fixed__

#include "./CDecoder.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

class CDecoder_fixed : public CDecoder{
protected:
    int vSAT_NEG_MSG;
    int vSAT_POS_MSG;
    int vSAT_NEG_VAR;
    int vSAT_POS_VAR;

public:
    CDecoder_fixed();
    virtual ~CDecoder_fixed();
    virtual void setVarRange(int min, int max);
    virtual void setMsgRange(int min, int max);
    virtual void decode(signed char  var_nodes[], signed char Rprime_fix[], int nombre_iterations) = 0;
    virtual void decode(float        var_nodes[], signed char Rprime_fix[], int nombre_iterations) = 0;
};

#endif
