/**
  Copyright (c) 2012-2015 "Bordeaux INP, Bertrand LE GAL"
  [http://legal.vvv.enseirb-matmeca.fr]
  This file is part of LDPC_C_Simulator.
  LDPC_C_Simulator is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "CDecoder_OMS_fixed_NEON8.h"

#define TYPE int8x8_t

#define VECTOR_LOAD(ptr)            ((ptr)[0])
#define VECTOR_UNCACHED_LOAD(ptr)   ((ptr)[0])
#define VECTOR_STORE(ptr,v)         ((ptr)[0])=v

#define VECTOR_ADD(a,b)             (vqaddq_s8(a,b)) //
#define VECTOR_SUB(a,b)             (vqsubq_s8(a,b)) //
#define VECTOR_ABS(a)               (vabsq_s8(a))    //
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
#define VECTOR_NEG(a)               (vqnegq_s8 (a,8))
#define VECTOR_CMP_EQ(a,b)          (vceqq_s8(a,b))
#define VECTOR_CMP_LTEQ(a,b)        (vcleq_s8(a,b))
#define VECTOR_NOT(a)               (vmvnq_s8(a))
#define VECTOR_GET_SIGN_BIT(a,b)    (SSE_16S_AND(a, b))
#define VECTOR_MIN(a,b)             (VECTOR_MIN_1(a,b))

#define VECTOR_ADD(a,b)             (vqaddq_s8(a,b))
#define VECTOR_SBU(a,b)             (vqsubq_u8(a,b))
#define VECTOR_SUB(a,b)             (vqsubq_s8(a,b))
#define VECTOR_ANDNOT(a,b)          (VECTOR_AND(a,VECTOR_NOT(b))) // OUPS
#define VECTOR_SIGN(a,b)            (VECTOR_XOR(a,b))    // OUPS
#define VECTOR_EQUAL(a,b)           (VECTOR_CMP_EQ(a,b))
#define VECTOR_ZERO                 (VECTOR_SET1u(0))
#define VECTOR_SET1(a)              (VECTOR_SET1i(a))

#define VECTOR_MIN_2(val,old_min1,min2) \
    (VECTOR_MIN(min2,VECTOR_MAX(val,old_min1)))

#define VECTOR_VAR_SATURATE(a) \
    (VECTOR_MAX(VECTOR_MIN(a, max_var), min_var))

#define VECTOR_SATURATE(a, max, min) \
    (VECTOR_MAX(VECTOR_MIN(a, max), min))

#define VECTOR_SUB_AND_SATURATE_VAR_8bits(a,b,min) \
    (VECTOR_MAX(VECTOR_SUB(a,b), min)) // ON DOIT CONSERVER LA SATURATION MIN A CAUSE DE -128

#define VECTOR_ADD_AND_SATURATE_VAR_8bits(a,b,min) \
    (VECTOR_MAX(VECTOR_ADD(a,b), min)) // ON DOIT CONSERVER LA SATURATION MIN A CAUSE DE -128

#define VECTOR_SUB_AND_SATURATE_VAR(a,b,max,min) \
    (VECTOR_SATURATE(VECTOR_SUB(a,b),max,min))

#define VECTOR_ADD_AND_SATURATE_VAR(a,b,max,min) \
    (VECTOR_SATURATE(VECTOR_ADD(a,b),max,min))

#define VECTOR_invSIGN2(val,sig) \
    (VECTOR_SIGN(val, sig))

inline TYPE VECTOR_GET_SIGN_BIT(TYPE a, TYPE m){
    TYPE b = VECTOR_AND(a, m);
    return b;
}

inline TYPE VECTOR_CMOV( TYPE a, TYPE b, TYPE c, TYPE d){
    TYPE z = VECTOR_EQUAL  ( a, b );
//    return _mm_blendv_epi8( d, c, z );
    TYPE g = VECTOR_AND   ( c, z );
    TYPE h = VECTOR_ANDNOT( z, d );
    return VECTOR_OR         ( g, h );
}

inline int VECTOR_XOR_REDUCE(TYPE reg){
    unsigned int *p;
    p = (unsigned int*)&reg;
    return (( (p[0] & 0x80808080) | (p[1] & 0x80808080) | (p[2] & 0x80808080) | (p[3] & 0x80808080) ) != 0);
//    return ((p[0] | p[1] | p[2] | p[3]) != 0);
}


CDecoder_OMS_fixed_SSE::CDecoder_OMS_fixed_SSE()
{
    offset = -1;
}

void CDecoder_OMS_fixed_SSE::setOffset(int _offset)
{
    if( offset == -1 ){
        offset = _offset;
    }else{
        printf("(EE) Offset value was already configured !\n");
        exit( 0 );
    }
}

void CDecoder_OMS_fixed_SSE::decode(signed char Intrinsic_fix[], signed char Rprime_fix[], int nombre_iterations)
{
	decode_8bits(Intrinsic_fix, Rprime_fix, nombre_iterations);
}

bool CDecoder_OMS_fixed_SSE::decode_8bits(signed char Intrinsic_fix[], signed char Rprime_fix[], int nombre_iterations)
{
    ////////////////////////////////////////////////////////////////////////////
    //
    // Initilisation des espaces memoire
    //
    const TYPE zero = VECTOR_ZERO;
    for (int i=0; i<MESSAGE; i++){
        var_mesgs[i] = zero;
    }
    //
    ////////////////////////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////////////////////////
    //
    // ENTRELACEMENT DES DONNEES D'ENTREE POUR POUVOIR EXPLOITER LE MODE SIMD
    //
//    if( NOEUD%16 == 0  ){
//        uchar_transpose_sse((TYPE*)Intrinsic_fix, (TYPE*)var_nodes, NOEUD);
//    }else{
        const int SIMD = 8;
        char *ptrVar = (char*) var_nodes;
        for (int i=0; i<NOEUD; i++){
            for (int z=0; z<SIMD; z++){
                ptrVar[SIMD * i + z] = Intrinsic_fix[z * NOEUD + i];
            }
        }
//    }
    //
    ////////////////////////////////////////////////////////////////////////////


//    unsigned int arret = 0;

    while (nombre_iterations--) {
        TYPE *p_msg1r                      = var_mesgs;
        TYPE *p_msg1w                      = var_mesgs;
        const unsigned short *p_indice_nod1   = PosNoeudsVariable;
        const unsigned short *p_indice_nod2   = PosNoeudsVariable;

//        arret = 0;

        const TYPE min_var = VECTOR_SET1( vSAT_NEG_VAR );
        const TYPE max_msg = VECTOR_SET1( vSAT_POS_MSG );

#if NB_DEGRES >= 1
        for (int i=0; i<DEG_1_COMPUTATIONS; i++){

            TYPE tab_vContr[DEG_1];
            TYPE sign = VECTOR_ZERO;
            TYPE min1 = VECTOR_SET1(vSAT_POS_VAR);
            TYPE min2 = min1;

#if (DEG_1 & 0x01) == 1
        const unsigned char sign8   = 0x80;
        const unsigned char isign8  = 0xC0;
        const TYPE msign8        = VECTOR_SET1( sign8   );
        const TYPE misign8       = VECTOR_SET1( isign8  );
#else
        const unsigned char sign8   = 0x80;
        const unsigned char isign8b = 0x40;
        const TYPE msign8        = VECTOR_SET1( sign8   );
        const TYPE misign8b      = VECTOR_SET1( isign8b );
#endif

            for(int j=0; j<DEG_1; j++){
                TYPE vNoeud = VECTOR_LOAD(&var_nodes[(*p_indice_nod1)]);
                TYPE vMessg = VECTOR_LOAD(p_msg1r);
                TYPE vContr = VECTOR_SUB_AND_SATURATE_VAR_8bits(vNoeud, vMessg, min_var);
                TYPE cSign  = VECTOR_GET_SIGN_BIT(vContr, msign8);
                sign        = VECTOR_XOR(sign, cSign);
                TYPE vAbs   = VECTOR_ABS( VECTOR_MIN(vContr, max_msg) );
                tab_vContr[j] = vContr;
                TYPE vTemp = min1;
                min1       = VECTOR_MIN_1(vAbs, min1);
                min2       = VECTOR_MIN_2(vAbs, vTemp, min2);
                p_indice_nod1 += 1;
                p_msg1r += 1;
            }

            TYPE cste_1 = VECTOR_MIN(VECTOR_SBU(min2, VECTOR_SET1(offset)), max_msg); // ON SATURE DIREECTEMENT AU FORMAT MSG
            TYPE cste_2 = VECTOR_MIN(VECTOR_SBU(min1, VECTOR_SET1(offset)), max_msg); // ON SATURE DIREECTEMENT AU FORMAT MSG

#if (DEG_1 & 0x01) == 1
            sign = VECTOR_XOR(sign, misign8);
#else
            sign = VECTOR_XOR(sign, misign8b);
#endif

            for(int j=0 ; j<DEG_1 ; j++) {
                    TYPE vContr = tab_vContr[j];
                    TYPE vAbs   = VECTOR_ABS    ( VECTOR_MIN(vContr, max_msg) );
                    TYPE vRes   = VECTOR_CMOV   (vAbs, min1, cste_1, cste_2);
                    TYPE vSig   = VECTOR_XOR    (sign, VECTOR_GET_SIGN_BIT(vContr, msign8));
                    TYPE v2St   = VECTOR_invSIGN2(vRes, vSig);
                    TYPE v2Sr   = VECTOR_ADD_AND_SATURATE_VAR_8bits(vContr, v2St, min_var);
                    VECTOR_STORE( p_msg1w,                      v2St);
                    VECTOR_STORE( &var_nodes[(*p_indice_nod2)], v2Sr);
                    p_msg1w        += 1;
                    p_indice_nod2  += 1;
            }
        }
#endif
/////////////////////////////////////////////////////////////////////////////////
#if NB_DEGRES >= 2
        for (int i=0; i<DEG_2_COMPUTATIONS; i++){

#if (DEG_2 & 0x01) == 1
        const unsigned char sign8   = 0x80;
        const unsigned char isign8  = 0xC0;
        const TYPE msign8        = VECTOR_SET1( sign8   );
        const TYPE misign8       = VECTOR_SET1( isign8  );
#else
        const unsigned char sign8   = 0x80;
        const unsigned char isign8b = 0x40;
        const TYPE msign8        = VECTOR_SET1( sign8   );
        const TYPE misign8b      = VECTOR_SET1( isign8b );
#endif

            TYPE tab_vContr[DEG_2];
            TYPE sign = zero;
            TYPE min1 = VECTOR_SET1(vSAT_POS_VAR);
            TYPE min2 = min1;

            for(int j=0 ; j<DEG_2 ; j++)
            {
                    TYPE vNoeud = VECTOR_LOAD( &var_nodes[(*p_indice_nod1)] );
                    TYPE vMessg = VECTOR_LOAD( p_msg1r );
                    TYPE vContr = VECTOR_SUB_AND_SATURATE_VAR_8bits(vNoeud, vMessg, min_var);
                    TYPE cSign  = VECTOR_GET_SIGN_BIT(vContr, msign8);
                    sign        = VECTOR_XOR (sign, cSign);
                    TYPE vAbs   = VECTOR_ABS ( VECTOR_MIN(vContr, max_msg) );
                    tab_vContr[j]  = vContr;
                    TYPE vTemp     = min1;
                    min1           = VECTOR_MIN_1(vAbs, min1      );
                    min2           = VECTOR_MIN_2(vAbs, vTemp, min2);
                    p_indice_nod1 += 1;
                    p_msg1r       += 1;
            }

            TYPE cste_1   = VECTOR_MIN( VECTOR_SBU(min2, VECTOR_SET1(offset)), max_msg); // ON SATURE DIREECTEMENT AU FORMAT MSG
            TYPE cste_2   = VECTOR_MIN( VECTOR_SBU(min1, VECTOR_SET1(offset)), max_msg); // ON SATURE DIREECTEMENT AU FORMAT MSG

#if (DEG_2 & 0x01) == 1
            sign = VECTOR_XOR(sign, misign8);
#else
            sign = VECTOR_XOR(sign, misign8b);
#endif

            for(int j=0 ; j<DEG_2 ; j++) {
                    TYPE vContr = tab_vContr[j];
                    TYPE vAbs   = VECTOR_ABS    ( VECTOR_MIN(vContr, max_msg) );
                    TYPE vRes   = VECTOR_CMOV   (vAbs, min1, cste_1, cste_2);
                    TYPE vSig   = VECTOR_XOR    (sign, VECTOR_GET_SIGN_BIT(vContr, msign8));
                    TYPE v2St   = VECTOR_invSIGN2(vRes, vSig);
                    TYPE v2Sr   = VECTOR_ADD_AND_SATURATE_VAR_8bits(vContr, v2St, min_var);
                    VECTOR_STORE( p_msg1w,                      v2St);
                    VECTOR_STORE( &var_nodes[(*p_indice_nod2)], v2Sr);
                    p_msg1w        += 1;
                    p_indice_nod2  += 1;
            }
        }
#endif
/////////////////////////////////////////////////////////////////////////////////
#if NB_DEGRES >= 3
        for (int i=0; i<DEG_3_COMPUTATIONS; i++){

#if (DEG_3 & 0x01) == 1
        const unsigned char sign8   = 0x80;
        const unsigned char isign8  = 0xC0;
        const TYPE msign8        = VECTOR_SET1( sign8   );
        const TYPE misign8       = VECTOR_SET1( isign8  );
#else
        const unsigned char sign8   = 0x80;
        const unsigned char isign8b = 0x40;
        const TYPE msign8        = VECTOR_SET1( sign8   );
        const TYPE misign8b      = VECTOR_SET1( isign8b );
#endif

            TYPE tab_vContr[DEG_3];
            TYPE sign = zero;
            TYPE min1 = VECTOR_SET1(vSAT_POS_VAR);
            TYPE min2 = min1;

            for(int j=0 ; j<DEG_3 ; j++)
            {
                    TYPE vNoeud = VECTOR_LOAD( &var_nodes[(*p_indice_nod1)] );
                    TYPE vMessg = VECTOR_LOAD( p_msg1r );
                    TYPE vContr = VECTOR_SUB_AND_SATURATE_VAR_8bits(vNoeud, vMessg, min_var);
                    TYPE cSign  = VECTOR_GET_SIGN_BIT(vContr, msign8);
                    sign        = VECTOR_XOR (sign, cSign);
                    TYPE vAbs   = VECTOR_ABS ( VECTOR_MIN(vContr, max_msg) );
                    tab_vContr[j]  = vContr;
                    TYPE vTemp     = min1;
                    min1           = VECTOR_MIN_1(vAbs, min1      );
                    min2           = VECTOR_MIN_2(vAbs, vTemp, min2);
                    p_indice_nod1 += 1;
                    p_msg1r       += 1;
            }

            TYPE cste_1   = VECTOR_MIN( VECTOR_SBU(min2, VECTOR_SET1(offset)), max_msg); // ON SATURE DIREECTEMENT AU FORMAT MSG
            TYPE cste_2   = VECTOR_MIN( VECTOR_SBU(min1, VECTOR_SET1(offset)), max_msg); // ON SATURE DIREECTEMENT AU FORMAT MSG

#if (DEG_3 & 0x01) == 1
            sign = VECTOR_XOR(sign, misign8);
#else
            sign = VECTOR_XOR(sign, misign8b);
#endif

            for(int j=0 ; j<DEG_3 ; j++) {
                    TYPE vContr = tab_vContr[j];
                    TYPE vAbs   = VECTOR_ABS    ( VECTOR_MIN(vContr, max_msg) );
                    TYPE vRes   = VECTOR_CMOV   (vAbs, min1, cste_1, cste_2);
                    TYPE vSig   = VECTOR_XOR    (sign, VECTOR_GET_SIGN_BIT(vContr, msign8));
                    TYPE v2St   = VECTOR_invSIGN2(vRes, vSig);
                    TYPE v2Sr   = VECTOR_ADD_AND_SATURATE_VAR_8bits(vContr, v2St, min_var);
                    VECTOR_STORE( p_msg1w,                      v2St);
                    VECTOR_STORE( &var_nodes[(*p_indice_nod2)], v2Sr);
                    p_msg1w        += 1;
                    p_indice_nod2  += 1;
            }
        }
#endif
/////////////////////////////////////////////////////////////////////////////////
#if NB_DEGRES >= 4
        for (int i=0; i<DEG_4_COMPUTATIONS; i++){

#if (DEG_4 & 0x01) == 1
        const unsigned char sign8   = 0x80;
        const unsigned char isign8  = 0xC0;
        const TYPE msign8        = VECTOR_SET1( sign8   );
        const TYPE misign8       = VECTOR_SET1( isign8  );
#else
        const unsigned char sign8   = 0x80;
        const unsigned char isign8b = 0x40;
        const TYPE msign8        = VECTOR_SET1( sign8   );
        const TYPE misign8b      = VECTOR_SET1( isign8b );
#endif

            TYPE tab_vContr[DEG_4];
            TYPE sign = zero;
            TYPE min1 = VECTOR_SET1(vSAT_POS_VAR);
            TYPE min2 = min1;

            for(int j=0 ; j<DEG_4 ; j++)
            {
                    TYPE vNoeud = VECTOR_LOAD( &var_nodes[(*p_indice_nod1)] );
                    TYPE vMessg = VECTOR_LOAD( p_msg1r );
                    TYPE vContr = VECTOR_SUB_AND_SATURATE_VAR_8bits(vNoeud, vMessg, min_var);
                    TYPE cSign  = VECTOR_GET_SIGN_BIT(vContr, msign8);
                    sign        = VECTOR_XOR (sign, cSign);
                    TYPE vAbs   = VECTOR_ABS ( VECTOR_MIN(vContr, max_msg) );
                    tab_vContr[j]  = vContr;
                    TYPE vTemp     = min1;
                    min1           = VECTOR_MIN_1(vAbs, min1      );
                    min2           = VECTOR_MIN_2(vAbs, vTemp, min2);
                    p_indice_nod1 += 1;
                    p_msg1r       += 1;
            }

            TYPE cste_1   = VECTOR_MIN( VECTOR_SBU(min2, VECTOR_SET1(offset)), max_msg); // ON SATURE DIREECTEMENT AU FORMAT MSG
            TYPE cste_2   = VECTOR_MIN( VECTOR_SBU(min1, VECTOR_SET1(offset)), max_msg); // ON SATURE DIREECTEMENT AU FORMAT MSG

#if (DEG_4 & 0x01) == 1
            sign = VECTOR_XOR(sign, misign8);
#else
            sign = VECTOR_XOR(sign, misign8b);
#endif

            for(int j=0 ; j<DEG_4 ; j++) {
                    TYPE vContr = tab_vContr[j];
                    TYPE vAbs   = VECTOR_ABS    ( VECTOR_MIN(vContr, max_msg) );
                    TYPE vRes   = VECTOR_CMOV   (vAbs, min1, cste_1, cste_2);
                    TYPE vSig   = VECTOR_XOR    (sign, VECTOR_GET_SIGN_BIT(vContr, msign8));
                    TYPE v2St   = VECTOR_invSIGN2(vRes, vSig);
                    TYPE v2Sr   = VECTOR_ADD_AND_SATURATE_VAR_8bits(vContr, v2St, min_var);
                    VECTOR_STORE( p_msg1w,                      v2St);
                    VECTOR_STORE( &var_nodes[(*p_indice_nod2)], v2Sr);
                    p_msg1w        += 1;
                    p_indice_nod2  += 1;
            }
        }
#endif
/////////////////////////////////////////////////////////////////////////////////
#if NB_DEGRES >= 5
        for (int i=0; i<DEG_5_COMPUTATIONS; i++){

#if (DEG_5 & 0x01) == 1
        const unsigned char sign8   = 0x80;
        const unsigned char isign8  = 0xC0;
        const TYPE msign8        = VECTOR_SET1( sign8   );
        const TYPE misign8       = VECTOR_SET1( isign8  );
#else
        const unsigned char sign8   = 0x80;
        const unsigned char isign8b = 0x40;
        const TYPE msign8        = VECTOR_SET1( sign8   );
        const TYPE misign8b      = VECTOR_SET1( isign8b );
#endif

            TYPE tab_vContr[DEG_5];
            TYPE sign = zero;
            TYPE min1 = VECTOR_SET1(vSAT_POS_VAR);
            TYPE min2 = min1;

            for(int j=0 ; j<DEG_5 ; j++)
            {
                    TYPE vNoeud = VECTOR_LOAD( &var_nodes[(*p_indice_nod1)] );
                    TYPE vMessg = VECTOR_LOAD( p_msg1r );
                    TYPE vContr = VECTOR_SUB_AND_SATURATE_VAR_8bits(vNoeud, vMessg, min_var);
                    TYPE cSign  = VECTOR_GET_SIGN_BIT(vContr, msign8);
                    sign        = VECTOR_XOR (sign, cSign);
                    TYPE vAbs   = VECTOR_ABS ( VECTOR_MIN(vContr, max_msg) );
                    tab_vContr[j]  = vContr;
                    TYPE vTemp     = min1;
                    min1           = VECTOR_MIN_1(vAbs, min1      );
                    min2           = VECTOR_MIN_2(vAbs, vTemp, min2);
                    p_indice_nod1 += 1;
                    p_msg1r       += 1;
            }

            TYPE cste_1   = VECTOR_MIN( VECTOR_SBU(min2, VECTOR_SET1(offset)), max_msg); // ON SATURE DIREECTEMENT AU FORMAT MSG
            TYPE cste_2   = VECTOR_MIN( VECTOR_SBU(min1, VECTOR_SET1(offset)), max_msg); // ON SATURE DIREECTEMENT AU FORMAT MSG

#if (DEG_5 & 0x01) == 1
            sign = VECTOR_XOR(sign, misign8);
#else
            sign = VECTOR_XOR(sign, misign8b);
#endif

            for(int j=0 ; j<DEG_5 ; j++) {
                    TYPE vContr = tab_vContr[j];
                    TYPE vAbs   = VECTOR_ABS    ( VECTOR_MIN(vContr, max_msg) );
                    TYPE vRes   = VECTOR_CMOV   (vAbs, min1, cste_1, cste_2);
                    TYPE vSig   = VECTOR_XOR    (sign, VECTOR_GET_SIGN_BIT(vContr, msign8));
                    TYPE v2St   = VECTOR_invSIGN2(vRes, vSig);
                    TYPE v2Sr   = VECTOR_ADD_AND_SATURATE_VAR_8bits(vContr, v2St, min_var);
                    VECTOR_STORE( p_msg1w,                      v2St);
                    VECTOR_STORE( &var_nodes[(*p_indice_nod2)], v2Sr);
                    p_msg1w        += 1;
                    p_indice_nod2  += 1;
            }
        }
#endif
/////////////////////////////////////////////////////////////////////////////////
#if NB_DEGRES > 5
    printf("The number of DEGREE(Cn) IS HIGHER THAN 5. YOU NEED TO PERFORM A COPY PASTE IN SOURCE CODE...\n");
    exit( 0 );
#endif
/////////////////////////////////////////////////////////////////////////////////
        //
        // GESTION DU CRITERE D'ARRET
        //
//        if( (arret == 0) && (fast_stop == 1) ){
//            break;
//        }
    }

    ////////////////////////////////////////////////////////////////////////////
    //
    // ON REMET EN FORME LES DONNEES DE SORTIE POUR LA SUITE DU PROCESS
    //
//    if( NOEUD%16 == 0  ){
//        uchar_itranspose_sse((TYPE*)var_nodes, (TYPE*)Rprime_fix, NOEUD);
//    }else{
        char* ptr = (char*) var_nodes;
        for (int i=0; i<NOEUD; i+=1){
            for (int j=0; j<SIMD; j+=1){
                Rprime_fix[j*NOEUD +i] = (ptr[SIMD*i+j] > 0);
            }
        }
//    }
    //
    ////////////////////////////////////////////////////////////////////////////

    return 0;
}
