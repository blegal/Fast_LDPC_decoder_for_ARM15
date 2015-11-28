

#ifndef CLASS_ENCODER_GENERIC
#define CLASS_ENCODER_GENERIC

#define xorf(a,b) (a^b)

#include "../CTrame/CTrame.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

class Encoder
{

protected:
    int  _size_in;
    int  _size_out;
    int  _frames;
    int* data_in;
    int* data_out;

public:
    Encoder(CTrame *t);
    virtual ~Encoder();

    virtual int size_in();
    virtual int size_out();
    virtual void encode();
    virtual void sum_bits();
    virtual void sum_pos();
};

#endif
