#include "CFakeEncoder.h"

CFakeEncoder::CFakeEncoder(CTrame *t) : Encoder(t)
{
    int i_size = _frames * _size_in;
    int o_size = _frames * _size_out;
    int* p = data_in;
    while(i_size--) (*p++) = 0;
    int* q = data_out;
    while(o_size--) (*q++) = 0;
}
    
void CFakeEncoder::encode()
{

}
