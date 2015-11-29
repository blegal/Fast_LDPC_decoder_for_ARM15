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

#ifndef CLASS_ENCODER_GENERIC
#define CLASS_ENCODER_GENERIC

#define xorf(a,b) (a^b)

#include "../CTrame/CTrame.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

class Encoder
{

protected:
    int  _size_in;
    int  _size_out;
    int  _frames;
    int* data_in;
    int* data_out;

public:
    Encoder(CTrame *t);
    virtual ~Encoder();

    virtual int size_in();
    virtual int size_out();
    virtual void encode();
    virtual void sum_bits();
    virtual void sum_pos();
};

#endif
