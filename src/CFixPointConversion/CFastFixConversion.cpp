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

#include "CFastFixConversion.h"

CFastFixConversion::CFastFixConversion(CTrame *t, int _FACTEUR_BETA, int _vSAT_NEG_LLR, int _vSAT_POS_LLR) : CFixConversion(t)
{
    FACTEUR_BETA  = _FACTEUR_BETA;
    vSAT_NEG_LLR  = _vSAT_NEG_LLR;
    vSAT_POS_LLR  = _vSAT_POS_LLR;
}

CFastFixConversion::~CFastFixConversion()
{

}

void CFastFixConversion::generate()
{
  for(int z = 0; z<_frames; z++)
  {
    int offset = z * _data;
    for(int i=0; i<_data; i++)
    {
      signed int value = (FACTEUR_BETA * t_noise_data[offset+i]); // ON TRANFORME LES FLOTTANT EN FIXED POINT
      value = (value>vSAT_NEG_LLR) ? value : vSAT_NEG_LLR;        // ON GERE LA SATURATION DES DONNEES DANS LE
      value = (value<vSAT_POS_LLR) ? value : vSAT_POS_LLR;        // FORMAT DEDIE AU LLR
      t_fpoint_data[offset+i] = value;
    }
  }
}
