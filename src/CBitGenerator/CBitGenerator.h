#ifndef CLASS_CCbitGenerator
#define CLASS_CCbitGenerator

#include "../CTrame/CTrame.h"

class CBitGenerator
{
    
protected:
    int  _vars;
    bool _zero_mode;
    int*  t_in_bits;      // taille (var)
    
public:
    CBitGenerator(CTrame *t, bool zero_only);
    virtual void generate();

};

#endif
