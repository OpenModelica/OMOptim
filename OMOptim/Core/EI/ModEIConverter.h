#ifndef MODEICONVERTER_H
#define MODEICONVERTER_H

#include "EIItem.h"
#include "EIStream.h"
#include "EIGroup.h"
#include "EILinguist.h"
#include "MOomc.h"
#include "ModClass.h"
#include "ModModel.h"

class ModEIConverter
{
public:
    ModEIConverter();

    static EIStream* modClassToEIStream(ModClass*,EIItem* parent,MOomc*, bool &ok, QString &parentName );
    static EIGroup* modClassToEIGroup(ModClass*,EIItem* parent,MOomc*, bool &ok, QString &parentName );
};

#endif // MODEICONVERTER_H
