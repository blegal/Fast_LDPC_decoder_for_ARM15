#ifndef CLASS_CFastFixConversion
#define CLASS_CFastFixConversion

#include <stdlib.h>
#include <stdio.h>
#include "CFixConversion.h"

class CFastFixConversion : public CFixConversion
{
    
protected:
//    int  _data;
//    double*  t_noise_data;      // taille (var)
//    int*  t_fpoint_data;   // taille (width)
    signed int FACTEUR_BETA;
    signed int vSAT_NEG_LLR;
    signed int vSAT_POS_LLR;

public:
    
    CFastFixConversion(CTrame *t, int _FACTEUR_BETA, int _vSAT_NEG_LLR, int _vSAT_POS_LLR);
    ~CFastFixConversion();
    virtual void generate();
};

#endif

