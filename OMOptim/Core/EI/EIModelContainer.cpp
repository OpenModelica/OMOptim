// $Id$
        /**
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR 
 * THIS OSMC PUBLIC LICENSE (OSMC-PL). 
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE
 * OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3, ACCORDING TO RECIPIENTS CHOICE. 
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from OSMC, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or  
 * http://www.openmodelica.org, and in the OpenModelica distribution. 
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)

 	@file EIModelContainer.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "EIModelContainer.h"

        EIModelContainer::EIModelContainer():EIItem(NULL,"Model")
{
    setFieldValue(MODEL,QString());
}

EIModelContainer::EIModelContainer(EIItem* parent,QString name, QString modelName)
    :EIItem(parent,name)
{
    setFieldValue(MODEL,modelName);
}

EIModelContainer::~EIModelContainer()
{
    clearDescendants();
}

EIModelContainer::EIModelContainer(const EIModelContainer & item):EIItem(item)
{
    setFieldValue(MODEL,item._model);
}

EIItem* EIModelContainer::clone()
{
    return (EIItem*)(new EIModelContainer(*this));
}

EIModelContainer::EIModelContainer(QDomElement & domEl)
{
    QDomNamedNodeMap attributes = domEl.attributes();
    QString fieldName;
    QString fieldValue;

    for(int i=0;i<attributes.count();i++)
    {
        fieldName = attributes.item(i).toAttr().name();
        fieldName.replace(XMLTools::space()," ");
        fieldValue = attributes.item(i).toAttr().value();
        fieldValue.replace(XMLTools::space()," ");
        MOItem::setFieldValue(fieldName,QVariant(fieldValue));
    }

    _editableFields << EIModelContainer::NAME;
}

QString EIModelContainer::model()
{
    return _model;
}

QVariant EIModelContainer::getFieldValue(int ifield, int role) const
{
    if (!_filledFields.contains(ifield)&&(role==Qt::DisplayRole))
        return QString("-");
    else
    {
        switch (ifield)
        {
        case NAME :
            return _name;
        case MODEL :
            return _model;
        case CHECKED :
            return _checked;
        default :
                return "unknown field";
    }
    }
}

QString EIModelContainer::sFieldName(int ifield, int role)
{
    switch (ifield)
    {
    case NAME :
        return "Name";
    case MODEL :
        return "Model";
    case CHECKED :
        return "Checked";
    default :
            return "unknown field";
}
}



bool EIModelContainer::setFieldValue(int ifield,QVariant value_)
{
    bool ok=true;

    switch (ifield)
    {
    case NAME :
        _name = value_.toString();
        break;
    case MODEL :
        _model = value_.toString();
        break;
    case CHECKED :
        _checked =value_.toBool();
        break;
    }
    if(!_filledFields.contains(ifield))
        _filledFields.push_back(ifield);

    emit modified();
    return ok;
}
