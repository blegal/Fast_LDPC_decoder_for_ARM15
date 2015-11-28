/*
 *  ldcp_decoder.h
 *  ldpc3
 *
 *  Created by legal on 02/04/11.
 *  Copyright 2011 ENSEIRB. All rights reserved.
 *
 */

/*----------------------------------------------------------------------------*/

#include "CDecoder_OMS_fixed_x86.h"

#define Signe_de_contrib(a) ((a<=0)?0:1)
#define f_abs_fix(a)        ((a<0)?-a:a)
#define i32b_fix_CondInvSign(a,b) ((a == 1)?b:-b)

#define MAX(a,b) ((a>b)?a:b)
#define MIN(a,b) ((a<b)?a:b)
#define SATURATE(a,b,c)           (MIN(MAX(a,b),c))
#define i_mesg_Saturate(a)        SATURATE(a,vSAT_NEG_MSG,vSAT_POS_MSG)
#define i_contrib_sym_Saturate(a) SATURATE(a,vSAT_NEG_VAR,vSAT_POS_VAR)

static inline void i32b_fix_min_update(int input, int *i32_min, int *i32_min2) {
    if (input < *i32_min) {
        *i32_min2 = *i32_min;
        *i32_min = input;
    } else if (input < *i32_min2) {
        *i32_min2 = input;
        *i32_min = *i32_min;
    }
}

CDecoder_OMS_fixed_x86::CDecoder_OMS_fixed_x86() {
    offset = -1;
}

CDecoder_OMS_fixed_x86::~CDecoder_OMS_fixed_x86() {
}

void CDecoder_OMS_fixed_x86::setOffset(int _offset) {
    if( offset == -1 ){
        offset = _offset;
    }else{
        printf("(EE) Offset value was already configured !\n");
        exit( 0 );
    }
}


