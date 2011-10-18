#include "EIModelicaExtractor.h"

namespace EI
{
EIModelicaExtractor::EIModelicaExtractor()
{
}


EIModelContainer* EIModelicaExtractor::extractFromModClass(ModClass* model,ModClassTree* modClassTree,MOomc* moomc)
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
    QTime compTime;
    QTime elapsed;

    compTime.restart();
    tmpModEI =  modClassTree->findInDescendantsByClass(EILinguist::getModelicaClassType(EI::GROUP),model);
    elapsed.setHMS(0,0,0);
    elapsed = elapsed.addMSecs(compTime.elapsed());
    qDebug(QString("Finding groups took "+ elapsed.toString("ss:zzz")).toLatin1().data());

    EIGroup* newEIGroup;
    for(int i=0;i<tmpModEI.size();i++)
    {
        compTime.restart();
        newEIGroup = modClassToEIGroup(model->name(),tmpModEI.at(i),tmpParent,moomc,ok,parentName);
        elapsed.setHMS(0,0,0);
        elapsed = elapsed.addMSecs(compTime.elapsed());
        qDebug(QString("building Group "+tmpModEI.at(i)->name() + " took "+ elapsed.toString("ss:zzz")).toLatin1().data());

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
    compTime.restart();
    tmpModEI =  modClassTree->findInDescendantsByClass(EILinguist::getModelicaClassType(EI::STREAM),model);
    elapsed.setHMS(0,0,0);
    elapsed = elapsed.addMSecs(compTime.elapsed());
    qDebug(QString("Finding streams took "+ elapsed.toString("ss:zzz")).toLatin1().data());


    EIStream* newEIStream;

    for(int i=0;i<tmpModEI.size();i++)
    {
        compTime.restart();
        newEIStream = modClassToEIStream(model->name(),tmpModEI.at(i),tmpParent,moomc,ok,parentName);
        elapsed.setHMS(0,0,0);
        elapsed = elapsed.addMSecs(compTime.elapsed());
        qDebug(QString("Finding stream "+tmpModEI.at(i)->name() +"took" + elapsed.toString("ss:zzz")).toLatin1().data());

        if(ok)
        {
            // get fullname and fullparentname
            if(parentName.isEmpty())
                fullParentName = model->name();
            else
                fullParentName = parentName;


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

        if(!parentItem && curItem!=rootEI)
        {
            rootEI->addChild(curItem);
        }
        else if(parentItem && (curItem != rootEI))
        {
            parentItem->addChild(curItem);
        }
    }


    return rootEI;
}



EIStream* EIModelicaExtractor::modClassToEIStream(QString modelName,ModClass* modClass,EIItem* parent,MOomc* moomc, bool &ok, QString &parentName)
{

    // fill fields
    QString shortName = modClass->name(Modelica::FULL);
    shortName = shortName.remove(modelName+".");

    EIStream* eiStream = new EIStream(parent,shortName);

    QMap<EIStream::Field,QString> mapFields;
    mapFields.insert(EIStream::TIN_V,"Tin");
    mapFields.insert(EIStream::TOUT_V,"Tout");
    mapFields.insert(EIStream::QFLOW_V,"QFlow");
    mapFields.insert(EIStream::DTMIN2,"DTmin");
    mapFields.insert(EIStream::CHECKED,"Checked");

    QVariant value;
    EIStream::Field field;

    QString flattenedModel = moomc->getFlattenedModel(modelName);

    for(int i=0;i<mapFields.keys().size();i++)
    {
        field = mapFields.keys().at(i);
        value = QVariant(moomc->getFlattenedModifierValue(modelName,shortName,mapFields.value(field),flattenedModel));

        switch(field)
        {
        case EIStream::TIN_V :
            eiStream->_TinRef.setValue(value,METemperature::K);
        case EIStream::TOUT_V :
            eiStream->_ToutRef.setValue(value,METemperature::K);
        case EIStream::QFLOW_V :
            eiStream->_QflowRef.setValue(value,MEQflow::W);
        default :
            eiStream->setFieldValue(field,value);
        }
    }

    parentName = moomc->getFlattenedModifierValue(modelName,shortName,"Parent",flattenedModel);
    if(!parentName.isEmpty())
    {
        // should be improved !! best way would be to use ref and not string in modelica model
        // but instantiating ~erases name
        parentName = modClass->name(Modelica::FULL).section(".",0,-2)+"."+parentName;
    }

    ok = true;
    return eiStream;
}


EIGroup* EIModelicaExtractor::modClassToEIGroup(QString modelName,ModClass* modClass,EIItem* parent,MOomc* moomc, bool &ok, QString &parentName )
{


    // fill fields
    QString shortName = modClass->name(Modelica::FULL);
    shortName = shortName.remove(modelName+".");

    EIGroup* eiGroup = new EIGroup(parent,shortName);


    QMap<EIGroup::Field,QString> mapFields;
    mapFields.insert(EIGroup::FACTMIN,"FactMin");
    mapFields.insert(EIGroup::FACTMAX,"FactMax");
    mapFields.insert(EIGroup::COSTFIX,"CostFix");
    mapFields.insert(EIGroup::COSTMULT,"CostMult");
    mapFields.insert(EIGroup::FACT,"Fact");
    mapFields.insert(EIGroup::CHECKED,"Checked");
    mapFields.insert(EIGroup::FACTISVARIABLE,"FactVariable");

    QVariant value;
    EIGroup::Field field;

    QString flattenedModel = moomc->getFlattenedModel(modelName);

    for(int i=0;i<mapFields.keys().size();i++)
    {
        field = mapFields.keys().at(i);
        value = QVariant(moomc->getFlattenedModifierValue(modelName,shortName,mapFields.value(field),flattenedModel));

            eiGroup->setFieldValue(field,value);
    }

    parentName = moomc->getFlattenedModifierValue(modelName,shortName,"Parent",flattenedModel);
    if(!parentName.isEmpty())
    {
        // should be improved !! best way would be to use ref and not string in modelica model
        // but instantiating ~erases name
        parentName = modClass->name(Modelica::FULL).section(".",0,-2)+"."+parentName;
    }

    ok = true;
    return eiGroup;
}

}
