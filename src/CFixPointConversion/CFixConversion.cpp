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

#include "CFixConversion.h"

CFixConversion::CFixConversion(CTrame *t)
{
    _data         = t->nb_data();
    _frames       = t->nb_frames();
    t_noise_data  = t->get_t_noise_data();
    t_fpoint_data = (signed char*)t->get_t_fpoint_data();
}

CFixConversion::~CFixConversion()
{

}

void CFixConversion::generate()
{
    printf("(WW) CFixConversion : execution of the generate function !\n");
    exit( 0 );
}
