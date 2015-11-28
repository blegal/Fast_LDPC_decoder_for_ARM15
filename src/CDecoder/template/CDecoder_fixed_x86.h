/*
 *  ldcp_decoder.h
 *  ldpc3
 *
 *  Created by legal on 02/04/11.
 *  Copyright 2011 ENSEIRB. All rights reserved.
 *
 */

/*----------------------------------------------------------------------------*/

#ifndef __CDecoder_x86_
#define __CDecoder_x86_

#include "../../Constantes/constantes_sse.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "./CDecoder_fixed.h"

class CDecoder_fixed_x86 : public CDecoder_fixed {
protected:
    short *var_mesgs;
    short *var_nodes;

public:
    CDecoder_fixed_x86();
    virtual ~CDecoder_fixed_x86();
    virtual void decode(signed char var_nodes[], signed char Rprime_fix[], int nombre_iterations) = 0;
    virtual void decode(float var_nodes[],       signed char Rprime_fix[], int nombre_iterations);
};

#endif
