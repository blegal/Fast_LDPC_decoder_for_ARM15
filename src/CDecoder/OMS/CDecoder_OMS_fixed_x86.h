/**
  Copyright (c) 2012-2015 "Bordeaux INP, Bertrand LE GAL"
  [http://legal.vvv.enseirb-matmeca.fr]
  This file is part of LDPC_C_Simulator.
  LDPC_C_Simulator is free software: you can redistribute it and/or modify
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
#ifndef _CLASS_CDecoder_x86_OMS_
#define _CLASS_CDecoder_x86_OMS_

#include "../template/CDecoder_fixed_x86.h"


class CDecoder_OMS_fixed_x86 : public CDecoder_fixed_x86{
private:
    int _i32_min;
    int _i32_min2;
    int offset;
public:
    CDecoder_OMS_fixed_x86();
    ~CDecoder_OMS_fixed_x86();
    void setOffset(int _offset);
    void decode(signed char var_nodes[], signed char Rprime_fix[], int nombre_iterations);
};

#endif
