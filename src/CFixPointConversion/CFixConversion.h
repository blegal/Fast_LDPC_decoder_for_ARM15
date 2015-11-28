#ifndef CLASS_CFixConversion
#define CLASS_CFixConversion

#include <stdlib.h>
#include <stdio.h>
#include "../CTrame/CTrame.h"

class CFixConversion
{
    
protected:
    int          _data;
    int          _frames;
    float*       t_noise_data;      // taille (var)
    signed char* t_fpoint_data;     // taille (width)

    bool dump_q;
    long long histo[256];

public:
    CFixConversion(CTrame *t);
    virtual ~CFixConversion();
    virtual void generate();
    void ShowHistoOnDestroy( bool enable );
};

#endif

