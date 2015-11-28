#include "GenericEncoder.h"
#include "GenericEncoderTable.h"

GenericEncoder::GenericEncoder(CTrame *t) : Encoder(t)
{
    assert( _size_in  == (K_LDPC) );
    assert( _size_out == (N_LDPC) );
}

GenericEncoder::~GenericEncoder()
{

}

//
// METHODE PERMETTANT D'ENCODER LES TRAINS DE BITS AU FORMAT DVB-S2 (r=9/10)
//
void GenericEncoder::encode()
{    
    for(int y=0; y<_size_out; y++)
    {
        data_out[y] = 0;
    }

    for(int y=0; y<_size_in; y++)
    {
        int bit     = rand()%2;
        data_in [y] = bit;
        data_out[y] = bit;
    }

    int *Px  = &data_out[K_LDPC];
    int *p   = EncValues;
    int xPos = 0;

    for(int y=0; y<N_LINES; y++)
    {
        int nbPos = (*p++);

        for(int l=0; l<M_LDPC; l++){

            int bit   = data_in[xPos];
            if( bit == 1 )
            {
                for(int q=0; q<nbPos; q++){
                    int position = (p[q] + (xPos % 360) * Q_LDPC) % NmK_LDPC;
                    Px[position] ^= bit;
                }
            }
            xPos += 1;
        }
        p += nbPos;            
    }

    for(int i=1; i<NmK_LDPC; i++){
        Px[i] = Px[i] ^ Px[i-1];
    }
}

