/*
 *  ldcp_decoder.h
 *  ldpc3
 *
 *  Created by legal on 02/04/11.
 *  Copyright 2011 ENSEIRB. All rights reserved.
 *
 */

/*----------------------------------------------------------------------------*/

#ifndef __CFakeDecoder__
#define __CFakeDecoder__

class CFakeDecoder{

public:
    CFakeDecoder();
    CFakeDecoder(int data);
    virtual void decode(double var_nodes[N], int Rprime_fix[N], int nombre_iterations);
};


CFakeDecoder::CFakeDecoder()
{
}

void CFakeDecoder::decode(double var_nodes[N], int Rprime_fix[N], int nombre_iterations)
{
    int i;
    for (i=0;i<N;i++){
    	Rprime_fix[i] = (var_nodes[i] > 0.0);
    }
}

#endif;
