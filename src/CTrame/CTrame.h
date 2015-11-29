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

#include <stdlib.h>
#include <stdio.h>

#ifndef CLASS_TRAME
#define CLASS_TRAME

class CTrame
{

protected:
    int  _width;
    int  _height;
    int  _frame;

    int*    t_in_bits;      // taille (var)
    int*    t_coded_bits;   // taille (width)
    float*  t_noise_data;   // taille (width)
    signed char*   t_fpoint_data;  // taille (width/4)
    signed char*   t_decode_data;  // taille (var)
    signed char*   t_decode_bits;  // taille (var)

public:
    CTrame(int width, int height);
    CTrame(int width, int height, int frame);
    ~CTrame();

    int nb_vars();
    int nb_checks();
    int nb_data();
    int nb_frames();

    int*   get_t_in_bits();
    int*   get_t_coded_bits();
    float* get_t_noise_data();
    signed char*  get_t_fpoint_data();
    signed char*  get_t_decode_data();
    signed char*  get_t_decode_bits();
};

#endif
