/*
 *  ldcp_decoder.h
 *  ldpc3
 *
 *  Created by legal on 02/04/11.
 *  Copyright 2011 ENSEIRB. All rights reserved.
 *
 */

/*----------------------------------------------------------------------------*/
/*
    - 10 mars 2014 : bugfix de la saturation dans le message entrant (VN => CN)
 */

#include "CDecoder_OMS_fixed_NEON16_v2.h"
#include "../../CTools/transpose_neon.hpp"

#define  TYPE int8x16_t
#define uTYPE uint8x16_t

#define  _PREFETCH_

#define VECTOR_LOAD(ptr)            vld1q_s8((int8_t*)ptr) //((ptr)[0])
#define VECTOR_STORE(ptr,v)         vst1q_s8((int8_t*)ptr,v)//((ptr)[0])=v 

#define VECTOR_ADD(a,b)             (vqaddq_s8(a,b)) //
#define VECTOR_SUB(a,b)             (vqsubq_s8(a,b)) //
#define VECTOR_ABS(a)               (vabsq_s8(a))    //
#define VECTOR_NEG(a)               (vnegq_s8(a))    //

//#define VECTOR_NEG(a)               (vqnegq_s8 (a,8))

#define VECTOR_MAX(a,b)             (vmaxq_s8(a,b))  //
#define VECTOR_MIN_1(a,b)           (vminq_s8(a,b))  //
#define VECTOR_XOR(a,b)             (veorq_s8(a,b))  //
#define VECTOR_OR(a,b)              (vorrq_s8(a,b))   //
#define VECTOR_AND(a,b)             (vandq_s8(a,b))  //
#define VECTOR_SET1u(a)             (vdupq_n_u8(a))  //
#define VECTOR_SET1i(a)             (vdupq_n_s8(a))  //
#define VECTOR_SHR_BY_1(a,b)        (vshrq_n_s8(a,b))
#define VECTOR_SHL_BY_1(a,b)        (vshlq_n_s8(a,b))
#define VECTOR_SHR_BY_8(a)          (vshrq_n_s8(a,8))

#define VECTOR_CMP_EQ(a,b)          (vceqq_s8(a,b))
#define VECTOR_CMP_LTEQ(a,b)        (vcleq_s8(a,b))
#define VECTOR_CMP_GE(a,b)          (vcgeq_s8(a,b))

#define VECTOR_NOT(a)               (vmvnq_s8(a))
#define VECTOR_MIN(a,b)             (VECTOR_MIN_1(a,b))

#define VECTOR_ADD(a,b)             (vqaddq_s8(a,b))
#define VECTOR_SBU(a,b)             (vqsubq_u8(a,b))
#define VECTOR_SUB(a,b)             (vqsubq_s8(a,b))
#define VECTOR_ANDNOT(a,b)          (VECTOR_AND(a,VECTOR_NOT(b))) // OUPS
//#define VECTOR_ANDNOT(a,b)          (vbicq_u8(a,b)) // OUPS

#define VECTOR_SIGN(a,b)            (VECTOR_XOR(a,b))    // OUPS
#define VECTOR_EQUAL(a,b)           (VECTOR_CMP_EQ(a,b))
#define VECTOR_ZERO                 (VECTOR_SET1u(0))
#define VECTOR_SET1(a)              (VECTOR_SET1i(a))

//
// INFERIEUR A ZERO DONNE 0
//
inline TYPE VECTOR_GET_SIGN_BIT( TYPE a ){
    return VECTOR_CMP_GE(a, VECTOR_SET1i(0x00));
}

//
// ON INVERSE LE SIGNE DE LA DONNEE EN FONCTION DU SIGNE
//
inline TYPE VECTOR_invSIGN2( TYPE a, TYPE z){
    TYPE g = VECTOR_AND   ( a,             z );
    TYPE h = VECTOR_ANDNOT( VECTOR_NEG(a), z );
    return VECTOR_OR      ( g, h );
}

#define VECTOR_MIN_2(val,old_min1,min2) \
    (VECTOR_MIN(min2,VECTOR_MAX(val,old_min1)))

#define VECTOR_VAR_SATURATE(a) \
    (VECTOR_MAX(VECTOR_MIN(a, max_var), min_var))

#define VECTOR_SATURATE(a, max, min) \
    (VECTOR_MAX(VECTOR_MIN(a, max), min))


inline TYPE VECTOR_CMOV( TYPE a, TYPE b, TYPE c, TYPE d){
    TYPE z = VECTOR_EQUAL  ( a, b );
    TYPE g = VECTOR_AND    ( c, z );
    TYPE h = VECTOR_ANDNOT ( d, z );
    return VECTOR_OR       ( g, h );
}

#define VECTOR_SUB_AND_SATURATE_VAR_8bits(a,b,min) \
    (VECTOR_MAX(VECTOR_SUB(a,b), min)) // ON DOIT CONSERVER LA SATURATION MIN A CAUSE DE -128

#define VECTOR_ADD_AND_SATURATE_VAR_8bits(a,b,min) \
    (VECTOR_MAX(VECTOR_ADD(a,b), min)) // ON DOIT CONSERVER LA SATURATION MIN A CAUSE DE -128

#define VECTOR_SUB_AND_SATURATE_VAR(a,b,max,min) \
    (VECTOR_SATURATE(VECTOR_SUB(a,b),max,min))

#define VECTOR_ADD_AND_SATURATE_VAR(a,b,max,min) \
    (VECTOR_SATURATE(VECTOR_ADD(a,b),max,min))


CDecoder_OMS_fixed_NEON16_v2::CDecoder_OMS_fixed_NEON16_v2()
{
    p_vn_addr = new int8x16_t*[_M];
    for(int i=0; i<_M; i++){
       p_vn_addr[i] = &var_nodes[PosNoeudsVariable[i]];
    }
}

CDecoder_OMS_fixed_NEON16_v2::~CDecoder_OMS_fixed_NEON16_v2()
{
    delete p_vn_addr;
}

void CDecoder_OMS_fixed_NEON16_v2::setOffset(int _offset)
{
	offset = _offset;
}

void CDecoder_OMS_fixed_NEON16_v2::decode(signed char Intrinsic_fix[], signed char Rprime_fix[], int nombre_iterations)
{
	decode_8bits(Intrinsic_fix, Rprime_fix, nombre_iterations);
}

