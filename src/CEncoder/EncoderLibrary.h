

#ifndef CLASS_ENCODER_LIBRARY
#define CLASS_ENCODER_LIBRARY

#include "./CFakeEncoder.h"
#include "./GenericEncoder.h"

Encoder* EncoderLibrary(bool REAL_ENCODER, CTrame *trame)
{
    if( REAL_ENCODER == true ){
        return new GenericEncoder(trame);
    }else{
        return new CFakeEncoder(trame);
    }
}

#endif

