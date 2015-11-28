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

#include "CDecoder_fixed_SSE.h"

CDecoder_fixed_SSE::CDecoder_fixed_SSE()
{
    var_nodes = new int8x16_t[NOEUD];
    var_mesgs = new int8x16_t[MESSAGE];
}

CDecoder_fixed_SSE::~CDecoder_fixed_SSE()
{
    delete var_nodes;
    delete var_mesgs;
}

void CDecoder_fixed_SSE::decode(float var_nodes[], signed char Rprime_fix[], int nombre_iterations)
{
	if( nombre_iterations < 0 ){
		printf("%p\n", var_nodes);
		printf("%p\n", Rprime_fix);
	}
    // ON NE FAIT RIEN !
    // CETTE METHODE ASSURE JUSTE LA COMPATIBILITE ENTRE LES CLASSES MANIPULANT
    // DES DONNEES FLOTTANTES ET CELLES MANIPULANT DES DONNEES EN VIRGULE FIXE.
}
