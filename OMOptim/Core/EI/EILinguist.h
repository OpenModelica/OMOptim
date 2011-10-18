#ifndef EILINGUIST_H
#define EILINGUIST_H


#include "EIItem.h"


class EILinguist
{

public :
       static QString getModelicaClassType(EI::Type type)
    {
        switch (type)
        {
        case EI::GROUP:
            return "CERESInfo.EnergyIntegration.EIGroup";
        case EI::GENERIC:
            return "";
        case EI::STREAM:
            return "CERESInfo.EnergyIntegration.EIStream";
        default:
            // should never be reached
            return "";
        }
    }
};








#endif // EILINGUIST_H
