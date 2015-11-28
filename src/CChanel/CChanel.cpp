#include "CChanel.h"

double CChanel::get_R(){
    return R;
}

double CChanel::get_SigB(){
    return SigB;
}

CChanel::~CChanel(){
}


CChanel::CChanel(CTrame *t, int _BITS_LLR, bool QPSK, bool ES_N0){
    qbeta        = 0.0;
    R            = 0.0;
    _vars        = t->nb_vars();
    _data        = t->nb_data();
    _checks      = t->nb_checks();
    t_coded_bits = t->get_t_coded_bits();
    t_noise_data = t->get_t_noise_data();
    _frames      = t->nb_frames();
    BITS_LLR     = _BITS_LLR;
    qpsk         = QPSK;
    es_n0        = ES_N0;
    normalize    = false;
    norm_factor  = 0.0f;
}

void CChanel::setNormalize(bool enable){
    normalize = enable;
}
