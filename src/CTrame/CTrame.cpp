#include "CTrame.h"


CTrame::CTrame(int width, int height){
    _width        = width;
    _height       = height;
    _frame        = 1;
    t_in_bits     = new int[ nb_frames() * nb_vars() ];
    t_coded_bits  = new int[ nb_frames() * nb_data() ];
    t_noise_data  = new float[ nb_frames() * nb_data() + 4 ];
    t_fpoint_data = new signed char[ nb_frames() * nb_data() ];
    t_decode_data = new signed char[ nb_frames() * nb_data() ];
    t_decode_bits = new signed char[ nb_frames() * nb_vars() ];
}

CTrame::CTrame(int width, int height, int frame){
    _width        = width;
    _height       = height;
    _frame        = frame;
    t_in_bits     = new int[ nb_frames() *nb_vars() ];
    t_coded_bits  = new int[ nb_frames() *nb_data() ];
    t_noise_data  = new float[ nb_frames() *nb_data() + 4];
    t_fpoint_data = new signed char[ nb_frames() *nb_data() ];
    t_decode_data = new signed char[ nb_frames() *nb_data() ];
    t_decode_bits = new signed char[ nb_frames() *nb_vars() ];
}

CTrame::~CTrame(){
    delete t_in_bits;
    delete t_coded_bits;
    delete t_noise_data;
    delete t_fpoint_data;
    delete t_decode_data;
    delete t_decode_bits;
}

int CTrame::nb_vars(){
    return  /*nb_frames() * */(nb_data()-nb_checks());
}

int CTrame::nb_frames(){
    return  _frame;
}

int CTrame::nb_checks(){
    return _height;
}

int CTrame::nb_data(){
    return _width;
}

int* CTrame::get_t_in_bits(){
    return t_in_bits;
}

int* CTrame::get_t_coded_bits(){
    return t_coded_bits;
}

float* CTrame::get_t_noise_data(){
    return t_noise_data;
}

signed char* CTrame::get_t_fpoint_data(){
    return t_fpoint_data;
}

signed char* CTrame::get_t_decode_data(){
    return t_decode_data;
}

signed char* CTrame::get_t_decode_bits(){
    return t_decode_bits;
}
