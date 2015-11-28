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

#include "CDecoder_OMS_fixed_AVX.h"
#include "../../CTools/transpose_avx.h"

#define TYPE __m256i

#define VECTOR_LOAD(ptr)            (_mm256_load_si256(ptr))
#define VECTOR_UNCACHED_LOAD(ptr)   (_mm256_stream_load_si256(ptr))
#define VECTOR_STORE(ptr,v)         (_mm256_store_si256(ptr,v))
#define VECTOR_ADD(a,b)             (_mm256_adds_epi8(a,b))
#define VECTOR_SBU(a,b)             (_mm256_subs_epu8(a,b))
#define VECTOR_SUB(a,b)             (_mm256_subs_epi8(a,b))
#define VECTOR_ABS(a)               (_mm256_abs_epi8(a))
#define VECTOR_MAX(a,b)             (_mm256_max_epi8(a,b))
#define VECTOR_MIN(a,b)             (_mm256_min_epi8(a,b))
#define VECTOR_XOR(a,b)             (_mm256_xor_si256(a,b))
#define VECTOR_OR(a,b)              (_mm256_or_si256(a,b))
#define VECTOR_AND(a,b)             (_mm256_and_si256(a,b))
#define VECTOR_ANDNOT(a,b)          (_mm256_andnot_si256(a,b))
#define VECTOR_MIN_1(a,min1)        (VECTOR_MIN(a,min1))
#define VECTOR_SIGN(a,b)            (_mm256_sign_epi8(a,b))
#define VECTOR_EQUAL(a,b)           (_mm256_cmpeq_epi8(a,b))
#define VECTOR_ZERO                 (_mm256_setzero_si256())
#define VECTOR_SET1(a)              (_mm256_set1_epi8(a))

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
//    return _mm256_blendv_epi8( d, c, z );
    TYPE g = VECTOR_AND   ( c, z );
    TYPE h = VECTOR_ANDNOT( z, d );
    return VECTOR_OR      ( g, h );
}


inline int VECTOR_XOR_REDUCE(TYPE reg){
//    unsigned int *p;
//    p = (unsigned int*)&reg;
//    return ((p[0] | p[1] | p[2] | p[3] | p[4] | p[5] | p[6] | p[7]) != 0);
    const TYPE mask = VECTOR_SET1( -128 );
    reg = VECTOR_AND(reg, mask);
    unsigned int *p;
    p = (unsigned int*)&reg;
    return (( p[0] | p[1] | p[2]| p[3] | p[4] | p[5] | p[6] | p[7] )) != 0;
}


CDecoder_OMS_fixed_AVX::CDecoder_OMS_fixed_AVX()
{
    nb_exec         = 0;
    nb_saved_iters  = 0;
    offset          = -1;
}


#define VERBOSE 0
CDecoder_OMS_fixed_AVX::~CDecoder_OMS_fixed_AVX()
{
#if VERBOSE == 1
    float sav = (float)nb_saved_iters/(float)nb_exec;
    printf("(DD) # SAVED ITERS / EXEC = %f\n", sav);
#endif    
}


void CDecoder_OMS_fixed_AVX::setOffset(int _offset)
{
    if( offset == -1 ){
        offset = _offset;
    }else{
        printf("(EE) Offset value was already configured !\n");
        exit( 0 );
    }
}


void CDecoder_OMS_fixed_AVX::decode(char Intrinsic_fix[], char Rprime_fix[], int nombre_iterations)
{
    if( vSAT_POS_VAR == 127 )
        decode_8bits(Intrinsic_fix, Rprime_fix, nombre_iterations);
    else
        decode_generic(Intrinsic_fix, Rprime_fix, nombre_iterations);
    nb_exec += 1;
}


