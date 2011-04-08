#include "EIModelExtractor.h"

EIModelExtractor::EIModelExtractor()
{
}


EIItem* EIModelExtractor::extractFromModClass(ModClass* model,ModReader* modReader,MOomc* moomc)
{
    EIItem* rootEI = new EIItem(NULL,model->name(),model->name());

    QList<ModClass*> tmpModEI;

    EIItem* parent = NULL;

    //Streams
    tmpModEI =  modReader->findCompOfClassInDescendants(model,EILinguist::getModelicaClassType(EI::STREAM));
    EIStream* newEIStream;
    bool ok;
    for(int i=0;i<tmpModEI.size();i++)
    {
        newEIStream = ModEIConverter::modClassToEIStream(tmpModEI.at(i),rootEI,moomc,ok);
        if(ok)
            rootEI->addChild(newEIStream);
        else
            delete newEIStream;
    }


//    //Groups
//    tmpModEI =  modReader->findCompOfClassInDescendants(model,EILinguist::getModelicaClassType(EI::GROUP));
//    EIGroup* newEIGroup;
//    for(int i=0;i<tmpModEI.size();i++)
//    {
//        newEIStream = ModEIConverter::modClassToEIStream(model,parent,moomc,ok);
//        if(ok)
//            allEIItems.push_back(newEIGroup);
//        else
//            delete newEIGroup;
//    }

    return rootEI;
}
