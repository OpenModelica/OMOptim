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
 * Main contributor 2011, Hubert Thierot, CEP - ARMINES (France)

 	@file EIConn.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */

#include "EIConn.h"

EIConn::EIConn(EITree* eiTree)
{
    _streamA = NULL;
    _streamB = NULL;
    _eiTree = eiTree;
}

EIConn::~EIConn(void)
{

}

EIConn::EIConn(const EIConn &)
{

}

EIConn::EIConn(QDomElement &domEl, EITree* eiTree)
{
    _eiTree = eiTree;

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
}


// MOItem overwrited
bool EIConn::isValid()
{
    return (_streamA && _streamB);
}

// Specific functions
void EIConn::setA(EIStream* a,METemperature Tina,METemperature Touta,double fracta)
{
    _streamA = a;
    _TinA = Tina;
    _ToutA = Touta;
    _fractA = fracta;
}

void EIConn::setB(EIStream* b,METemperature Tinb,METemperature Toutb,double fractb)
{
    _streamB = b;
    _TinB = Tinb;
    _ToutB = Toutb;
    _fractB = fractb;
}

void EIConn::setQFlow(MEQflow qFlow)
{
    _qFlow = qFlow;
}


QVariant EIConn::getFieldValue(int iField, int role ) const
{
    switch(iField)
    {
    case NAME:
        return _name;
    case STREAMA :
        if(_streamA)
            return _streamA->name();
        break;
    case FRACTA :
        return _fractA;
    case TINA_V :
        return _TinA.value();
    case TINA_U :
        return _TinA.unit();
    case TOUTA_V :
        return _ToutA.value();
    case TOUTA_U :
        return _ToutA.unit();
    case STREAMB :
        if(_streamB)
            return _streamB->name();
        break;
    case FRACTB :
        return _fractB;
    case TINB_V :
        return _TinB.value();
    case TINB_U :
        return _TinB.unit();
    case TOUTB_V :
        return _ToutB.value();
    case TOUTB_U :
        return _ToutB.unit();
    case QFLOW_V :
        return _qFlow.value();
    case QFLOW_U :
        return _qFlow.unit();
    }

    return QVariant();
}

QString EIConn::sFieldName(int field, int role)
{
    switch(field)
    {
    case NAME:
        return "Name";
    case STREAMA :
        return "Stream A";
    case FRACTA :
        return "Fraction of A";
    case TINA_V :
        return "TinA";
    case TINA_U :
        switch(role)
        {
        case Qt::DisplayRole:
            return "[TinA]";
        default :
                return "TinA_Unit";
        }
    case TOUTA_V :
        return "ToutA";
    case TOUTA_U :
        switch(role)
        {
        case Qt::DisplayRole:
            return "[ToutA]";
        default :
            return "ToutA_Unit";
        }
    case STREAMB :
        return "Stream B";
    case FRACTB :
        return "Fraction of B";
    case TINB_V :
        return "TinB";
    case TINB_U :
        switch(role)
        {
        case Qt::DisplayRole:
            return "[TinB]";
        default :
                return "TinB_Unit";
        }
    case TOUTB_V :
        return "ToutB";
    case TOUTB_U :
        switch(role)
        {
        case Qt::DisplayRole:
            return "[ToutB]";
        default :
            return "ToutB_Unit";
        }

    case QFLOW_V :
        return "QFlow";
    case QFLOW_U :
        switch(role)
        {
        case Qt::DisplayRole:
            return "[QFlow]";
        default :
            return "QFlow";
        }
    }
    return QString();
}

bool EIConn::setFieldValue(int field,QVariant value)
{
    bool ok=true;

        switch(field)
        {
        case NAME:
            _name = value.toString();
        case STREAMA :
                _streamA = dynamic_cast<EIStream*>(_eiTree->findItem(value.toString()));
                ok = (bool)_streamA;
                break;
        case TINA_V :
                _TinA.setValue(value.toDouble());
                break;
        case TINA_U :
                if(value.type()==QVariant::String)
                        ok=_TinA.setUnit(value.toString());
                else
                        _TinA.setUnit(value.toInt());
                break;
        case TOUTA_V :
                _ToutA.setValue(value.toDouble());
                break;
        case TOUTA_U :
                if(value.type()==QVariant::String)
                        ok=_ToutA.setUnit(value.toString());
                else
                        _ToutA.setUnit(value.toInt());
                break;
        case FRACTA:
                _fractA = value.toDouble();
                break;

        case STREAMB :
                _streamB = dynamic_cast<EIStream*>(_eiTree->findItem(value.toString()));
                ok = (bool)_streamB;
                break;
        case TINB_V :
                _TinB.setValue(value.toDouble());
                break;
        case TINB_U :
                if(value.type()==QVariant::String)
                        ok=_TinB.setUnit(value.toString());
                else
                        _TinB.setUnit(value.toInt());
                break;
        case TOUTB_V :
                _ToutB.setValue(value.toDouble());
                break;
        case TOUTB_U :
                if(value.type()==QVariant::String)
                        ok=_ToutB.setUnit(value.toString());
                else
                        _ToutB.setUnit(value.toInt());
                break;
        case FRACTB:
                _fractB = value.toDouble();
                break;



        case QFLOW_V :
                _qFlow.setValue(value.toDouble());
                break;
        case QFLOW_U :
                if(value.type()==QVariant::String)
                        ok=_qFlow.setUnit(value.toString());
                else
                        _qFlow.setUnit(value.toInt());
                break;
        }
        return ok;
}