bool CDecoder_OMS_fixed_AVX::decode_8bits(char Intrinsic_fix[], char Rprime_fix[], int nombre_iterations)
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
    if( NOEUD%32 == 0  ){
        uchar_transpose_avx((TYPE*)Intrinsic_fix, (TYPE*)var_nodes, NOEUD);
    }else{
        char *ptrVar = (char*)var_nodes;
        for (int i=0; i<NOEUD; i++){
            for (int z=0; z<32; z++){
                ptrVar[32 * i + z] = Intrinsic_fix[z * NOEUD + i];
            }
        }
    }
    //
    ////////////////////////////////////////////////////////////////////////////

    unsigned int arret;

    while (nombre_iterations--) {
        TYPE *p_msg1r                       = var_mesgs;
        TYPE *p_msg1w                       = var_mesgs;
        const unsigned short *p_indice_nod1 = PosNoeudsVariable;
        const unsigned short *p_indice_nod2 = PosNoeudsVariable;

        arret = 0;

        const TYPE min_var = VECTOR_SET1( vSAT_NEG_VAR );
        const TYPE max_msg = VECTOR_SET1( vSAT_POS_MSG );

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

            for(int j=0 ; j<DEG_1 ; j++){
                TYPE vNoeud    = VECTOR_LOAD(&var_nodes[(*p_indice_nod1)]);
                TYPE vMessg    = VECTOR_LOAD(p_msg1r);
                TYPE vContr    = VECTOR_SUB_AND_SATURATE_VAR_8bits(vNoeud, vMessg, min_var);
                TYPE cSign     = VECTOR_GET_SIGN_BIT(vContr, msign8);
                sign           = VECTOR_XOR(sign, cSign);
                TYPE vAbs      = VECTOR_MIN( VECTOR_ABS(vContr), max_msg );
                tab_vContr[j]  = vContr;
                TYPE vTemp     = min1;
                min1           = VECTOR_MIN_1(vAbs, min1);
                min2           = VECTOR_MIN_2(vAbs, vTemp, min2);
                p_indice_nod1 += 1;
                p_msg1r       += 1;
            }

            TYPE cste_1   = VECTOR_MIN( VECTOR_SBU(min2, VECTOR_SET1(offset)), max_msg); // ON SATURE DIREECTEMENT AU FORMAT MSG
            TYPE cste_2   = VECTOR_MIN( VECTOR_SBU(min1, VECTOR_SET1(offset)), max_msg); // ON SATURE DIREECTEMENT AU FORMAT MSG

#if (DEG_1 & 0x01) == 1
            sign = VECTOR_XOR(sign, misign8);
#else
            sign = VECTOR_XOR(sign, misign8b);
#endif

            for(int j=0 ; j<DEG_1 ; j++) {
                    TYPE vContr = tab_vContr[j];
                    TYPE vAbs   = VECTOR_MIN( VECTOR_ABS(vContr), max_msg );
                    TYPE vRes   = VECTOR_CMOV   (vAbs, min1, cste_1, cste_2);
                    TYPE vSig   = VECTOR_XOR    (sign, VECTOR_GET_SIGN_BIT(vContr, msign8));
                    TYPE v2St   = VECTOR_invSIGN2(vRes, vSig);
                    TYPE v2Sr   = VECTOR_ADD_AND_SATURATE_VAR_8bits(vContr, v2St, min_var);
                    VECTOR_STORE( p_msg1w,                      v2St);
                    VECTOR_STORE( &var_nodes[(*p_indice_nod2)], v2Sr);
                    p_msg1w        += 1;
                    p_indice_nod2  += 1;
            }
            arret = arret || VECTOR_XOR_REDUCE( sign );
        }

//////////////////////////
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
                    TYPE vAbs   = VECTOR_MIN( VECTOR_ABS(vContr), max_msg );
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
                    TYPE vAbs   = VECTOR_MIN( VECTOR_ABS(vContr), max_msg );
                    TYPE vRes   = VECTOR_CMOV   (vAbs, min1, cste_1, cste_2);
                    TYPE vSig   = VECTOR_XOR    (sign, VECTOR_GET_SIGN_BIT(vContr, msign8));
                    TYPE v2St   = VECTOR_invSIGN2(vRes, vSig);
                    TYPE v2Sr   = VECTOR_ADD_AND_SATURATE_VAR_8bits(vContr, v2St, min_var);
                    VECTOR_STORE( p_msg1w,                      v2St);
                    VECTOR_STORE( &var_nodes[(*p_indice_nod2)], v2Sr);
                    p_msg1w        += 1;
                    p_indice_nod2  += 1;
            }
            
            arret = arret || VECTOR_XOR_REDUCE( sign );
        }
