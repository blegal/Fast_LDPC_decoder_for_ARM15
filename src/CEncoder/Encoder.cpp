#include "Encoder.h"

Encoder::Encoder(CTrame *t)
{
    _size_in   = t->nb_vars();
    _size_out  = t->nb_data();
    data_in    = t->get_t_in_bits();
    data_out   = t->get_t_coded_bits();
    _frames    = t->nb_frames();
}

Encoder::~Encoder()
{
}

int Encoder::size_in()
{
    return _size_in;
}


int Encoder::size_out()
{
    return _size_out;
}


void Encoder::encode()
{
    for(int y=0; y<_size_out; y++)
    {
        data_out[y] = 0;
    }
}


void Encoder::sum_bits(){
    int sum = 0;
    for(int y=0; y<_size_out; y++)
    {
        sum += data_out[y];
    }
    printf("SOMME BITS = %d\n", sum);
}


void Encoder::sum_pos(){
    int sum = 0;
    for(int y=0; y<_size_out; y++)
    {
        sum += (data_out[y]!=0)?(y+1):0;
    }
    printf("SOMME POSI = %d\n", sum);
}
