#include "CBitGenerator.h"


CBitGenerator::CBitGenerator(CTrame *t, bool zero_only){
    _vars        = t->nb_vars();
    t_in_bits    = t->get_t_in_bits();
    _zero_mode   = zero_only;

    for(int i=0; i<_vars; i++){
        t_in_bits[i] = 0;
    }
}


void CBitGenerator::generate(){
    if( _zero_mode == false ){
        for(int i=0; i<_vars; i++){
            t_in_bits[i] = rand()%2;
        }
    }
}
