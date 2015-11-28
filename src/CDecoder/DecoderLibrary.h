/* 
 * File:   DecoderLibrary.h
 * Author: legal
 *
 * Created on 23 janvier 2013, 01:02
 */

/* 
 * File:   DecoderLibrary.h
 * Author: legal
 *
 * Created on 23 janvier 2013, 01:02
 */

#ifndef DECODERLIBRARY_H
#define	DECODERLIBRARY_H

#include <iostream>
using namespace std;

//
// DECODEUR OMS (OFFSET MIN-SUM)
//
//#include "./OMS/CDecoder_OMS_fixed_x86.h"
//#include "./OMS/CDecoder_OMS_fixed_SSE.h"
//#include "./OMS/CDecoder_OMS_fixed_NEON16.h"
#include "./OMS/CDecoder_OMS_fixed_NEON16_v2.h"
//#include "./OMS/CDecoder_NMS_fixed_NEON16_v2.h"
//#include "./OMS/CDecoder_NMS_fixed_NEON16_v3.h"

CDecoder* CreateDecoder(
        param_decoder p_decoder,
        int vMin,       //
        int vMax,       //
        int mMin,       //
        int mMax
    ) {
    
    CDecoder_OMS_fixed_NEON16_v2* dec = new CDecoder_OMS_fixed_NEON16_v2();
    dec->setEarlyTerm( p_decoder.early_term       );
    dec->setOffset   ( p_decoder.oms_offset_fixed );
    dec->setVarRange(vMin, vMax);
    dec->setMsgRange(mMin, mMax);
    return dec;
}

#endif	/* DECODERLIBRARY_H */

