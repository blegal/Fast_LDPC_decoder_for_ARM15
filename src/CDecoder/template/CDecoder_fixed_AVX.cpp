/*
 *  ldcp_decoder.h
 *  ldpc3
 *
 *  Created by legal on 02/04/11.
 *  Copyright 2011 ENSEIRB. All rights reserved.
 *
 */

/*----------------------------------------------------------------------------*/

#ifdef __AVX2__
#include "CDecoder_fixed_AVX.h"

CDecoder_fixed_AVX::CDecoder_fixed_AVX()
{
    var_nodes = new __m256i[NOEUD];
    var_mesgs = new __m256i[MESSAGE];
}

CDecoder_fixed_AVX::~CDecoder_fixed_AVX()
{
    delete var_nodes;
    delete var_mesgs;
}

void CDecoder_fixed_AVX::decode(float var_nodes[], char Rprime_fix[], int nombre_iterations)
{
    // ON NE FAIT RIEN !
    // CETTE METHODE ASSURE JUSTE LA COMPATIBILITE ENTRE LES CLASSES MANIPULANT
    // DES DONNEES FLOTTANTES ET CELLES MANIPULANT DES DONNEES EN VIRGULE FIXE.
}
#endif
