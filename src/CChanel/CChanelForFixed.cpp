/**
  Copyright (c) 2012-2015 "Bordeaux INP, Bertrand LE GAL"
  [bertrand.legal@ims-bordeaux.fr     ]
  [http://legal.vvv.enseirb-matmeca.fr]

  This file is part of Fast_LDPC_C_decoder_for_ARM15.

  Fast_LDPC_C_decoder_for_ARM15 is free software: you can redistribute it and/or modify

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

#include "CChanelForFixed.h"

double CChanelForFixed::inv_erf(int v){
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


void CChanelForFixed::SigmaDuBruit(){
    double interm;
//    int pari  = _data - _vars;
    rendement = (float)(_vars)/(float)(_data);
    interm    = 10.0 * log10(rendement);
    interm    = -0.1*((double)Eb_N0+interm);
    SigB      =  sqrt(pow(10.0,interm)/2);
    qbeta     = SigB * sqrt(2.0) * inv_erf( BITS_LLR - 1 ); // PATCH CEDRIC MARCHAND
    R         = (1.0 + qbeta);
//    printf("(DD) CChannel : data=%d, var=%d, par=%d, r=%f, Eb/N0=%f, SigmaB=%f,qbeta=%f, R=%f\n", _data, _vars, pari, rendement, Eb_N0, SigB, qbeta, R);
        if( qpsk == true ){
    SigB      = sqrt(2.0*rendement*pow(10.0,(Eb_N0/10.0)));
        }
}

#include <limits.h>
#define MAX_RANDOM LONG_MAX    /* Maximum value of random() */


double awgn(double amp)
{
  double u1,u2,s,noise /*,randmum*/;
    //printf("Start looping\n");
  do {
        double rand1 = (double)random();
        double rand2 = (double)random();
        u1 = (rand1/2147483647) * 2.0 - 1.0;
        u2 = (rand2/2147483647) * 2.0 - 1.0;
        s  = (u1 * u1) + (u2 * u2);
    //printf("(%f, %f) - %d - (%f, %f, %f)\n", rand1, rand2, MAX_RANDOM, u1, u2, s);
    } while( s >= 1);
    //printf("Stop looping\n");
    noise = (u1 * sqrt( (-2.0 * log(s))/s )) / (amp);               /* BPSK */
//    noise = (u1 * sqrt( (-2.0 * log(s))/s )) / (amp * sqrt(2.0)); /* QPSK */
  return(noise);
}

double CChanelForFixed::get_R(){
    return R;
}


void CChanelForFixed::BruitGaussien()
{
    double x  = sqrt(-2.0*log((double)rand()/(double)2147483647));
    double y  = (double)rand()/(double)2147483647;
    Ph = x * sin(_2pi*y);
    Qu = x * cos(_2pi*y);
}


CChanelForFixed::CChanelForFixed(CTrame *t, int _BITS_LLR, bool QPSK){
    qbeta        = 0.0;
    R            = 0.0;
    _vars        = t->nb_vars();
    _data        = t->nb_data();
    _checks      = t->nb_checks();
    t_coded_bits = t->get_t_coded_bits();
    t_noise_data = t->get_t_noise_data();
    BITS_LLR     = _BITS_LLR;
    qpsk         = QPSK;
}


void CChanelForFixed::configure(double _Eb_N0){
    Eb_N0 = _Eb_N0;
    //printf("(DD) CChannel : Eb/N0 = %f\n", Eb_N0);
    SigmaDuBruit();
}

#define QPSK 0.707106781
#define BPSK 1.0

void CChanelForFixed::generate()
{
    double pv =  BPSK; // ON CHOISIT LE TYPE DE CODAGE DU SIGNAL
    double mv = -BPSK; // BPSK OU QPSK (CODAGES LES + SIMPLES)

if( qpsk == false ){
    double x,y;
//    double facteur_echelle = 1.0;
//    int i, /*signe,compt=0*/;
    for (int i=0;i<(_data+1)/2;i++){
        BruitGaussien();
        x = ( t_coded_bits[2*i] == 1 ? pv : mv ) + (SigB * Ph);
        t_noise_data[2*i]= x;
        if( ((2*i)+1) < _data ){
            y = ( t_coded_bits[2*i+1] == 1 ? pv : mv ) + (SigB * Qu);
            t_noise_data[(2*i)+1]= y;
        }
    }
}else{
    double x,y;
//    double facteur_echelle = 1.0;
//    int i, signe,compt=0;
    for (int i=0;i<(_data+1)/2;i++){
        x = ( t_coded_bits[2*i] == 1 ? pv : mv ) + awgn(SigB);//(SigB * Ph);
        t_noise_data[2*i]= x;
        if( ((2*i)+1) < _data ){
            y = ( t_coded_bits[2*i+1] == 1 ? pv : mv ) + awgn(SigB);//(SigB * Qu);
            t_noise_data[(2*i)+1]= y;
        }
    }

}

}
