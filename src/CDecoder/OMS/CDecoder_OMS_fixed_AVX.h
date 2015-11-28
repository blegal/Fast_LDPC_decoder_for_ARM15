/*
 *  ldcp_decoder.h
 *  ldpc3
 *
 *  Created by legal on 02/04/11.
 *  Copyright 2011 ENSEIRB. All rights reserved.
 *
 */

/*----------------------------------------------------------------------------*/

#ifndef CLASS_CDecoder_OMS_AVX_
#define CLASS_CDecoder_OMS_AVX_

#include "../template/CDecoder_fixed_AVX.h"

class CDecoder_OMS_fixed_AVX : public CDecoder_fixed_AVX{
private:
    int offset;
    int nb_exec;
    int nb_saved_iters;
public:
    CDecoder_OMS_fixed_AVX();
    ~CDecoder_OMS_fixed_AVX();
    void setOffset(int _offset);
    void decode(char var_nodes[], char Rprime_fix[], int nombre_iterations);

    bool decode_8bits  (char var_nodes[], char Rprime_fix[], int nombre_iterations);
    bool decode_generic(char var_nodes[], char Rprime_fix[], int nombre_iterations);
};

#endif
