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

#ifndef DECODERLIBRARY_H
#define	DECODERLIBRARY_H

#include <iostream>
using namespace std;

//
// DECODEUR OMS (OFFSET MIN-SUM)
//
#include "./OMS/CDecoder_OMS_fixed_NEON16_v2.h"

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
