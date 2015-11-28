/*
 *  ldcp_decoder.h
 *  ldpc3
 *
 *  Created by legal on 02/04/11.
 *  Copyright 2011 ENSEIRB. All rights reserved.
 *
 */

/*----------------------------------------------------------------------------*/

#ifndef __CDecoder__
#define __CDecoder__

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

class CDecoder{
protected:
    float sigB;
    bool  fast_stop; // Use early termination criteria
    int   nb_iters;  // Use early termination criteria

public:
    CDecoder();
    virtual ~CDecoder();
    virtual void setSigmaChannel(float _sigB);
    virtual void setEarlyTerm(bool _early);
    virtual void setNumberOfIterations(int _value);
    virtual void decode(signed char  var_nodes[], signed char Rprime_fix[], int nombre_iterations) = 0;
    virtual void decode(float        var_nodes[], signed char Rprime_fix[], int nombre_iterations) = 0;
};

#endif
