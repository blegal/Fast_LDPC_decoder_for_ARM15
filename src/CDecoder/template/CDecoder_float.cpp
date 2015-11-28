/*
 *  ldcp_decoder.h
 *  ldpc3
 *
 *  Created by legal on 02/04/11.
 *  Copyright 2011 ENSEIRB. All rights reserved.
 *
 */

/*----------------------------------------------------------------------------*/

#include "./CDecoder_float.h"

CDecoder_float::CDecoder_float()
{
    var_mesgs   = new float[ONE_COUNT];    
}

CDecoder_float::~CDecoder_float()
{
    delete var_mesgs;
}

void CDecoder_float::decode(char var_nodes[], char Rprime_fix[], int nombre_iterations)
{
    // METHODE DECLAREE UNIQUEMENT AFIN DE MAINTENIR UNE COMPATIBILITE ENTRE
    // LES CLASSES MANIPULANT DES DONNEES FLOTTANTES ET CELLES MANIPULANT DES
    // DONNEES EN VIRGULE FIXE.
}
