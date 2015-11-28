/*
 *  ldcp_decoder.h
 *  ldpc3
 *
 *  Created by legal on 02/04/11.
 *  Copyright 2011 ENSEIRB. All rights reserved.
 *
 */

/*----------------------------------------------------------------------------*/

#ifndef __CDecoder_float_x86__
#define __CDecoder_float_x86__

#include "./Constantes/constantes_sse.h"
#include "./float_x86_Functions.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "./CDecoder_float.h"

class CDecoder_float_x86 : public CDecoder_float {
protected:
    float i32_min4;
    float i32_min3;
    float i32_min2;
    float i32_min;

protected:
    float  *var_mesgs;

public:
    CDecoder_float_x86();
    virtual ~CDecoder_float_x86();
    virtual void decode(float var_nodes[], char Rprime_fix[], int nombre_iterations) = 0;

inline void i32b_fix_min_update(double input)
{
    if(input < i32_min){
        i32_min2 = i32_min;
        i32_min  = input;
    }else if(input < i32_min2){
        i32_min2 = input;
        /*i32_min  = i32_min;*/
    }
}    
};

#endif
