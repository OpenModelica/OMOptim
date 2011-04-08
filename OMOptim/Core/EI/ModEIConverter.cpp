#include "ModEIConverter.h"

ModEIConverter::ModEIConverter()
{
}


EIStream* ModEIConverter::modClassToEIStream(ModClass* modClass,EIItem* parent,MOomc* moomc, bool &ok )
{

    //check modClass


    // fill fields
    QString shortName = modClass->name(Modelica::FULL);
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

    ok = true;
    return eiStream;
}
