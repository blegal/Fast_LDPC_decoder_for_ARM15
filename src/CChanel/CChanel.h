#ifndef CLASS_CChanel
#define CLASS_CChanel

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "../CTrame/CTrame.h"

#define small_pi  3.1415926536
#define _2pi  (2.0 * small_pi)

class CChanel
{
    
protected:
    int  _vars;
    int  _checks;
    int  _data;
    int  _frames;
    int  BITS_LLR;
    int* data_in;
    int* data_out;
    bool qpsk;
    bool es_n0;

    bool normalize; // Normalize by 2/pow(sigma, 2)
    float norm_factor;
    
    float* t_noise_data;   // taille (width)
    int*   t_coded_bits;   // taille (width)
    
    double rendement;
    double SigB;
    double Gauss;
    double Ph;
    double Qu;
    double Eb_N0;
    double qbeta;
    double R;
    
public:
    CChanel(CTrame *t, int _BITS_LLR, bool QPSK, bool Es_N0);
    virtual ~CChanel();
    virtual void configure(double _Eb_N0) = 0;  // VIRTUELLE PURE
    virtual double get_R();
    virtual double get_SigB();
    virtual void setNormalize(bool enable);
    virtual void generate() = 0;                // VIRTUELLE PURE    
};

#endif

