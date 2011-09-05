#include "EIModelExtractor.h"

namespace EI
{
EIModelExtractor::EIModelExtractor()
{
}


EIModelContainer* EIModelExtractor::extractFromModClass(ModClass* model,ModClassTree* modClassTree,MOomc* moomc)
{


    MOKeepVector<EIItem> listEIItems;
    QMap<QString,EIItem*> mapFullNames;
    QMultiMap<QString,EIItem*> mapFullParentNames;
    QList<ModClass*> tmpModEI;

    //create root and add it to list
    EIModelContainer* rootEI = new EIModelContainer(NULL,model->name(),model->name());
    listEIItems.addItem(rootEI);
    mapFullNames.insert(rootEI->name(EI::SHORT),rootEI);

    EIItem* tmpParent= NULL;
    QString parentName;
    QString fullName;
    QString fullParentName;
    bool ok;

    //Groups
    tmpModEI =  modClassTree->findCompOfClassInDescendants(EILinguist::getModelicaClassType(EI::GROUP),model);
    EIGroup* newEIGroup;
    for(int i=0;i<tmpModEI.size();i++)
    {
        newEIGroup = ModEIConverter::modClassToEIGroup(tmpModEI.at(i),tmpParent,moomc,ok,parentName);
        if(ok)
        {
            // get fullname and fullparentname
            if(parentName.isEmpty())
                fullParentName = model->name();
            else
                fullParentName = model->name()+"."+parentName;

            fullName = fullParentName+"."+newEIGroup->name(EI::SHORT);

            // add new EIGroup
            listEIItems.addItem(newEIGroup);
            mapFullNames.insert(fullName,newEIGroup);
            mapFullParentNames.insert(fullParentName,newEIGroup);
        }
        else
            delete newEIGroup;
    }

    //Streams
    tmpModEI =  modClassTree->findCompOfClassInDescendants(EILinguist::getModelicaClassType(EI::STREAM),model);
    EIStream* newEIStream;

    for(int i=0;i<tmpModEI.size();i++)
    {
        newEIStream = ModEIConverter::modClassToEIStream(tmpModEI.at(i),tmpParent,moomc,ok,parentName);
        if(ok)
        {
            // get fullname and fullparentname
            if(parentName.isEmpty())
                fullParentName = model->name();
        else
                fullParentName = model->name()+"."+parentName;

            fullName = fullParentName+"."+newEIStream->name(EI::SHORT);

            // add new EIGroup
            listEIItems.addItem(newEIStream);
            mapFullNames.insert(fullName,newEIStream);
            mapFullParentNames.insert(fullParentName,newEIStream);
        }
        else
            delete newEIStream;

    }

    // add all items
    EIItem* parentItem;
    EIItem* curItem;
    for(int i=0;i<listEIItems.items.size();i++)
    {
        curItem = listEIItems.items.at(i);
        parentItem = mapFullNames.value(mapFullParentNames.key(curItem),NULL);

        if(!parentItem)
        {
            if(curItem!=rootEI)
            {
                //ERROR
                QString msg;
                msg.sprintf("Error reading eiitem %s : could not find parent. Item will not be considered",listEIItems.items.at(i)->name(EI::SHORT).toLatin1().data());
                infoSender.send(Info(msg,ListInfo::WARNING2));
            }
        }
        else
        {
            parentItem->addChild(curItem);
        }
    }


    return rootEI;
}
}
