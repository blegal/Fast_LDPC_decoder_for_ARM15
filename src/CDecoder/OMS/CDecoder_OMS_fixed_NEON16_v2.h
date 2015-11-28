/*
 *  ldcp_decoder.h
 *  ldpc3
 *
 *  Created by legal on 02/04/11.
 *  Copyright 2011 ENSEIRB. All rights reserved.
 *
 */

/*----------------------------------------------------------------------------*/

#ifndef CLASS_CDecoder_NEON16_OMS_V2_
#define CLASS_CDecoder_NEON16_OMS_V2_

#include "../template/CDecoder_fixed_SSE.h"

class CDecoder_OMS_fixed_NEON16_v2 : public CDecoder_fixed_SSE{
private:
    int8x16_t** p_vn_addr;
    signed char offset;
public:
    CDecoder_OMS_fixed_NEON16_v2();
    ~CDecoder_OMS_fixed_NEON16_v2();
    void setOffset(int _offset);
    void decode(signed char var_nodes[], signed char Rprime_fix[], int nombre_iterations);

public:
    bool decode_8bits  (signed char var_nodes[], signed char Rprime_fix[], int nombre_iterations);
//    bool decode_generic(signed char var_nodes[], signed char Rprime_fix[], int nombre_iterations);
};

//    p_vn_addr = new int8x16_t*[_M];
//    for(int i=0; i<_M; i++){
//       p_vn_addr[i] = &var_nodes[PosNoeudsVariable[i]];
//    }


//int8x16_t** p_indice_nod1   = p_vn_addr;
//int8x16_t** p_indice_nod2   = p_vn_addr;

//TYPE vNoeud   = VECTOR_LOAD( *p_indice_nod1 );
    
#endif
