#ifndef CLASS_CChanelAWGN_x86
#define CLASS_CChanelAWGN_x86

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "CChanel.h"

#define small_pi  3.1415926536
#define _2pi  (2.0 * small_pi)

class CChanelAWGN_x86 : public CChanel
{
private:
    void BruitGaussien();
    double inv_erf(int v);
    float* tabPh;
    float* tabQu;

public:
    CChanelAWGN_x86(CTrame *t, int _BITS_LLR, bool QPSK, bool Es_N0);
    ~CChanelAWGN_x86();
    
    virtual void configure(double _Eb_N0);
    virtual void generate();
};

#endif

