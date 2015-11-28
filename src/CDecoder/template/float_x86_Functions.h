/*
 *  ldcp_decoder.h
 *  ldpc3
 *
 *  Created by legal on 02/04/11.
 *  Copyright 2011 ENSEIRB. All rights reserved.
 *
 */

/*----------------------------------------------------------------------------*/

//#include "../shared/genere_par_java_simu.h"

#ifndef __x86_Functions__
#define __x86_Functions__

#define llr_from_input(v)  ((2.0 * v)/(sigB * sigB))

static inline int Signe_de_contrib(float entree){
    return ((entree<0.0)?0:1);
}

static inline float valeur_absolue(float entree){
    double sortie;
    if (entree < 0.0)
        sortie = -entree;
    else
        sortie = entree;
    return(sortie);
}

static inline float InversionCondSign(int signe, float value){
    if (signe == 1){
        return value;
    }
    return -value;
}

#endif