void CDecoder_OMS_fixed_x86::decode(signed char Intrinsic_fix[], signed char Rprime_fix[], int nombre_iterations) {
    // Initilisation des espaces memoire
    for (int i = 0; i < MESSAGE; i++) {
        var_mesgs[i] = 0;
    }

    // ON RECOPIE LA VALEUR DES INTRINSEQUES (VALEURS DES NOEUDS VARIABLES)
    for (int i = 0; i < NOEUD; i++) {
        var_nodes[i] = Intrinsic_fix[i];
    }

    while (nombre_iterations--) {
        short *p_msg1r = var_mesgs;
        short *p_msg1w = var_mesgs;
        const unsigned short *p_indice_nod1 = PosNoeudsVariable;
        const unsigned short *p_indice_nod2 = PosNoeudsVariable;

        for (int i = 0; i < DEG_1_COMPUTATIONS; i++) {
            int sign_somme = 0;
            int i32_min  = vSAT_POS_VAR + 1;
            int i32_min2 = vSAT_POS_VAR + 1;

            //
            // ON UTILISE UNE PETITE ASTUCE AFIN D'ACCELERER LA SIMULATION DU DECODEUR
            //
            int tab_vContr[DEG_1];
            for (int j = 0; j < DEG_1; j++) {
                int vNoeud    = var_nodes[ (*p_indice_nod1++) ];
                int vMessg    = (*p_msg1r++);
                int vContr    = i_contrib_sym_Saturate(vNoeud - vMessg);
                tab_vContr[j] = vContr;
                i32b_fix_min_update(f_abs_fix(i_mesg_Saturate(vContr)), &i32_min, &i32_min2);
                sign_somme    = sign_somme ^ Signe_de_contrib(vContr);
            }

            int cste_1 = (i32_min2 - offset); cste_1 = (cste_1 < 0) ? 0 : cste_1;
            int cste_2 = (i32_min  - offset); cste_2 = (cste_2 < 0) ? 0 : cste_2;

            for (int j = 0; j < DEG_1; j++) {
                int vContr    = tab_vContr[j];
                int vResultat = (f_abs_fix(i_mesg_Saturate(vContr)) == i32_min) ? cste_1 : cste_2;
                int vSigne    = sign_somme ^ Signe_de_contrib(vContr);
                vResultat     = i32b_fix_CondInvSign(vSigne, vResultat);
                vResultat     = i_mesg_Saturate(vResultat);
                (*p_msg1w++)  = vResultat;
                var_nodes[ (*p_indice_nod2++) ] = i_contrib_sym_Saturate(vContr + vResultat);
            }
        }

#if NB_DEGRES > 1
        for (int i = 0; i < DEG_2_COMPUTATIONS; i++) {
            int sign_somme = 0;
            int i32_min  = vSAT_POS_VAR + 1;
            int i32_min2 = vSAT_POS_VAR + 1;

            //
            // ON UTILISE UNE PETITE ASTUCE AFIN D'ACCELERER LA SIMULATION DU DECODEUR
            //
            int tab_vContr[DEG_2];
            for (int j = 0; j < DEG_2; j++) {
                int vNoeud    = var_nodes[ (*p_indice_nod1++) ];
                int vMessg    = (*p_msg1r++);
                int vContr    = i_contrib_sym_Saturate(vNoeud - vMessg);
                tab_vContr[j] = vContr;
                i32b_fix_min_update(f_abs_fix(i_mesg_Saturate(vContr)), &i32_min, &i32_min2);
                sign_somme    = sign_somme ^ Signe_de_contrib(vContr);
            }

            int cste_1 = (i32_min2 - offset); cste_1 = (cste_1 < 0) ? 0 : cste_1;
            int cste_2 = (i32_min  - offset); cste_2 = (cste_2 < 0) ? 0 : cste_2;

            for (int j = 0; j < DEG_2; j++) {
                int vContr    = tab_vContr[j];
                int vResultat = (f_abs_fix(i_mesg_Saturate(vContr)) == i32_min) ? cste_1 : cste_2;
                int vSigne    = sign_somme ^ Signe_de_contrib(vContr);
                vResultat     = i32b_fix_CondInvSign(vSigne, vResultat);
                vResultat     = i_mesg_Saturate(vResultat);
                (*p_msg1w++)  = vResultat;
                var_nodes[ (*p_indice_nod2++) ] = i_contrib_sym_Saturate(vContr + vResultat);
            }
        }
#endif
#if NB_DEGRES > 2
    printf("The number of DEGREE(Cn) IS HIGHER THAN 2. YOU NEED TO PERFORM A COPY PASTE IN SOURCE CODE...\n");
    exit( 0 );
#endif
        //
        // GESTION PROPRE DU CRITERE D'ARRET
        //
        if(fast_stop == 1){
            p_indice_nod1 = PosNoeudsVariable;
            p_msg1r       = var_mesgs;
            unsigned int arret = 0;

            for (int i = 0; i < DEG_1_COMPUTATIONS; i++) {
                unsigned int stop_criterion = 0;
                for (int j = 0; j < DEG_1; j++) {
                    int vNoeud    = var_nodes[ (*p_indice_nod1++) ];
                    int vMessg    = (*p_msg1r++);
                    int vContr    = i_contrib_sym_Saturate(vNoeud - vMessg);
                    stop_criterion = stop_criterion ^ Signe_de_contrib( vContr );
                }
                arret = arret || stop_criterion;
            }
            
#if NB_DEGRES > 1
            for (int i = 0; i < DEG_2_COMPUTATIONS; i++) {
                unsigned int stop_criterion = 0;
                for (int j = 0; j < DEG_2; j++) {
                    int vNoeud    = var_nodes[ (*p_indice_nod1++) ];
                    int vMessg    = (*p_msg1r++);
                    int vContr    = i_contrib_sym_Saturate(vNoeud - vMessg);
                    stop_criterion = stop_criterion ^ Signe_de_contrib( vContr );
                }
                arret = arret || stop_criterion;
            }
#endif

            //
            // THIS IS A SANITY CHECK
            //
            if ( arret == 0 ){
                int sum = 0;
                for (int i = 0; i < _N; i++) {
                    sum += (var_nodes[i] > 0);
                }
                if( sum ){
                    printf("(EE) Error in stop criterion (%d)\n", sum);
                }
                break;
            }
        }

    }
    // FIN DU PROCESS ITERATIF DE DECODAGE

    for (int i = 0; i < _N; i++) {
        Rprime_fix[i] = (var_nodes[i] > 0);
    }
}
