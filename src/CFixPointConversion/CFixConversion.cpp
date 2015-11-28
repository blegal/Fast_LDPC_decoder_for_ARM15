#include "CFixConversion.h"


CFixConversion::CFixConversion(CTrame *t){
    _data         = t->nb_data();
    _frames       = t->nb_frames();
    t_noise_data  = t->get_t_noise_data();
    t_fpoint_data = (signed char*)t->get_t_fpoint_data();

    //
    // POUR POUVOIR AFFICHER L'HISTO POST-CONVERSION
    //
    dump_q = false;
    for(int i=0; i<256; i++){
        histo[i] = 0;
    }
}

CFixConversion::~CFixConversion(){

}

void CFixConversion::generate(){
    printf("(WW) CFixConversion : execution of the generate function !\n");
    exit( 0 );
}

void CFixConversion::ShowHistoOnDestroy( bool enable ){
    dump_q = enable;
}
