#ifndef CLASS_GenericEncoder
#define CLASS_GenericEncoder

#include "Encoder.h"

class GenericEncoder : public Encoder
{
public:
    
    GenericEncoder(CTrame *t);
    virtual ~GenericEncoder();

    virtual void encode();
};

#endif