bool CDecoder_OMS_fixed_NEON16_v2::decode_8bits(signed char Intrinsic_fix[], signed char Rprime_fix[], int nombre_iterations)
{
    ////////////////////////////////////////////////////////////////////////////
    //
    // Initilisation des espaces memoire
    //
    //for (int i=0; i<MESSAGE; i++){
    //    var_mesgs[i] = zero;
    //}
    //
    ////////////////////////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////////////////////////    
    //
    // ENTRELACEMENT DES DONNEES D'ENTREE POUR POUVOIR EXPLOITER LE MODE SIMD
    //
    if( NOEUD%16 == 0  ){
        uchar_transpose_neon((trans_TYPE*)Intrinsic_fix, (trans_TYPE*)var_nodes, NOEUD);
    }else{
        signed char *ptrVar = (signed char*) var_nodes;
        for (int i=0; i<NOEUD; i++){
            for (int z=0; z<16; z++){
                ptrVar[16 * i + z] = Intrinsic_fix[z * NOEUD + i];
            }
        }
    }
    //
    ////////////////////////////////////////////////////////////////////////////

    if( 1 )    
    {
	nombre_iterations--;
        TYPE *p_msg1w               = var_mesgs;
        int8x16_t** p_indice_nod1   = p_vn_addr;
        int8x16_t** p_indice_nod2   = p_vn_addr;
        //const TYPE min_var = VECTOR_SET1( vSAT_NEG_VAR );
        const TYPE max_msg = VECTOR_SET1( vSAT_POS_MSG );

#ifdef DEG_1
        for (int i=0; i<DEG_1_COMPUTATIONS; i++){
            
            TYPE tab_vContr[DEG_1];
            TYPE sign = VECTOR_ZERO;
            TYPE min1 = VECTOR_SET1(127);
            TYPE min2 = min1;

            for(int j=0; j<DEG_1; j++){
                TYPE vContr   = VECTOR_LOAD( *p_indice_nod1 );
                TYPE cSign    = VECTOR_GET_SIGN_BIT(vContr);
                sign          = VECTOR_XOR(sign, cSign);
                TYPE vAbs     = VECTOR_ABS( vContr );
                tab_vContr[j] = vContr;
                TYPE vTemp    = min1;
                min1          = VECTOR_MIN_1(vAbs, min1);
                min2          = VECTOR_MIN_2(vAbs, vTemp, min2);
                p_indice_nod1 += 1;
            }

            const uTYPE offSet = VECTOR_SET1u(0x01);
            const TYPE  cste_1 = VECTOR_MIN(VECTOR_SBU(min2, offSet), max_msg);
            const TYPE  cste_2 = VECTOR_MIN(VECTOR_SBU(min1, offSet), max_msg);

            for(int j=0 ; j<DEG_1 ; j++) {
                    TYPE vContr = tab_vContr[j];
                    TYPE vAbs   = VECTOR_ABS     ( vContr );
                    TYPE vRes   = VECTOR_CMOV    (vAbs, min1, cste_1, cste_2);
                    TYPE vSig   = VECTOR_XOR     (sign, VECTOR_GET_SIGN_BIT(vContr));
                    TYPE v2St   = VECTOR_invSIGN2(vRes,   vSig);
                    TYPE v2Sr   = VECTOR_ADD     (vContr, v2St);
                    VECTOR_STORE( p_msg1w,        v2St);
                    VECTOR_STORE( *p_indice_nod2, v2Sr);
                    p_msg1w        += 1;
                    p_indice_nod2  += 1;
            }
        }
#endif
/////////////////////////////////////////////////////////////////////////////////
#ifdef DEG_2
        for (int i=0; i<DEG_2_COMPUTATIONS; i++){

            TYPE tab_vContr[DEG_2];
            TYPE sign = VECTOR_ZERO;
            TYPE min1 = VECTOR_SET1(vSAT_POS_VAR);
            TYPE min2 = min1;

            for(int j=0 ; j<DEG_2 ; j++){
                TYPE vContr   = VECTOR_LOAD( *p_indice_nod1 );
                TYPE cSign    = VECTOR_GET_SIGN_BIT(vContr);
                sign          = VECTOR_XOR(sign, cSign);
                TYPE vAbs     = VECTOR_ABS( vContr );
                tab_vContr[j] = vContr;
                TYPE vTemp    = min1;
                min1          = VECTOR_MIN_1(vAbs, min1);
                min2          = VECTOR_MIN_2(vAbs, vTemp, min2);
                p_indice_nod1 += 1;
            }

            uTYPE offSet = VECTOR_SET1u(0x01);
            TYPE cste_1 = VECTOR_MIN( VECTOR_SBU(min2, offSet), max_msg);
            TYPE cste_2 = VECTOR_MIN( VECTOR_SBU(min1, offSet), max_msg);

            for(int j=0 ; j<DEG_2 ; j++) {
                    TYPE vContr = tab_vContr[j];
                    TYPE vAbs   = VECTOR_ABS     ( vContr );
                    TYPE vRes   = VECTOR_CMOV    (vAbs, min1, cste_1, cste_2);
                    TYPE vSig   = VECTOR_XOR     (sign, VECTOR_GET_SIGN_BIT(vContr));
                    TYPE v2St   = VECTOR_invSIGN2(vRes,   vSig);
                    TYPE v2Sr   = VECTOR_ADD     (vContr, v2St);
                    VECTOR_STORE( p_msg1w,        v2St);
                    VECTOR_STORE( *p_indice_nod2, v2Sr);
                    p_msg1w        += 1;
                    p_indice_nod2  += 1;
            }
        }
#endif
/////////////////////////////////////////////////////////////////////////////////
#ifdef DEG_3
        for (int i=0; i<DEG_3_COMPUTATIONS; i++){

            TYPE tab_vContr[DEG_3];
            TYPE sign = VECTOR_ZERO;
            TYPE min1 = VECTOR_SET1(vSAT_POS_VAR);
            TYPE min2 = min1;

            for(int j=0 ; j<DEG_3 ; j++){
                TYPE vContr   = VECTOR_LOAD( *p_indice_nod1 );
                TYPE cSign    = VECTOR_GET_SIGN_BIT(vContr);
                sign          = VECTOR_XOR(sign, cSign);
                TYPE vAbs     = VECTOR_ABS( vContr );
                tab_vContr[j] = vContr;
                TYPE vTemp    = min1;
                min1          = VECTOR_MIN_1(vAbs, min1);
                min2          = VECTOR_MIN_2(vAbs, vTemp, min2);
                p_indice_nod1 += 1;
            }

            uTYPE offSet = VECTOR_SET1u(0x01);
            TYPE cste_1 = VECTOR_MIN( VECTOR_SBU(min2, offSet), max_msg);
            TYPE cste_2 = VECTOR_MIN( VECTOR_SBU(min1, offSet), max_msg);

            for(int j=0 ; j<DEG_3 ; j++) {
                    TYPE vContr = tab_vContr[j];
                    TYPE vAbs   = VECTOR_ABS     ( vContr );
                    TYPE vRes   = VECTOR_CMOV    (vAbs, min1, cste_1, cste_2);
                    TYPE vSig   = VECTOR_XOR     (sign, VECTOR_GET_SIGN_BIT(vContr));
                    TYPE v2St   = VECTOR_invSIGN2(vRes,   vSig);
                    TYPE v2Sr   = VECTOR_ADD     (vContr, v2St);
                    VECTOR_STORE( p_msg1w,        v2St);
                    VECTOR_STORE( *p_indice_nod2, v2Sr);
                    p_msg1w        += 1;
                    p_indice_nod2  += 1;
            }
        }
#endif
/////////////////////////////////////////////////////////////////////////////////
#ifdef DEG_4
        for (int i=0; i<DEG_4_COMPUTATIONS; i++){

            TYPE tab_vContr[DEG_4];
            TYPE sign = VECTOR_ZERO;
            TYPE min1 = VECTOR_SET1(vSAT_POS_VAR);
            TYPE min2 = min1;

            for(int j=0 ; j<DEG_4 ; j++){
                TYPE vContr   = VECTOR_LOAD( *p_indice_nod1 );
                TYPE cSign    = VECTOR_GET_SIGN_BIT(vContr);
                sign          = VECTOR_XOR(sign, cSign);
                TYPE vAbs     = VECTOR_ABS( vContr );
                tab_vContr[j] = vContr;
                TYPE vTemp    = min1;
                min1          = VECTOR_MIN_1(vAbs, min1);
                min2          = VECTOR_MIN_2(vAbs, vTemp, min2);
                p_indice_nod1 += 1;
            }

            uTYPE offSet = VECTOR_SET1u(0x01);
            TYPE cste_1 = VECTOR_MIN( VECTOR_SBU(min2, offSet), max_msg);
            TYPE cste_2 = VECTOR_MIN( VECTOR_SBU(min1, offSet), max_msg);

            for(int j=0 ; j<DEG_4 ; j++) {
                    TYPE vContr = tab_vContr[j];
                    TYPE vAbs   = VECTOR_ABS     ( vContr );
                    TYPE vRes   = VECTOR_CMOV    (vAbs, min1, cste_1, cste_2);
                    TYPE vSig   = VECTOR_XOR     (sign, VECTOR_GET_SIGN_BIT(vContr));
                    TYPE v2St   = VECTOR_invSIGN2(vRes,   vSig);
                    TYPE v2Sr   = VECTOR_ADD     (vContr, v2St);
                    VECTOR_STORE( p_msg1w,        v2St);
                    VECTOR_STORE( *p_indice_nod2, v2Sr);
                    p_msg1w        += 1;
                    p_indice_nod2  += 1;
            }
        }
#endif
/////////////////////////////////////////////////////////////////////////////////
#ifdef DEG_5
        for (int i=0; i<DEG_5_COMPUTATIONS; i++){

            TYPE tab_vContr[DEG_5];
            TYPE sign = VECTOR_ZERO;
            TYPE min1 = VECTOR_SET1(vSAT_POS_VAR);
            TYPE min2 = min1;

            for(int j=0 ; j<DEG_5 ; j++){
                TYPE vContr   = VECTOR_LOAD( *p_indice_nod1 );
                TYPE cSign    = VECTOR_GET_SIGN_BIT(vContr);
                sign          = VECTOR_XOR(sign, cSign);
                TYPE vAbs     = VECTOR_ABS( vContr );
                tab_vContr[j] = vContr;
                TYPE vTemp    = min1;
                min1          = VECTOR_MIN_1(vAbs, min1);
                min2          = VECTOR_MIN_2(vAbs, vTemp, min2);
                p_indice_nod1 += 1;
            }

            uTYPE offSet = VECTOR_SET1u(0x01);
            TYPE cste_1 = VECTOR_MIN( VECTOR_SBU(min2, offSet), max_msg);
            TYPE cste_2 = VECTOR_MIN( VECTOR_SBU(min1, offSet), max_msg);

            for(int j=0 ; j<DEG_5 ; j++) {
                    TYPE vContr = tab_vContr[j];
                    TYPE vAbs   = VECTOR_ABS     ( vContr );
                    TYPE vRes   = VECTOR_CMOV    (vAbs, min1, cste_1, cste_2);
                    TYPE vSig   = VECTOR_XOR     (sign, VECTOR_GET_SIGN_BIT(vContr));
                    TYPE v2St   = VECTOR_invSIGN2(vRes,   vSig);
                    TYPE v2Sr   = VECTOR_ADD     (vContr, v2St);
                    VECTOR_STORE( p_msg1w,        v2St);
                    VECTOR_STORE( *p_indice_nod2, v2Sr);
                    p_msg1w        += 1;
                    p_indice_nod2  += 1;
            }
        }
#endif
/////////////////////////////////////////////////////////////////////////////////
#ifdef DEG_6
        for (int i=0; i<DEG_6_COMPUTATIONS; i++){

            TYPE tab_vContr[DEG_6];
            TYPE sign = VECTOR_ZERO;
            TYPE min1 = VECTOR_SET1(vSAT_POS_VAR);
            TYPE min2 = min1;

            for(int j=0 ; j<DEG_6 ; j++){
                TYPE vContr   = VECTOR_LOAD( *p_indice_nod1 );
                TYPE cSign    = VECTOR_GET_SIGN_BIT(vContr);
                sign          = VECTOR_XOR(sign, cSign);
                TYPE vAbs     = VECTOR_ABS( vContr );
                tab_vContr[j] = vContr;
                TYPE vTemp    = min1;
                min1          = VECTOR_MIN_1(vAbs, min1);
                min2          = VECTOR_MIN_2(vAbs, vTemp, min2);
                p_indice_nod1 += 1;
            }

            uTYPE offSet = VECTOR_SET1u(0x01);
            TYPE cste_1 = VECTOR_MIN( VECTOR_SBU(min2, offSet), max_msg);
            TYPE cste_2 = VECTOR_MIN( VECTOR_SBU(min1, offSet), max_msg);

            for(int j=0 ; j<DEG_6 ; j++) {
                    TYPE vContr = tab_vContr[j];
                    TYPE vAbs   = VECTOR_ABS     ( vContr );
                    TYPE vRes   = VECTOR_CMOV    (vAbs, min1, cste_1, cste_2);
                    TYPE vSig   = VECTOR_XOR     (sign, VECTOR_GET_SIGN_BIT(vContr));
                    TYPE v2St   = VECTOR_invSIGN2(vRes,   vSig);
                    TYPE v2Sr   = VECTOR_ADD     (vContr, v2St);
                    VECTOR_STORE( p_msg1w,        v2St);
                    VECTOR_STORE( *p_indice_nod2, v2Sr);
                    p_msg1w        += 1;
                    p_indice_nod2  += 1;
            }
        }
#endif
/////////////////////////////////////////////////////////////////////////////////
#ifdef DEG_7
        for (int i=0; i<DEG_7_COMPUTATIONS; i++){

            TYPE tab_vContr[DEG_7];
            TYPE sign = VECTOR_ZERO;
            TYPE min1 = VECTOR_SET1(vSAT_POS_VAR);
            TYPE min2 = min1;

            for(int j=0 ; j<DEG_7 ; j++){
                TYPE vContr   = VECTOR_LOAD( *p_indice_nod1 );
                TYPE cSign    = VECTOR_GET_SIGN_BIT(vContr);
                sign          = VECTOR_XOR(sign, cSign);
                TYPE vAbs     = VECTOR_ABS( vContr );
                tab_vContr[j] = vContr;
                TYPE vTemp    = min1;
                min1          = VECTOR_MIN_1(vAbs, min1);
                min2          = VECTOR_MIN_2(vAbs, vTemp, min2);
                p_indice_nod1 += 1;
            }

            uTYPE offSet = VECTOR_SET1u(0x01);
            TYPE cste_1 = VECTOR_MIN( VECTOR_SBU(min2, offSet), max_msg);
            TYPE cste_2 = VECTOR_MIN( VECTOR_SBU(min1, offSet), max_msg);

            for(int j=0 ; j<DEG_7 ; j++) {
                    TYPE vContr = tab_vContr[j];
                    TYPE vAbs   = VECTOR_ABS     ( vContr );
                    TYPE vRes   = VECTOR_CMOV    (vAbs, min1, cste_1, cste_2);
                    TYPE vSig   = VECTOR_XOR     (sign, VECTOR_GET_SIGN_BIT(vContr));
                    TYPE v2St   = VECTOR_invSIGN2(vRes,   vSig);
                    TYPE v2Sr   = VECTOR_ADD     (vContr, v2St);
                    VECTOR_STORE( p_msg1w,        v2St);
                    VECTOR_STORE( *p_indice_nod2, v2Sr);
                    p_msg1w        += 1;
                    p_indice_nod2  += 1;
            }
        }
#endif
/////////////////////////////////////////////////////////////////////////////////
#ifdef DEG_8
        for (int i=0; i<DEG_8_COMPUTATIONS; i++){

            TYPE tab_vContr[DEG_8];
            TYPE sign = VECTOR_ZERO;
            TYPE min1 = VECTOR_SET1(vSAT_POS_VAR);
            TYPE min2 = min1;

            for(int j=0 ; j<DEG_8 ; j++){
                TYPE vContr   = VECTOR_LOAD( *p_indice_nod1 );
                TYPE cSign    = VECTOR_GET_SIGN_BIT(vContr);
                sign          = VECTOR_XOR(sign, cSign);
                TYPE vAbs     = VECTOR_ABS( vContr );
                tab_vContr[j] = vContr;
                TYPE vTemp    = min1;
                min1          = VECTOR_MIN_1(vAbs, min1);
                min2          = VECTOR_MIN_2(vAbs, vTemp, min2);
                p_indice_nod1 += 1;
            }

            uTYPE offSet = VECTOR_SET1u(0x01);
            TYPE cste_1 = VECTOR_MIN( VECTOR_SBU(min2, offSet), max_msg);
            TYPE cste_2 = VECTOR_MIN( VECTOR_SBU(min1, offSet), max_msg);

            for(int j=0 ; j<DEG_8 ; j++) {
                    TYPE vContr = tab_vContr[j];
                    TYPE vAbs   = VECTOR_ABS     ( vContr );
                    TYPE vRes   = VECTOR_CMOV    (vAbs, min1, cste_1, cste_2);
                    TYPE vSig   = VECTOR_XOR     (sign, VECTOR_GET_SIGN_BIT(vContr));
                    TYPE v2St   = VECTOR_invSIGN2(vRes,   vSig);
                    TYPE v2Sr   = VECTOR_ADD     (vContr, v2St);
                    VECTOR_STORE( p_msg1w,        v2St);
                    VECTOR_STORE( *p_indice_nod2, v2Sr);
                    p_msg1w        += 1;
                    p_indice_nod2  += 1;
            }
        }
#endif
/////////////////////////////////////////////////////////////////////////////////
#ifdef DEG_9
        for (int i=0; i<DEG_9_COMPUTATIONS; i++){

            TYPE tab_vContr[DEG_9];
            TYPE sign = VECTOR_ZERO;
            TYPE min1 = VECTOR_SET1(vSAT_POS_VAR);
            TYPE min2 = min1;

            for(int j=0 ; j<DEG_9 ; j++){
                TYPE vContr   = VECTOR_LOAD( *p_indice_nod1 );
                TYPE cSign    = VECTOR_GET_SIGN_BIT(vContr);
                sign          = VECTOR_XOR(sign, cSign);
                TYPE vAbs     = VECTOR_ABS( vContr );
                tab_vContr[j] = vContr;
                TYPE vTemp    = min1;
                min1          = VECTOR_MIN_1(vAbs, min1);
                min2          = VECTOR_MIN_2(vAbs, vTemp, min2);
                p_indice_nod1 += 1;
            }

            uTYPE offSet = VECTOR_SET1u(0x01);
            TYPE cste_1 = VECTOR_MIN( VECTOR_SBU(min2, offSet), max_msg);
            TYPE cste_2 = VECTOR_MIN( VECTOR_SBU(min1, offSet), max_msg);

            for(int j=0 ; j<DEG_9 ; j++) {
                    TYPE vContr = tab_vContr[j];
                    TYPE vAbs   = VECTOR_ABS     ( vContr );
                    TYPE vRes   = VECTOR_CMOV    (vAbs, min1, cste_1, cste_2);
                    TYPE vSig   = VECTOR_XOR     (sign, VECTOR_GET_SIGN_BIT(vContr));
                    TYPE v2St   = VECTOR_invSIGN2(vRes,   vSig);
                    TYPE v2Sr   = VECTOR_ADD     (vContr, v2St);
                    VECTOR_STORE( p_msg1w,        v2St);
                    VECTOR_STORE( *p_indice_nod2, v2Sr);
                    p_msg1w        += 1;
                    p_indice_nod2  += 1;
            }
        }
#endif
/////////////////////////////////////////////////////////////////////////////////
#ifdef DEG_10
        for (int i=0; i<DEG_10_COMPUTATIONS; i++){

            TYPE tab_vContr[DEG_10];
            TYPE sign = VECTOR_ZERO;
            TYPE min1 = VECTOR_SET1(vSAT_POS_VAR);
            TYPE min2 = min1;

            for(int j=0 ; j<DEG_10 ; j++){
                TYPE vContr   = VECTOR_LOAD( *p_indice_nod1 );
                TYPE cSign    = VECTOR_GET_SIGN_BIT(vContr);
                sign          = VECTOR_XOR(sign, cSign);
                TYPE vAbs     = VECTOR_ABS( vContr );
                tab_vContr[j] = vContr;
                TYPE vTemp    = min1;
                min1          = VECTOR_MIN_1(vAbs, min1);
                min2          = VECTOR_MIN_2(vAbs, vTemp, min2);
                p_indice_nod1 += 1;
            }

            uTYPE offSet = VECTOR_SET1u(0x01);
            TYPE cste_1 = VECTOR_MIN( VECTOR_SBU(min2, offSet), max_msg);
            TYPE cste_2 = VECTOR_MIN( VECTOR_SBU(min1, offSet), max_msg);

            for(int j=0 ; j<DEG_10 ; j++) {
                    TYPE vContr = tab_vContr[j];
                    TYPE vAbs   = VECTOR_ABS     ( vContr );
                    TYPE vRes   = VECTOR_CMOV    (vAbs, min1, cste_1, cste_2);
                    TYPE vSig   = VECTOR_XOR     (sign, VECTOR_GET_SIGN_BIT(vContr));
                    TYPE v2St   = VECTOR_invSIGN2(vRes,   vSig);
                    TYPE v2Sr   = VECTOR_ADD     (vContr, v2St);
                    VECTOR_STORE( p_msg1w,        v2St);
                    VECTOR_STORE( *p_indice_nod2, v2Sr);
                    p_msg1w        += 1;
                    p_indice_nod2  += 1;
            }
        }
#endif
/////////////////////////////////////////////////////////////////////////////////
#ifdef DEG_11
        for (int i=0; i<DEG_11_COMPUTATIONS; i++){

            TYPE tab_vContr[DEG_11];
            TYPE sign = VECTOR_ZERO;
            TYPE min1 = VECTOR_SET1(vSAT_POS_VAR);
            TYPE min2 = min1;

            for(int j=0 ; j<DEG_11 ; j++){
                TYPE vContr   = VECTOR_LOAD( *p_indice_nod1 );
                TYPE cSign    = VECTOR_GET_SIGN_BIT(vContr);
                sign          = VECTOR_XOR(sign, cSign);
                TYPE vAbs     = VECTOR_ABS( vContr );
                tab_vContr[j] = vContr;
                TYPE vTemp    = min1;
                min1          = VECTOR_MIN_1(vAbs, min1);
                min2          = VECTOR_MIN_2(vAbs, vTemp, min2);
                p_indice_nod1 += 1;
            }

            uTYPE offSet = VECTOR_SET1u(0x01);
            TYPE cste_1 = VECTOR_MIN( VECTOR_SBU(min2, offSet), max_msg);
            TYPE cste_2 = VECTOR_MIN( VECTOR_SBU(min1, offSet), max_msg);

            for(int j=0 ; j<DEG_11 ; j++) {
                    TYPE vContr = tab_vContr[j];
                    TYPE vAbs   = VECTOR_ABS     ( vContr );
                    TYPE vRes   = VECTOR_CMOV    (vAbs, min1, cste_1, cste_2);
                    TYPE vSig   = VECTOR_XOR     (sign, VECTOR_GET_SIGN_BIT(vContr));
                    TYPE v2St   = VECTOR_invSIGN2(vRes,   vSig);
                    TYPE v2Sr   = VECTOR_ADD     (vContr, v2St);
                    VECTOR_STORE( p_msg1w,        v2St);
                    VECTOR_STORE( *p_indice_nod2, v2Sr);
                    p_msg1w        += 1;
                    p_indice_nod2  += 1;
            }
        }
#endif
/////////////////////////////////////////////////////////////////////////////////
#ifdef DEG_12
    #error "The number of DEGREE(Cn) IS HIGHER THAN 11. YOU NEED TO PERFORM A COPY PASTE IN SOURCE CODE..."
#endif
    }




    //
    //
    // ON REPREND LE TRAITEMENT NORMAL DE L'INFORMATION
    //
    //

    while (nombre_iterations--) {
        TYPE *p_msg1r               = var_mesgs;
        TYPE *p_msg1w               = var_mesgs;
        int8x16_t** p_indice_nod1   = p_vn_addr;
        int8x16_t** p_indice_nod2   = p_vn_addr;

//        arret = 0;

        //const TYPE min_var = VECTOR_SET1( vSAT_NEG_VAR );
        const TYPE max_msg = VECTOR_SET1( vSAT_POS_MSG );

#ifdef DEG_1
        for (int i=0; i<DEG_1_COMPUTATIONS; i++){
            
            TYPE tab_vContr[DEG_1];
            TYPE sign = VECTOR_ZERO;
            TYPE min1 = VECTOR_SET1(vSAT_POS_VAR);
            TYPE min2 = min1;

            __builtin_prefetch (p_indice_nod1 + DEG_1,   0, 3);
            __builtin_prefetch (p_indice_nod1 + DEG_1+4, 0, 3);
            for(int j=0; j<DEG_1; j++){
                TYPE vNoeud = VECTOR_LOAD( *p_indice_nod1 );
                TYPE vMessg = VECTOR_LOAD(p_msg1r);
                if( (j & 0x01) == 0 ) __builtin_prefetch (p_msg1r+DEG_1, 0, 0);
                TYPE vContr = VECTOR_SUB(vNoeud, vMessg);
                TYPE cSign  = VECTOR_GET_SIGN_BIT(vContr);
                sign        = VECTOR_XOR(sign, cSign);
                TYPE vAbs   = VECTOR_ABS( vContr );
                tab_vContr[j] = vContr;
                TYPE vTemp = min1;
                min1       = VECTOR_MIN_1(vAbs, min1);
                min2       = VECTOR_MIN_2(vAbs, vTemp, min2);
                p_indice_nod1 += 1;
                p_msg1r       += 1;
            }
            for(int j=0; j<DEG_1; j++){
            	__builtin_prefetch ( p_indice_nod1[j], 0, 3);
            }

            uTYPE offSet = VECTOR_SET1u(0x01);
            TYPE cste_1 = VECTOR_MIN( VECTOR_SBU(min2, offSet), max_msg);
            TYPE cste_2 = VECTOR_MIN( VECTOR_SBU(min1, offSet), max_msg);

            for(int j=0 ; j<DEG_1 ; j++) {
                    TYPE vContr = tab_vContr[j];
                    TYPE vAbs   = VECTOR_ABS    (vContr);
                    TYPE vRes   = VECTOR_CMOV   (vAbs, min1, cste_1, cste_2);
                    TYPE vSig   = VECTOR_XOR    (sign, VECTOR_GET_SIGN_BIT(vContr));
                    TYPE v2St   = VECTOR_invSIGN2(vRes, vSig);
                    TYPE v2Sr   = VECTOR_ADD(vContr, v2St);
                    VECTOR_STORE( p_msg1w,                      v2St);
                    VECTOR_STORE( *p_indice_nod2, v2Sr);
                    p_msg1w        += 1;
                    p_indice_nod2  += 1;
            }
        }
#endif
/////////////////////////////////////////////////////////////////////////////////
#ifdef DEG_2
        for (int i=0; i<DEG_2_COMPUTATIONS; i++){

            TYPE tab_vContr[DEG_2];
            TYPE sign = VECTOR_ZERO;
            TYPE min1 = VECTOR_SET1(vSAT_POS_VAR);
            TYPE min2 = min1;

            __builtin_prefetch (p_indice_nod1 + DEG_2,   0, 3);
            __builtin_prefetch (p_indice_nod1 + DEG_2+4, 0, 3);
            for(int j=0 ; j<DEG_2 ; j++)
            {            
                TYPE vNoeud = VECTOR_LOAD( *p_indice_nod1 );
                TYPE vMessg = VECTOR_LOAD(p_msg1r);
                if( (j & 0x01) == 0 ) __builtin_prefetch (p_msg1r+DEG_2, 0, 0);
                TYPE vContr   = VECTOR_SUB(vNoeud, vMessg);
                TYPE cSign    = VECTOR_GET_SIGN_BIT(vContr);
                sign          = VECTOR_XOR(sign, cSign);
                TYPE vAbs     = VECTOR_ABS( vContr );
                tab_vContr[j] = vContr;
                TYPE vTemp    = min1;
                min1          = VECTOR_MIN_1(vAbs, min1);
                min2          = VECTOR_MIN_2(vAbs, vTemp, min2);
                p_indice_nod1 += 1;
                p_msg1r       += 1;
            }
            for(int j=0; j<DEG_2; j++){
            	__builtin_prefetch ( p_indice_nod1[j], 0, 3);
            }

            uTYPE offSet = VECTOR_SET1u(0x01);
            TYPE cste_1 = VECTOR_MIN( VECTOR_SBU(min2, offSet), max_msg);
            TYPE cste_2 = VECTOR_MIN( VECTOR_SBU(min1, offSet), max_msg);

            for(int j=0 ; j<DEG_2 ; j++) {
                    TYPE vContr = tab_vContr[j];
                    TYPE vAbs   = VECTOR_ABS    (vContr);
//                    TYPE vAbs   = VECTOR_ABS    ( VECTOR_MIN(vContr, max_msg) );
                    TYPE vRes   = VECTOR_CMOV   (vAbs, min1, cste_1, cste_2);
//                    vRes        = VECTOR_MIN(vRes, max_msg); // BLG
                    TYPE vSig   = VECTOR_XOR    (sign, VECTOR_GET_SIGN_BIT(vContr));
                    TYPE v2St   = VECTOR_invSIGN2(vRes, vSig);
//                    TYPE v2Sr   = VECTOR_ADD_AND_SATURATE_VAR_8bits(vContr, v2St, min_var);
                    TYPE v2Sr   = VECTOR_ADD(vContr, v2St);
                    VECTOR_STORE( p_msg1w, v2St);
                    VECTOR_STORE( *p_indice_nod2, v2Sr);
                    p_msg1w        += 1;
                    p_indice_nod2  += 1;
            }
        }
#endif
/////////////////////////////////////////////////////////////////////////////////
#ifdef DEG_3
        for (int i=0; i<DEG_3_COMPUTATIONS; i++){

            TYPE tab_vContr[DEG_3];
            TYPE sign = VECTOR_ZERO;
            TYPE min1 = VECTOR_SET1(vSAT_POS_VAR);
            TYPE min2 = min1;

            __builtin_prefetch (p_indice_nod1 + DEG_3,   0, 3);
            __builtin_prefetch (p_indice_nod1 + DEG_3+4, 0, 3);
            for(int j=0 ; j<DEG_3 ; j++)
            {            
                TYPE vNoeud = VECTOR_LOAD( *p_indice_nod1 );
                TYPE vMessg = VECTOR_LOAD(p_msg1r);
                if( (j & 0x01) == 0 ) __builtin_prefetch (p_msg1r+DEG_3, 0, 0);
                TYPE vContr   = VECTOR_SUB(vNoeud, vMessg);
                TYPE cSign    = VECTOR_GET_SIGN_BIT(vContr);
                sign          = VECTOR_XOR(sign, cSign);
                TYPE vAbs     = VECTOR_ABS( vContr );
                tab_vContr[j] = vContr;
                TYPE vTemp    = min1;
                min1          = VECTOR_MIN_1(vAbs, min1);
                min2          = VECTOR_MIN_2(vAbs, vTemp, min2);
                p_indice_nod1 += 1;
                p_msg1r       += 1;
            }
            for(int j=0; j<DEG_3; j++){
            	__builtin_prefetch ( p_indice_nod1[j], 0, 3);
            }

            uTYPE offSet = VECTOR_SET1u(0x01);
            TYPE cste_1 = VECTOR_MIN( VECTOR_SBU(min2, offSet), max_msg);
            TYPE cste_2 = VECTOR_MIN( VECTOR_SBU(min1, offSet), max_msg);

            for(int j=0 ; j<DEG_3 ; j++) {
                    TYPE vContr = tab_vContr[j];
                    TYPE vAbs   = VECTOR_ABS    (vContr);
//                    TYPE vAbs   = VECTOR_ABS    ( VECTOR_MIN(vContr, max_msg) );
                    TYPE vRes   = VECTOR_CMOV   (vAbs, min1, cste_1, cste_2);
//                    vRes        = VECTOR_MIN(vRes, max_msg); // BLG
                    TYPE vSig   = VECTOR_XOR    (sign, VECTOR_GET_SIGN_BIT(vContr));
                    TYPE v2St   = VECTOR_invSIGN2(vRes, vSig);
//                    TYPE v2Sr   = VECTOR_ADD_AND_SATURATE_VAR_8bits(vContr, v2St, min_var);
                    TYPE v2Sr   = VECTOR_ADD(vContr, v2St);
                    VECTOR_STORE( p_msg1w, v2St);
                    VECTOR_STORE( *p_indice_nod2, v2Sr);
                    p_msg1w        += 1;
                    p_indice_nod2  += 1;
            }
        }
#endif
/////////////////////////////////////////////////////////////////////////////////
#ifdef DEG_4
        for (int i=0; i<DEG_4_COMPUTATIONS; i++){

            TYPE tab_vContr[DEG_4];
            TYPE sign = VECTOR_ZERO;
            TYPE min1 = VECTOR_SET1(vSAT_POS_VAR);
            TYPE min2 = min1;

            __builtin_prefetch (p_indice_nod1 + DEG_4,   0, 3);
            __builtin_prefetch (p_indice_nod1 + DEG_4+4, 0, 3);
            for(int j=0 ; j<DEG_4 ; j++)
            {            
                TYPE vNoeud = VECTOR_LOAD( *p_indice_nod1 );
                TYPE vMessg = VECTOR_LOAD(p_msg1r);
                if( (j & 0x01) == 0 ) __builtin_prefetch (p_msg1r+DEG_4, 0, 0);
                TYPE vContr   = VECTOR_SUB(vNoeud, vMessg);
                TYPE cSign    = VECTOR_GET_SIGN_BIT(vContr);
                sign          = VECTOR_XOR(sign, cSign);
                TYPE vAbs     = VECTOR_ABS( vContr );
                tab_vContr[j] = vContr;
                TYPE vTemp    = min1;
                min1          = VECTOR_MIN_1(vAbs, min1);
                min2          = VECTOR_MIN_2(vAbs, vTemp, min2);
                p_indice_nod1 += 1;
                p_msg1r       += 1;
            }
            for(int j=0; j<DEG_4; j++){
            	__builtin_prefetch ( p_indice_nod1[j], 0, 3);
            }

            uTYPE offSet = VECTOR_SET1u(0x01);
            TYPE cste_1 = VECTOR_MIN( VECTOR_SBU(min2, offSet), max_msg);
            TYPE cste_2 = VECTOR_MIN( VECTOR_SBU(min1, offSet), max_msg);

            for(int j=0 ; j<DEG_4 ; j++) {
                    TYPE vContr = tab_vContr[j];
                    TYPE vAbs   = VECTOR_ABS    (vContr);
//                    TYPE vAbs   = VECTOR_ABS    ( VECTOR_MIN(vContr, max_msg) );
                    TYPE vRes   = VECTOR_CMOV   (vAbs, min1, cste_1, cste_2);
//                    vRes        = VECTOR_MIN(vRes, max_msg); // BLG
                    TYPE vSig   = VECTOR_XOR    (sign, VECTOR_GET_SIGN_BIT(vContr));
                    TYPE v2St   = VECTOR_invSIGN2(vRes, vSig);
//                    TYPE v2Sr   = VECTOR_ADD_AND_SATURATE_VAR_8bits(vContr, v2St, min_var);
                    TYPE v2Sr   = VECTOR_ADD(vContr, v2St);
                    VECTOR_STORE( p_msg1w, v2St);
                    VECTOR_STORE( *p_indice_nod2, v2Sr);
                    p_msg1w        += 1;
                    p_indice_nod2  += 1;
            }
        }
#endif
/////////////////////////////////////////////////////////////////////////////////
#ifdef DEG_5
        for (int i=0; i<DEG_5_COMPUTATIONS; i++){

            TYPE tab_vContr[DEG_5];
            TYPE sign = VECTOR_ZERO;
            TYPE min1 = VECTOR_SET1(vSAT_POS_VAR);
            TYPE min2 = min1;

            __builtin_prefetch (p_indice_nod1 + DEG_5,   0, 3);
            __builtin_prefetch (p_indice_nod1 + DEG_5+4, 0, 3);
            for(int j=0 ; j<DEG_5 ; j++)
            {            
                TYPE vNoeud = VECTOR_LOAD( *p_indice_nod1 );
                TYPE vMessg = VECTOR_LOAD(p_msg1r);
                if( (j & 0x01) == 0 ) __builtin_prefetch (p_msg1r+DEG_5, 0, 0);
                TYPE vContr   = VECTOR_SUB(vNoeud, vMessg);
                TYPE cSign    = VECTOR_GET_SIGN_BIT(vContr);
                sign          = VECTOR_XOR(sign, cSign);
                TYPE vAbs     = VECTOR_ABS( vContr );
                tab_vContr[j] = vContr;
                TYPE vTemp    = min1;
                min1          = VECTOR_MIN_1(vAbs, min1);
                min2          = VECTOR_MIN_2(vAbs, vTemp, min2);
                p_indice_nod1 += 1;
                p_msg1r       += 1;
            }
            for(int j=0; j<DEG_5; j++){
            	__builtin_prefetch ( p_indice_nod1[j], 0, 3);
            }

            uTYPE offSet = VECTOR_SET1u(0x01);
            TYPE cste_1 = VECTOR_MIN( VECTOR_SBU(min2, offSet), max_msg);
            TYPE cste_2 = VECTOR_MIN( VECTOR_SBU(min1, offSet), max_msg);

            for(int j=0 ; j<DEG_5 ; j++) {
                    TYPE vContr = tab_vContr[j];
                    TYPE vAbs   = VECTOR_ABS    (vContr);
//                    TYPE vAbs   = VECTOR_ABS    ( VECTOR_MIN(vContr, max_msg) );
                    TYPE vRes   = VECTOR_CMOV   (vAbs, min1, cste_1, cste_2);
//                    vRes        = VECTOR_MIN(vRes, max_msg); // BLG
                    TYPE vSig   = VECTOR_XOR    (sign, VECTOR_GET_SIGN_BIT(vContr));
                    TYPE v2St   = VECTOR_invSIGN2(vRes, vSig);
//                    TYPE v2Sr   = VECTOR_ADD_AND_SATURATE_VAR_8bits(vContr, v2St, min_var);
                    TYPE v2Sr   = VECTOR_ADD(vContr, v2St);
                    VECTOR_STORE( p_msg1w, v2St);
                    VECTOR_STORE( *p_indice_nod2, v2Sr);
                    p_msg1w        += 1;
                    p_indice_nod2  += 1;
            }
        }
#endif
/////////////////////////////////////////////////////////////////////////////////
#ifdef DEG_6
        for (int i=0; i<DEG_6_COMPUTATIONS; i++){

            TYPE tab_vContr[DEG_6];
            TYPE sign = VECTOR_ZERO;
            TYPE min1 = VECTOR_SET1(vSAT_POS_VAR);
            TYPE min2 = min1;

            __builtin_prefetch (p_indice_nod1 + DEG_6,   0, 3);
            __builtin_prefetch (p_indice_nod1 + DEG_6+4, 0, 3);
            for(int j=0 ; j<DEG_6 ; j++)
            {            
                TYPE vNoeud = VECTOR_LOAD( *p_indice_nod1 );
                TYPE vMessg = VECTOR_LOAD(p_msg1r);
                if( (j & 0x01) == 0 ) __builtin_prefetch (p_msg1r+DEG_6, 0, 0);
                TYPE vContr   = VECTOR_SUB(vNoeud, vMessg);
                TYPE cSign    = VECTOR_GET_SIGN_BIT(vContr);
                sign          = VECTOR_XOR(sign, cSign);
                TYPE vAbs     = VECTOR_ABS( vContr );
                tab_vContr[j] = vContr;
                TYPE vTemp    = min1;
                min1          = VECTOR_MIN_1(vAbs, min1);
                min2          = VECTOR_MIN_2(vAbs, vTemp, min2);
                p_indice_nod1 += 1;
                p_msg1r       += 1;
            }
            for(int j=0; j<DEG_6; j++){
            	__builtin_prefetch ( p_indice_nod1[j], 0, 3);
            }

            uTYPE offSet = VECTOR_SET1u(0x01);
            TYPE cste_1 = VECTOR_MIN( VECTOR_SBU(min2, offSet), max_msg);
            TYPE cste_2 = VECTOR_MIN( VECTOR_SBU(min1, offSet), max_msg);

            for(int j=0 ; j<DEG_6 ; j++) {
                    TYPE vContr = tab_vContr[j];
                    TYPE vAbs   = VECTOR_ABS    (vContr);
//                    TYPE vAbs   = VECTOR_ABS    ( VECTOR_MIN(vContr, max_msg) );
                    TYPE vRes   = VECTOR_CMOV   (vAbs, min1, cste_1, cste_2);
//                    vRes        = VECTOR_MIN(vRes, max_msg); // BLG
                    TYPE vSig   = VECTOR_XOR    (sign, VECTOR_GET_SIGN_BIT(vContr));
                    TYPE v2St   = VECTOR_invSIGN2(vRes, vSig);
//                    TYPE v2Sr   = VECTOR_ADD_AND_SATURATE_VAR_8bits(vContr, v2St, min_var);
                    TYPE v2Sr   = VECTOR_ADD(vContr, v2St);
                    VECTOR_STORE( p_msg1w, v2St);
                    VECTOR_STORE( *p_indice_nod2, v2Sr);
                    p_msg1w        += 1;
                    p_indice_nod2  += 1;
            }
        }
#endif
/////////////////////////////////////////////////////////////////////////////////
#ifdef DEG_7
        for (int i=0; i<DEG_7_COMPUTATIONS; i++){

            TYPE tab_vContr[DEG_7];
            TYPE sign = VECTOR_ZERO;
            TYPE min1 = VECTOR_SET1(vSAT_POS_VAR);
            TYPE min2 = min1;

            __builtin_prefetch (p_indice_nod1 + DEG_7,   0, 3);
            __builtin_prefetch (p_indice_nod1 + DEG_7+4, 0, 3);
            for(int j=0 ; j<DEG_7 ; j++)
            {            
                TYPE vNoeud = VECTOR_LOAD( *p_indice_nod1 );
                TYPE vMessg = VECTOR_LOAD(p_msg1r);
                if( (j & 0x01) == 0 ) __builtin_prefetch (p_msg1r+DEG_7, 0, 0);
                TYPE vContr   = VECTOR_SUB(vNoeud, vMessg);
                TYPE cSign    = VECTOR_GET_SIGN_BIT(vContr);
                sign          = VECTOR_XOR(sign, cSign);
                TYPE vAbs     = VECTOR_ABS( vContr );
                tab_vContr[j] = vContr;
                TYPE vTemp    = min1;
                min1          = VECTOR_MIN_1(vAbs, min1);
                min2          = VECTOR_MIN_2(vAbs, vTemp, min2);
                p_indice_nod1 += 1;
                p_msg1r       += 1;
            }
            for(int j=0; j<DEG_7; j++){
            	__builtin_prefetch ( p_indice_nod1[j], 0, 3);
            }

            uTYPE offSet = VECTOR_SET1u(0x01);
            TYPE cste_1 = VECTOR_MIN( VECTOR_SBU(min2, offSet), max_msg);
            TYPE cste_2 = VECTOR_MIN( VECTOR_SBU(min1, offSet), max_msg);

            for(int j=0 ; j<DEG_7 ; j++) {
                    TYPE vContr = tab_vContr[j];
                    TYPE vAbs   = VECTOR_ABS    (vContr);
//                    TYPE vAbs   = VECTOR_ABS    ( VECTOR_MIN(vContr, max_msg) );
                    TYPE vRes   = VECTOR_CMOV   (vAbs, min1, cste_1, cste_2);
//                    vRes        = VECTOR_MIN(vRes, max_msg); // BLG
                    TYPE vSig   = VECTOR_XOR    (sign, VECTOR_GET_SIGN_BIT(vContr));
                    TYPE v2St   = VECTOR_invSIGN2(vRes, vSig);
//                    TYPE v2Sr   = VECTOR_ADD_AND_SATURATE_VAR_8bits(vContr, v2St, min_var);
                    TYPE v2Sr   = VECTOR_ADD(vContr, v2St);
                    VECTOR_STORE( p_msg1w, v2St);
                    VECTOR_STORE( *p_indice_nod2, v2Sr);
                    p_msg1w        += 1;
                    p_indice_nod2  += 1;
            }
        }
#endif
/////////////////////////////////////////////////////////////////////////////////
#ifdef DEG_8
        for (int i=0; i<DEG_8_COMPUTATIONS; i++){

            TYPE tab_vContr[DEG_8];
            TYPE sign = VECTOR_ZERO;
            TYPE min1 = VECTOR_SET1(vSAT_POS_VAR);
            TYPE min2 = min1;

            __builtin_prefetch (p_indice_nod1 + DEG_8,   0, 3);
            __builtin_prefetch (p_indice_nod1 + DEG_8+4, 0, 3);
            for(int j=0 ; j<DEG_8 ; j++)
            {            
                TYPE vNoeud = VECTOR_LOAD( *p_indice_nod1 );
                TYPE vMessg = VECTOR_LOAD(p_msg1r);
                if( (j & 0x01) == 0 ) __builtin_prefetch (p_msg1r+DEG_8, 0, 0);
                TYPE vContr   = VECTOR_SUB(vNoeud, vMessg);
                TYPE cSign    = VECTOR_GET_SIGN_BIT(vContr);
                sign          = VECTOR_XOR(sign, cSign);
                TYPE vAbs     = VECTOR_ABS( vContr );
                tab_vContr[j] = vContr;
                TYPE vTemp    = min1;
                min1          = VECTOR_MIN_1(vAbs, min1);
                min2          = VECTOR_MIN_2(vAbs, vTemp, min2);
                p_indice_nod1 += 1;
                p_msg1r       += 1;
            }
            for(int j=0; j<DEG_8; j++){
            	__builtin_prefetch ( p_indice_nod1[j], 0, 3);
            }

            uTYPE offSet = VECTOR_SET1u(0x01);
            TYPE cste_1 = VECTOR_MIN( VECTOR_SBU(min2, offSet), max_msg);
            TYPE cste_2 = VECTOR_MIN( VECTOR_SBU(min1, offSet), max_msg);

            for(int j=0 ; j<DEG_8 ; j++) {
                    TYPE vContr = tab_vContr[j];
                    TYPE vAbs   = VECTOR_ABS    (vContr);
//                    TYPE vAbs   = VECTOR_ABS    ( VECTOR_MIN(vContr, max_msg) );
                    TYPE vRes   = VECTOR_CMOV   (vAbs, min1, cste_1, cste_2);
//                    vRes        = VECTOR_MIN(vRes, max_msg); // BLG
                    TYPE vSig   = VECTOR_XOR    (sign, VECTOR_GET_SIGN_BIT(vContr));
                    TYPE v2St   = VECTOR_invSIGN2(vRes, vSig);
//                    TYPE v2Sr   = VECTOR_ADD_AND_SATURATE_VAR_8bits(vContr, v2St, min_var);
                    TYPE v2Sr   = VECTOR_ADD(vContr, v2St);
                    VECTOR_STORE( p_msg1w, v2St);
                    VECTOR_STORE( *p_indice_nod2, v2Sr);
                    p_msg1w        += 1;
                    p_indice_nod2  += 1;
            }
        }
#endif
/////////////////////////////////////////////////////////////////////////////////
#ifdef DEG_9
        for (int i=0; i<DEG_9_COMPUTATIONS; i++){

            TYPE tab_vContr[DEG_9];
            TYPE sign = VECTOR_ZERO;
            TYPE min1 = VECTOR_SET1(vSAT_POS_VAR);
            TYPE min2 = min1;

            __builtin_prefetch (p_indice_nod1 + DEG_9,   0, 3);
            __builtin_prefetch (p_indice_nod1 + DEG_9+4, 0, 3);
            for(int j=0 ; j<DEG_9 ; j++)
            {            
                TYPE vNoeud = VECTOR_LOAD( *p_indice_nod1 );
                TYPE vMessg = VECTOR_LOAD(p_msg1r);
                if( (j & 0x01) == 0 ) __builtin_prefetch (p_msg1r+DEG_9, 0, 0);
                TYPE vContr   = VECTOR_SUB(vNoeud, vMessg);
                TYPE cSign    = VECTOR_GET_SIGN_BIT(vContr);
                sign          = VECTOR_XOR(sign, cSign);
                TYPE vAbs     = VECTOR_ABS( vContr );
                tab_vContr[j] = vContr;
                TYPE vTemp    = min1;
                min1          = VECTOR_MIN_1(vAbs, min1);
                min2          = VECTOR_MIN_2(vAbs, vTemp, min2);
                p_indice_nod1 += 1;
                p_msg1r       += 1;
            }
            for(int j=0; j<DEG_9; j++){
            	__builtin_prefetch ( p_indice_nod1[j], 0, 3);
            }

            uTYPE offSet = VECTOR_SET1u(0x01);
            TYPE cste_1 = VECTOR_MIN( VECTOR_SBU(min2, offSet), max_msg);
            TYPE cste_2 = VECTOR_MIN( VECTOR_SBU(min1, offSet), max_msg);

            for(int j=0 ; j<DEG_9 ; j++) {
                    TYPE vContr = tab_vContr[j];
                    TYPE vAbs   = VECTOR_ABS    (vContr);
//                    TYPE vAbs   = VECTOR_ABS    ( VECTOR_MIN(vContr, max_msg) );
                    TYPE vRes   = VECTOR_CMOV   (vAbs, min1, cste_1, cste_2);
//                    vRes        = VECTOR_MIN(vRes, max_msg); // BLG
                    TYPE vSig   = VECTOR_XOR    (sign, VECTOR_GET_SIGN_BIT(vContr));
                    TYPE v2St   = VECTOR_invSIGN2(vRes, vSig);
//                    TYPE v2Sr   = VECTOR_ADD_AND_SATURATE_VAR_8bits(vContr, v2St, min_var);
                    TYPE v2Sr   = VECTOR_ADD(vContr, v2St);
                    VECTOR_STORE( p_msg1w, v2St);
                    VECTOR_STORE( *p_indice_nod2, v2Sr);
                    p_msg1w        += 1;
                    p_indice_nod2  += 1;
            }
        }
#endif
/////////////////////////////////////////////////////////////////////////////////
#ifdef DEG_10
        for (int i=0; i<DEG_10_COMPUTATIONS; i++){

            TYPE tab_vContr[DEG_10];
            TYPE sign = VECTOR_ZERO;
            TYPE min1 = VECTOR_SET1(vSAT_POS_VAR);
            TYPE min2 = min1;

            __builtin_prefetch (p_indice_nod1 + DEG_10,   0, 3);
            __builtin_prefetch (p_indice_nod1 + DEG_10+4, 0, 3);
            for(int j=0 ; j<DEG_10 ; j++)
            {            
                TYPE vNoeud = VECTOR_LOAD( *p_indice_nod1 );
                TYPE vMessg = VECTOR_LOAD(p_msg1r);
                if( (j & 0x01) == 0 ) __builtin_prefetch (p_msg1r+DEG_10, 0, 0);
                TYPE vContr   = VECTOR_SUB(vNoeud, vMessg);
                TYPE cSign    = VECTOR_GET_SIGN_BIT(vContr);
                sign          = VECTOR_XOR(sign, cSign);
                TYPE vAbs     = VECTOR_ABS( vContr );
                tab_vContr[j] = vContr;
                TYPE vTemp    = min1;
                min1          = VECTOR_MIN_1(vAbs, min1);
                min2          = VECTOR_MIN_2(vAbs, vTemp, min2);
                p_indice_nod1 += 1;
                p_msg1r       += 1;
            }
            for(int j=0; j<DEG_10; j++){
            	__builtin_prefetch ( p_indice_nod1[j], 0, 3);
            }

            uTYPE offSet = VECTOR_SET1u(0x01);
            TYPE cste_1 = VECTOR_MIN( VECTOR_SBU(min2, offSet), max_msg);
            TYPE cste_2 = VECTOR_MIN( VECTOR_SBU(min1, offSet), max_msg);

            for(int j=0 ; j<DEG_10 ; j++) {
                    TYPE vContr = tab_vContr[j];
                    TYPE vAbs   = VECTOR_ABS    (vContr);
//                    TYPE vAbs   = VECTOR_ABS    ( VECTOR_MIN(vContr, max_msg) );
                    TYPE vRes   = VECTOR_CMOV   (vAbs, min1, cste_1, cste_2);
//                    vRes        = VECTOR_MIN(vRes, max_msg); // BLG
                    TYPE vSig   = VECTOR_XOR    (sign, VECTOR_GET_SIGN_BIT(vContr));
                    TYPE v2St   = VECTOR_invSIGN2(vRes, vSig);
//                    TYPE v2Sr   = VECTOR_ADD_AND_SATURATE_VAR_8bits(vContr, v2St, min_var);
                    TYPE v2Sr   = VECTOR_ADD(vContr, v2St);
                    VECTOR_STORE( p_msg1w, v2St);
                    VECTOR_STORE( *p_indice_nod2, v2Sr);
                    p_msg1w        += 1;
                    p_indice_nod2  += 1;
            }
        }
#endif
/////////////////////////////////////////////////////////////////////////////////
#ifdef DEG_11
        for (int i=0; i<DEG_11_COMPUTATIONS; i++){

            TYPE tab_vContr[DEG_11];
            TYPE sign = VECTOR_ZERO;
            TYPE min1 = VECTOR_SET1(vSAT_POS_VAR);
            TYPE min2 = min1;

            __builtin_prefetch (p_indice_nod1 + DEG_11,   0, 3);
            __builtin_prefetch (p_indice_nod1 + DEG_11+4, 0, 3);
            for(int j=0 ; j<DEG_11 ; j++)
            {            
                TYPE vNoeud = VECTOR_LOAD( *p_indice_nod1 );
                TYPE vMessg = VECTOR_LOAD(p_msg1r);
                if( (j & 0x01) == 0 ) __builtin_prefetch (p_msg1r+DEG_11, 0, 0);
                TYPE vContr   = VECTOR_SUB(vNoeud, vMessg);
                TYPE cSign    = VECTOR_GET_SIGN_BIT(vContr);
                sign          = VECTOR_XOR(sign, cSign);
                TYPE vAbs     = VECTOR_ABS( vContr );
                tab_vContr[j] = vContr;
                TYPE vTemp    = min1;
                min1          = VECTOR_MIN_1(vAbs, min1);
                min2          = VECTOR_MIN_2(vAbs, vTemp, min2);
                p_indice_nod1 += 1;
                p_msg1r       += 1;
            }
            for(int j=0; j<DEG_11; j++){
            	__builtin_prefetch ( p_indice_nod1[j], 0, 3);
            }

            uTYPE offSet = VECTOR_SET1u(0x01);
            TYPE cste_1 = VECTOR_MIN( VECTOR_SBU(min2, offSet), max_msg);
            TYPE cste_2 = VECTOR_MIN( VECTOR_SBU(min1, offSet), max_msg);

            for(int j=0 ; j<DEG_11 ; j++) {
                    TYPE vContr = tab_vContr[j];
                    TYPE vAbs   = VECTOR_ABS    (vContr);
//                    TYPE vAbs   = VECTOR_ABS    ( VECTOR_MIN(vContr, max_msg) );
                    TYPE vRes   = VECTOR_CMOV   (vAbs, min1, cste_1, cste_2);
//                    vRes        = VECTOR_MIN(vRes, max_msg); // BLG
                    TYPE vSig   = VECTOR_XOR    (sign, VECTOR_GET_SIGN_BIT(vContr));
                    TYPE v2St   = VECTOR_invSIGN2(vRes, vSig);
//                    TYPE v2Sr   = VECTOR_ADD_AND_SATURATE_VAR_8bits(vContr, v2St, min_var);
                    TYPE v2Sr   = VECTOR_ADD(vContr, v2St);
                    VECTOR_STORE( p_msg1w, v2St);
                    VECTOR_STORE( *p_indice_nod2, v2Sr);
                    p_msg1w        += 1;
                    p_indice_nod2  += 1;
            }
        }
#endif
/////////////////////////////////////////////////////////////////////////////////
#ifdef DEG_12
    #error "The number of DEGREE(Cn) IS HIGHER THAN 11. YOU NEED TO PERFORM A COPY PASTE IN SOURCE CODE..."
#endif
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    // ON REMET EN FORME LES DONNEES DE SORTIE POUR LA SUITE DU PROCESS
    //
    if( NOEUD%16 == 0  ){
        uchar_itranspose_neon((trans_TYPE*)var_nodes, (trans_TYPE*)Rprime_fix, NOEUD);
    }else{
        signed char* ptr = (signed char*) var_nodes;
        for (int i=0; i<NOEUD; i+=1){
            for (int j=0; j<16; j+=1){
                Rprime_fix[j*NOEUD +i] = (ptr[16*i+j] > 0);
            }
        }
    }
    //
    ////////////////////////////////////////////////////////////////////////////

    return 0;
}


