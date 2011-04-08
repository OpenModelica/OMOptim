#ifndef MODEICONVERTER_H
#define MODEICONVERTER_H

#include "EIItem.h"
#include "EIStream.h"
#include "EILinguist.h"
#include "MOomc.h"
#include "ModClass.h"
#include "ModModel.h"

class ModEIConverter
{
public:
    ModEIConverter();

    static EIStream* modClassToEIStream(ModClass*,EIItem* parent,MOomc*, bool &ok );
};

#endif // MODEICONVERTER_H
