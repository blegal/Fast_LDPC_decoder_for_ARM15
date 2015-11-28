/*
 *  ldcp_decoder.h
 *  ldpc3
 *
 *  Created by legal on 02/04/11.
 *  Copyright 2011 ENSEIRB. All rights reserved.
 *
 */

/*----------------------------------------------------------------------------*/

#ifndef CLASS_CDecoder_NEON8_OMS_
#define CLASS_CDecoder_NEON8_OMS_

#include "../template/CDecoder_fixed_SSE.h"

class CDecoder_OMS_fixed_NEON8 : public CDecoder_OMS_fixed_NEON8{
private:
    int offset;
public:
    CDecoder_OMS_fixed_NEON8();
    void setOffset(int _offset);
    void decode(signed char var_nodes[], signed char Rprime_fix[], int nombre_iterations);

public:
    bool decode_8bits  (signed char var_nodes[], signed char Rprime_fix[], int nombre_iterations);
//    bool decode_generic(signed char var_nodes[], signed char Rprime_fix[], int nombre_iterations);
};

#endif
