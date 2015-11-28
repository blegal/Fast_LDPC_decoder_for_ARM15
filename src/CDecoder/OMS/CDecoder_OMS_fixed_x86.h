/*
 *  ldcp_decoder.h
 *  ldpc3
 *
 *  Created by legal on 02/04/11.
 *  Copyright 2011 ENSEIRB. All rights reserved.
 *
 */

/*----------------------------------------------------------------------------*/

#ifndef _CLASS_CDecoder_x86_OMS_
#define _CLASS_CDecoder_x86_OMS_

#include "../template/CDecoder_fixed_x86.h"


class CDecoder_OMS_fixed_x86 : public CDecoder_fixed_x86{
private:
    int _i32_min;
    int _i32_min2;
    int offset;
public:
    CDecoder_OMS_fixed_x86();
    ~CDecoder_OMS_fixed_x86();
    void setOffset(int _offset);
    void decode(signed char var_nodes[], signed char Rprime_fix[], int nombre_iterations);
};

#endif