#endif
#if NB_DEGRES > 2
    printf("The number of DEGREE(Cn) IS HIGHER THAN 2. YOU NEED TO PERFORM A COPY PASTE IN SOURCE CODE...\n");
    exit( 0 );
#endif
        //
        // GESTION DU CRITERE D'ARRET
        //
        if( (arret == 0) && (fast_stop == 1) ){
            nb_saved_iters += nombre_iterations;
//            printf("> Saving 1 : (%d) !\n", nombre_iterations);
            break;
        }

        ////////////////////////////////////////////////////////////////////////
        //
        //
        // CALCUL COMPLET DU CRITERE D'ARRET COMPLET...
        //
        //
        if(fast_stop == 1){
            const unsigned short *p_indice_nod1 = PosNoeudsVariable;
            arret = 0;

            const unsigned char sign8  = 0x80;
            const TYPE const_sign      = VECTOR_SET1( sign8 );
            
            for (int i=0; i<DEG_1_COMPUTATIONS; i++){
                TYPE sign = VECTOR_ZERO;
                for(int j=0 ; j<DEG_1 ; j++){
                    TYPE vContr = VECTOR_LOAD(&var_nodes[(*p_indice_nod1)]);
                    TYPE cSign  = VECTOR_GET_SIGN_BIT(vContr, const_sign);
                    sign        = VECTOR_XOR(sign, VECTOR_XOR(cSign,const_sign));
                    p_indice_nod1 += 1;
                }
                arret = arret || VECTOR_XOR_REDUCE( sign );
//                printf(" -(1)-> arret = %d (%d)\n", arret, VECTOR_XOR_REDUCE( sign ));
            }
            
    #if NB_DEGRES > 1
            for (int i=0; i<DEG_2_COMPUTATIONS; i++){
                TYPE sign = VECTOR_ZERO;
                for(int j=0 ; j<DEG_2 ; j++){
                    TYPE vContr = VECTOR_LOAD(&var_nodes[(*p_indice_nod1)]);
                    TYPE cSign  = VECTOR_GET_SIGN_BIT(vContr, const_sign);
                    sign        = VECTOR_XOR(sign, VECTOR_XOR(cSign,const_sign));
                    p_indice_nod1 += 1;
                }
                arret = arret || VECTOR_XOR_REDUCE( sign );
//                printf(" -(2)-> arret = %d (%d)\n", arret, VECTOR_XOR_REDUCE( sign ));
            }
    #endif
//            printf("> Test for iteration %d = (%d) !\n", nombre_iterations, arret);
            if( arret == 0 ){
                nb_saved_iters += nombre_iterations;
//                printf("> Saving 2 : (%d) !\n", nombre_iterations);
                break;
            }
        }
    //
    //
    // FIN DU CALCUL DU CRITERE D'ARRET (COMPLET)
    //
    ////////////////////////////////////////////////////////////////////////////
        
    }

    ////////////////////////////////////////////////////////////////////////////
    //
    // ON REMET EN FORME LES DONNEES DE SORTIE POUR LA SUITE DU PROCESS
    //
    if( NOEUD%32 == 0  ){
        uchar_itranspose_avx((TYPE*)var_nodes, (TYPE*)Rprime_fix, NOEUD);
    }else{
        char* ptr = (char*)var_nodes;
        for (int i=0; i<NOEUD; i+=1){
            for (int j=0; j<32; j+=1){
                Rprime_fix[j*NOEUD +i] = (ptr[32*i+j] > 0);
            }
        }
    }
    //
    ////////////////////////////////////////////////////////////////////////////
#ifdef SANITY_CHECKS
    int errs = 0;
        char* ptr = (char*)var_nodes;
    for (int i=0; i<32*NOEUD; i+=1){
        errs += (ptr[i] >= 0);
    }
    if((arret == 0) && (errs != 0)){
        printf("Oups : %d\n", errs);
    }
    if((arret != 0) && (errs == 0)){
        printf("Oups 2 : %d\n", errs);
    }
#endif
    return (arret == 0);
}


