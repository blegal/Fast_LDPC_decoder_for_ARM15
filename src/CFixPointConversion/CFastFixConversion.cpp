#include "CFastFixConversion.h"


CFastFixConversion::CFastFixConversion(CTrame *t, int _FACTEUR_BETA, int _vSAT_NEG_LLR, int _vSAT_POS_LLR) : CFixConversion(t) {
    FACTEUR_BETA  = _FACTEUR_BETA;
    vSAT_NEG_LLR  = _vSAT_NEG_LLR;
    vSAT_POS_LLR  = _vSAT_POS_LLR;
//    printf("Borne mini = %d\n", vSAT_NEG_LLR);
//    printf("Borne maxi = %d\n", vSAT_POS_LLR);
}

CFastFixConversion::~CFastFixConversion(){
    if( dump_q ){
        long long sum = 0;
        for(int i=0; i<256; i++){
            if( ((i-128) <= (vSAT_POS_LLR)) && ((i-128) >= (vSAT_NEG_LLR)) ){
                sum += histo[i];
            }
        }
        printf("(HISTO) START\n");
        for(int i=0; i<256; i++){
            if( ((i-128) <= (vSAT_POS_LLR+1)) && ((i-128) >= (vSAT_NEG_LLR-1)) ){
                double proba = 100.0 * ((double)histo[i]) / (double)sum;
                printf("(HISTO) %3d\t%f\n", i-128, proba);
            }
        }
        printf("(HISTO) STOP\n");
    }
}


#define OPTIMIZATION_SSE 0

void CFastFixConversion::generate(){
#if OPTIMIZATION_SSE == 0
    for(int z = 0; z<_frames; z++){
        int offset = z * _data;
	    for(int i=0; i<_data; i++){
	        signed int value = (FACTEUR_BETA * t_noise_data[offset+i]);  // ON TRANFORME LES FLOTTANT EN FIXED POINT
	        value = (value>vSAT_NEG_LLR) ? value : vSAT_NEG_LLR;  // ON GERE LA SATURATION DES DONNEES DANS LE
	        value = (value<vSAT_POS_LLR) ? value : vSAT_POS_LLR;  // FORMAT DEDIE AU LLR
	        t_fpoint_data[offset+i] = value;
//			printf("%f * %d = %d | %d | %d\n", t_noise_data[offset+i], FACTEUR_BETA, t_fpoint_data[offset+i], vSAT_NEG_LLR, vSAT_POS_LLR);
	    }
	}
#else    
    int nb_data = _frames * _data;
    // PEUT'ON FONCTIONNER EN MODE SSE ?
    if( (nb_data & 0x03) == 0 ){

        __m128i mini  = _mm_set1_epi32(vSAT_NEG_LLR);
        __m128i maxi  = _mm_set1_epi32(vSAT_POS_LLR);
        __m128  fact  = _mm_set1_ps(FACTEUR_BETA);
        __m128*  tabI = (__m128* ) t_noise_data;
        __m128i* tabO = (__m128i*) t_fpoint_data;
        int loop = nb_data / 4;

        for(int z=0; z<loop; z++){
            __m128  iData = _mm_mul_ps( tabI[z], fact );
            __m128i resul = _mm_cvttps_epi32(iData);
            __m128i satu1 = _mm_max_epi32  ( resul, mini );
            __m128i satu2 = _mm_min_epi32  ( satu1, maxi );
            tabO[z] = satu2;
        }
    }
    // SINON ON LANCE LE CALCUL EN MODE NORMAL
    else {
        for(int z = 0; z<nb_data; z++){
            int value = (FACTEUR_BETA * t_noise_data[z]);         // ON TRANFORME LES FLOTTANT EN FIXED POINT
            value = (value>vSAT_NEG_LLR) ? value : vSAT_NEG_LLR;  // ON GERE LA SATURATION DES DONNEES DANS LE
            value = (value<vSAT_POS_LLR) ? value : vSAT_POS_LLR;  // FORMAT DEDIE AU LLR
            t_fpoint_data[z] = value;
        }
    }
#endif
    if( dump_q ){
        signed char* ptr    = (signed char*)t_fpoint_data;
        unsigned int length = _frames * _data;
        while(length--) {
            histo[ ((int)(*ptr++)) + 128 ] += 1;
        }
    }
}
