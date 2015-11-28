#ifndef CLASS_CFakeEncoder
#define CLASS_CFakeEncoder

#include <stdlib.h>
#include <stdio.h>
#include "Encoder.h"

class CFakeEncoder : public Encoder
{
public:

    CFakeEncoder(CTrame *t);

    virtual void encode();
};

#endif