bool CDecoder_OMS_fixed_AVX::decode_generic(char Intrinsic_fix[], char Rprime_fix[], int nombre_iterations)
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
    if( NOEUD%32 == 0  ){
        uchar_transpose_avx((TYPE*)Intrinsic_fix, (TYPE*)var_nodes, NOEUD);
    }else{
        char *ptrVar = (char*)var_nodes;
        for (int i=0; i<NOEUD; i++){
            for (int z=0; z<32; z++){
                ptrVar[32 * i + z] = Intrinsic_fix[z * NOEUD + i];
            }
        }
    }
    //
    ////////////////////////////////////////////////////////////////////////////

    
    unsigned int arret = 0;

    while (nombre_iterations--) {
        TYPE *p_msg1r                      = var_mesgs;
        TYPE *p_msg1w                      = var_mesgs;
        const unsigned short *p_indice_nod1   = PosNoeudsVariable;
        const unsigned short *p_indice_nod2   = PosNoeudsVariable;

        arret = 0;

        const TYPE min_max_value = VECTOR_SET1(vSAT_POS_VAR);
        const TYPE min_var = VECTOR_SET1( vSAT_NEG_VAR );
        const TYPE max_var = VECTOR_SET1( vSAT_POS_VAR );
        const TYPE max_msg = VECTOR_SET1( vSAT_POS_MSG );

        // CONSTANTE UTILISEE POUR L'EXTRACTION DU SIGNE
        const unsigned char sign8 = 0x80;
        const TYPE msign8  = VECTOR_SET1( sign8 );
        const unsigned char isign8 = 0xC0;
        const TYPE misign8 = VECTOR_SET1( isign8 );


////////////////////////////////////////////////////////////////////////////////////////////////////////////
        for (int i=0; i<DEG_1_COMPUTATIONS; i++){
            
            TYPE tab_vContr[DEG_1];
            TYPE sign = VECTOR_ZERO;
            TYPE min1 = min_max_value;
            TYPE min2 = min_max_value;

            //
            // ON UTILISE UNE PETITE ASTUCE AFIN D'ACCELERER LA SIMULATION DU DECODEUR
            //
            for(int j=0 ; j<DEG_1 ; j++)
            {
                    TYPE vNoeud = VECTOR_LOAD( &var_nodes[(*p_indice_nod1)] );
                    TYPE vMessg = VECTOR_LOAD( p_msg1r );
                    tab_vContr[j]  = VECTOR_SUB_AND_SATURATE_VAR(vNoeud,vMessg,max_var,min_var);
                    TYPE cSign  = VECTOR_GET_SIGN_BIT(tab_vContr[j], msign8);
                    TYPE vAbs   = VECTOR_MIN( VECTOR_ABS(tab_vContr[j]), max_msg );
                    sign           = VECTOR_XOR (sign, cSign);
                    TYPE vTemp  = min1;
                    min1 = VECTOR_MIN_1(vAbs, min1      );
                    min2 = VECTOR_MIN_2(vAbs, vTemp, min2);
                    p_indice_nod1 += 1;
                    p_msg1r       += 1;
            }

            TYPE cste_1   = VECTOR_MIN( VECTOR_SBU(min2, _mm256_set1_epi8(offset)), max_msg); // ON SATURE DIREECTEMENT AU FORMAT MSG
            TYPE cste_2   = VECTOR_MIN( VECTOR_SBU(min1, _mm256_set1_epi8(offset)), max_msg); // ON SATURE DIREECTEMENT AU FORMAT MSG

            //
            // ON TRANSFORME L'INFORMATION DE PARITE POUR ACCELERER LE COND_INV_SIGN (SSE)
            //
            sign = VECTOR_XOR(sign, misign8);

            for(int j=0 ; j<DEG_1 ; j++) {
                    TYPE vAbs  = VECTOR_MIN( VECTOR_ABS(tab_vContr[j]), max_msg );
                    TYPE vRes  = VECTOR_CMOV   (vAbs, min1, cste_1, cste_2);
                    TYPE vSig  = VECTOR_XOR    (sign, VECTOR_GET_SIGN_BIT(tab_vContr[j], msign8));
                    TYPE v2St  = VECTOR_invSIGN2(vRes, vSig);
                    TYPE v2Sr  = VECTOR_ADD_AND_SATURATE_VAR(tab_vContr[j], v2St, max_var, min_var);
                    VECTOR_STORE( p_msg1w,                      v2St);
                    VECTOR_STORE( &var_nodes[(*p_indice_nod2)], v2Sr);
                    p_msg1w       += 1;
                    p_indice_nod2 += 1;
            }
            
            arret = arret || VECTOR_XOR_REDUCE( sign );
        }

//////////////////////////
#if NB_DEGRES > 1
        for (int i=0; i<DEG_2_COMPUTATIONS; i++){
            
            TYPE tab_vContr[DEG_2];
            TYPE sign = zero;
            TYPE min1 = min_max_value;
            TYPE min2 = min_max_value;

            //
            // ON UTILISE UNE PETITE ASTUCE AFIN D'ACCELERER LA SIMULATION DU DECODEUR
            //
            for(int j=0 ; j<DEG_2 ; j++)
            {
                    TYPE vNoeud    = VECTOR_LOAD( &var_nodes[(*p_indice_nod1)] );
                    TYPE vMessg    = VECTOR_LOAD( p_msg1r );
                    tab_vContr[j]  = VECTOR_SUB_AND_SATURATE_VAR(vNoeud,vMessg,max_var,min_var);
                    p_indice_nod1 += 1;
                    p_msg1r       += 1;
                    TYPE cSign     = VECTOR_GET_SIGN_BIT(tab_vContr[j], msign8);
                    TYPE vAbs      = VECTOR_MIN( VECTOR_ABS(tab_vContr[j]), max_msg );
                    sign           = VECTOR_XOR (sign, cSign);
                    TYPE vTemp     = min1;
                    min1 = VECTOR_MIN_1(vAbs, min1      );
                    min2 = VECTOR_MIN_2(vAbs, vTemp, min2);
            }

            TYPE cste_1   = VECTOR_MIN( VECTOR_SBU(min2, _mm256_set1_epi8(offset)), max_msg); // ON SATURE DIREECTEMENT AU FORMAT MSG
            TYPE cste_2   = VECTOR_MIN( VECTOR_SBU(min1, _mm256_set1_epi8(offset)), max_msg); // ON SATURE DIREECTEMENT AU FORMAT MSG

            sign = VECTOR_XOR(sign, misign8);

            for(int j=0 ; j<DEG_2 ; j++) {
                    TYPE vAbs  = VECTOR_MIN( VECTOR_ABS(tab_vContr[j]), max_msg );
                    TYPE vRes  = VECTOR_CMOV   (vAbs, min1, cste_1, cste_2);
                    TYPE vSig  = VECTOR_XOR    (sign, VECTOR_GET_SIGN_BIT(tab_vContr[j], msign8));
                    TYPE v2St  = VECTOR_invSIGN2(vRes, vSig);
                    TYPE v2Sr  = VECTOR_ADD_AND_SATURATE_VAR(tab_vContr[j], v2St, max_var, min_var);
                    VECTOR_STORE( p_msg1w,                      v2St);
                    VECTOR_STORE( &var_nodes[(*p_indice_nod2)], v2Sr);
                    p_msg1w       += 1;
                    p_indice_nod2 += 1;
            }
            
            arret = arret || VECTOR_XOR_REDUCE( sign );
        }
#endif
#if NB_DEGRES > 2
    printf("The number of DEGREE(Cn) IS HIGHER THAN 2. YOU NEED TO PERFORM A COPY PASTE IN SOURCE CODE...\n");
    exit( 0 );
#endif
        //
        // GESTION DU CRITERE D'ARRET
        //
        if( (arret == 0) && (fast_stop == 1) ) break;
    }

    //
    // ON REMET EN FORME LES DONNEES DE SORTIE POUR LA SUITE DU PROCESS
    //
    if( NOEUD%32 == 0  ){
        uchar_itranspose_avx((TYPE*)var_nodes, (TYPE*)Rprime_fix, NOEUD);
    }else{
        char* ptr = (char*)var_nodes;
        for (int i=0; i<NOEUD; i+=1){
            for (int j=0; j<32; j+=1){
                Rprime_fix[j*NOEUD +i] = (ptr[32*i+j] > 0);
            }
        }
    }
    
    return (arret == 0);
}
