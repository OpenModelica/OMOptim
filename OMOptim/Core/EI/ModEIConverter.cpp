#include "ModEIConverter.h"

ModEIConverter::ModEIConverter()
{
}


EIStream* ModEIConverter::modClassToEIStream(ModClass* modClass,EIItem* parent,MOomc* moomc, bool &ok, QString &parentName)
{

    // fill fields
    QString shortName = modClass->name(Modelica::SHORT);
    EIStream* eiStream = new EIStream(parent,shortName);

    QMap<EIStream::Field,QString> mapFields;
    mapFields.insert(EIStream::TIN_V,"Tin");
    mapFields.insert(EIStream::TOUT_V,"Tout");
    mapFields.insert(EIStream::QFLOW_V,"QFlow");
    mapFields.insert(EIStream::DTMIN2,"DTmin");
    mapFields.insert(EIStream::CHECKED,"Checked");

    QVariant value;
    EIStream::Field field;
    for(int i=0;i<mapFields.keys().size();i++)
    {
        field = mapFields.keys().at(i);
        value = QVariant(moomc->getComponentModifierValue(modClass->name(),mapFields.value(field)));
        eiStream->setFieldValue(field,value);
    }

    parentName = moomc->getComponentModifierValue(modClass->name(),"Parent");
    if(parentName.compare("Error",Qt::CaseInsensitive)==0)
        parentName.clear();

    ok = true;
    return eiStream;
}


EIGroup* ModEIConverter::modClassToEIGroup(ModClass* modClass,EIItem* parent,MOomc* moomc, bool &ok, QString &parentName )
{


    // fill fields
    QString shortName = modClass->name(Modelica::SHORT);
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
    for(int i=0;i<mapFields.keys().size();i++)
    {
        field = mapFields.keys().at(i);
        value = QVariant(moomc->getComponentModifierValue(modClass->name(),mapFields.value(field)));
        eiGroup->setFieldValue(field,value);
    }

    parentName = moomc->getComponentModifierValue(modClass->name(),"Parent");
    if(parentName.compare("Error",Qt::CaseInsensitive)==0)
        parentName.clear();

    ok = true;
    return eiGroup;
}
