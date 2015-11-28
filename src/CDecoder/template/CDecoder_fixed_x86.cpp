/*
 *  ldcp_decoder.h
 *  ldpc3
 *
 *  Created by legal on 02/04/11.
 *  Copyright 2011 ENSEIRB. All rights reserved.
 *
 */

/*----------------------------------------------------------------------------*/

#include "./CDecoder_fixed_x86.h"

#include "./Constantes/constantes_sse.h"

CDecoder_fixed_x86::CDecoder_fixed_x86()
{
    var_nodes   = new short[NOEUD];    
    var_mesgs   = new short[MESSAGE];
}

CDecoder_fixed_x86::~CDecoder_fixed_x86()
{
    delete var_nodes;
    delete var_mesgs;
}

void CDecoder_fixed_x86::decode(float var_nodes[], signed char Rprime_fix[], int nombre_iterations)
{
	if( nombre_iterations < 0 ){
		printf("%p\n", var_nodes);
		printf("%p\n", Rprime_fix);
	}
    // ON NE FAIT RIEN !
    // CETTE METHODE ASSURE JUSTE LA COMPATIBILITE ENTRE LES CLASSES MANIPULANT
    // DES DONNEES FLOTTANTES ET CELLES MANIPULANT DES DONNEES EN VIRGULE FIXE.
}
