/**
  Copyright (c) 2012-2015 "Bordeaux INP, Bertrand LE GAL"
  [http://legal.vvv.enseirb-matmeca.fr]
  This file is part of LDPC_C_Simulator.
  LDPC_C_Simulator is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "CChanelAWGN_x86.h"

#define MODE_SSE_AVX 0

double CChanelAWGN_x86::inv_erf(int v){
    if (v == 3) {
        return 0.86312;
    }else if(v == 4){
        return 1.1064;
    }else if(v == 5){
        return 1.3268;
    }else if(v == 6){
        return 1.5274;
    }else if(v == 7){
        return 1.7115;
    }else if(v == 8){
        return 1.8819;
    }else if(v == 9){
        return 2.0409;
    }else if(v == 10){
        return 2.1903;
    }
    return -1;
}

void CChanelAWGN_x86::BruitGaussien()
{
        double x  = sqrt(-2.0*log((double)rand()/(double)2147483647));
        double y  = (double)rand()/(double)2147483647;
        Ph = x * sin(_2pi * y);
        Qu = x * cos(_2pi * y);
        if( qpsk == true ){
            Ph /= sqrt(2.0);   /* QPSK */
            Qu /= sqrt(2.0);   /* QPSK */
        }
}

CChanelAWGN_x86::CChanelAWGN_x86(CTrame *t, int _BITS_LLR, bool QPSK, bool Es_N0)
    : CChanel(t, _BITS_LLR, QPSK, Es_N0){

}

CChanelAWGN_x86::~CChanelAWGN_x86(){

}

void CChanelAWGN_x86::configure(double _Eb_N0){

    rendement = (float) (_vars) / (float) (_data);
    if (es_n0) {
        // ES/N0 = Eb/N0 + 10*log10(R*m)
        // o√π R  = rendement
        // m     = nombre de bits par symbole de constellation (QPSK => 2)
        // Eb/N0 et ES/N0 sont en dB
        Eb_N0 = _Eb_N0 - 10.0 * log10(2 * rendement);
    } else {
        Eb_N0 = _Eb_N0;
    }

    double interm = 10.0 * log10(rendement);
    interm        = -0.1*((double)Eb_N0+interm);
    SigB          = sqrt(pow(10.0,interm)/2);
    qbeta         = SigB * sqrt(2.0) * inv_erf( BITS_LLR - 1 ); // PATCH CEDRIC MARCHAND
    R             = (1.0 + qbeta);

    //
    // FACTEUR DE NORMALISATION DU CANAL PROVENENT DU DECODEUR DE CODE POLAIRE
    // A CAMILLE.
    //
    if( normalize == true ){
        norm_factor  = 2.0 / (SigB * SigB);
    }else{
        norm_factor  = 1.0;
    }
}

#define QPSK 0.707106781
#define BPSK 1.0

void CChanelAWGN_x86::generate()
{
    double pv = (qpsk) ?  QPSK :  BPSK; // ON CHOISIT LE TYPE DE CODAGE DU SIGNAL
    double mv = (qpsk) ? -QPSK : -BPSK; // BPSK OU QPSK (CODAGES LES + SIMPLES)
    double x,y;

    for (int z = 0; z < _frames; z++) {
        int offset = z * _data;
        for (int i = 0; i < (_data + 1) / 2; i++) {
            BruitGaussien();
            x = (t_coded_bits[offset + 2 * i] == 1 ? pv : mv) + (SigB * Ph);
            t_noise_data[offset + 2 * i] = x * norm_factor;
            if (((2 * i) + 1) < _data) {
                y = (t_coded_bits[offset + 2 * i + 1] == 1 ? pv : mv) + (SigB * Qu);
                t_noise_data[offset + 2 * i + 1] = y * norm_factor;
            }
        }
    }
/*
    for(int z=0; z<_frames; z++){
        int offset = z * _data;
        for (int i=0; i<_data; i++){
            float val = (t_noise_data[offset + i]);// * (2.0f / (SigB*SigB) );
            min  = (min < val) ? min : val;
            max  = (max > val) ? max : val;
            moy += val;
            soy += val * val;
        }
    }
    bits += _frames * _data;
    printf("sigma = %f, min = %f, max = %f, moy = %f, soy = %f,, RAND_MAX = %0x%8.8X\n", SigB, min, max, moy/(bits), sqrt(soy/(bits)), RAND_MAX);
*/
}
