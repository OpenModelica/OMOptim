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

  @file EIStream.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version 0.9

  */
#include "EIStream.h"

EIStream::EIStream(EIItem* parent, QString name)
    :EIItem(parent,name)
{
    _editableFields << EIStream::NAME << EIStream::TIN_V << EIStream::TIN_U << EIStream::TOUT_V << EIStream::TOUT_U
                    << EIStream::QFLOW_V << EIStream::QFLOW_U << EIStream::DTMIN2;

    QMap<int,QVariant> defaultValues;
    defaultValues.insert(EIStream::TIN_V,0);
    defaultValues.insert(EIStream::TIN_U,METemperature::C);
    defaultValues.insert(EIStream::TOUT_V,0);
    defaultValues.insert(EIStream::TOUT_U,METemperature::C);
    defaultValues.insert(EIStream::QFLOW_V,0);
    defaultValues.insert(EIStream::QFLOW_U,MEQflow::W);
    defaultValues.insert(EIStream::DTMIN2,0);

    for(int i=0;i<defaultValues.count();i++)
        setFieldValue(defaultValues.keys().at(i),defaultValues.values().at(i));

    _numerized = false;
}

EIStream::~EIStream(void)
{
}

EIStream::EIStream(const EIStream & _stream):EIItem(_stream)
{
    _TinRef = _stream._TinRef;
    _ToutRef = _stream._ToutRef;
    _QflowRef = _stream._QflowRef;
    _DTmin2 = _stream._DTmin2;

    _TinNum = _stream._TinNum;
    _ToutNum = _stream._ToutNum;
    _QflowNum = _stream._QflowNum;

    _numerized = _stream._numerized;
}


EIItem* EIStream::clone()
{
    return (EIItem*)(new EIStream(*this));
}

EIStream::EIStream(QDomElement & domEl)
{
    QDomNamedNodeMap attributes = domEl.attributes();
    QString fieldName;
    QString fieldValue;
    int iField;

    for(int i=0;i<attributes.count();i++)
    {
        fieldName = attributes.item(i).toAttr().name();
        fieldName.replace(XMLTools::space()," ");
        fieldValue = attributes.item(i).toAttr().value();
        fieldValue.replace(XMLTools::space()," ");
        iField = getFieldIndex(fieldName,Qt::UserRole);
        setFieldValue(iField,QVariant(fieldValue));
    }

    _editableFields << EIStream::NAME << EIStream::TIN_V << EIStream::TIN_U << EIStream::TOUT_V
                    << EIStream::TOUT_U << EIStream::QFLOW_V << EIStream::QFLOW_U <<EIStream::DTMIN2;
}

QVariant EIStream::getFieldValue(int ifield, int role) const
{


    if (!_filledFields.contains(ifield)&&(role==Qt::DisplayRole))
        return QString("-");
    else
    {
        switch (ifield)
        {
        case NAME :
            return _name;
        case TIN_V :
            return _TinRef.value();
        case TIN_U :
            return _TinRef.unit();
        case TOUT_V :
            return _ToutRef.value();
        case TOUT_U :
            return _ToutRef.unit();
        case QFLOW_V :
            return _QflowRef.value();
        case QFLOW_U :
            return _QflowRef.unit();
        case DTMIN2 :
            return _DTmin2;
        case CHECKED :
            return _checked;
        default :
            return "unknown field";
        }
    }
}

QString EIStream::sFieldName(int ifield, int role)
{
    switch (ifield)
    {
    case NAME :
        return "Name";
    case TIN_V :
        return "Tin";
    case TIN_U :
        switch(role)
        {
        case Qt::DisplayRole:
            return "[Tin]";
        default :
            return "Tin_Unit";
        }
    case TOUT_V :
        return "Tout";
    case TOUT_U :
        switch(role)
        {
        case Qt::DisplayRole:
            return "[Tout]";
        default :
            return "Tout_Unit";
        }
    case QFLOW_V :
        return "Qflow";
    case QFLOW_U :
        switch(role)
        {
        case Qt::DisplayRole:
            return "[Qflow]";
        default :
            return "Qflow_Unit";
        }
    case CHECKED :
        return "Checked";
    case DTMIN2 :
        return "DTmin_2";
    default :
        return "unknown field";
    }
}



bool EIStream::setFieldValue(int ifield,QVariant value_)
{
    bool ok=true;

    switch (ifield)
    {
    case NAME :
        _name=value_.toString();
        break;
    case TIN_V :
        _TinRef.setValue(value_);
        break;
    case TIN_U :
        if(value_.type()==QVariant::String)
            ok=_TinRef.setUnit(value_.toString());
        else
            _TinRef.setUnit(value_.toInt());
        break;
    case TOUT_V :
        _ToutRef.setValue(value_);
        break;
    case TOUT_U :
        if(value_.type()==QVariant::String)
            ok=_ToutRef.setUnit(value_.toString());
        else
            _ToutRef.setUnit(value_.toInt());
        break;
    case QFLOW_V :
        _QflowRef.setValue(value_);
        break;
    case QFLOW_U :
        if(value_.type()==QVariant::String)
            ok=_QflowRef.setUnit(value_.toString());
        else
            _QflowRef.setUnit(value_.toInt());
        break;
    case CHECKED :
        _checked =value_.toBool();
        break;
    case DTMIN2:
        _DTmin2 =value_.toDouble();
        break;
    }
    if(!_filledFields.contains(ifield))
        _filledFields.push_back(ifield);

    emit modified();
    return ok;
}

/**
* @brief This function is used to check if this EIStream is valid or not.
* Checks if _Tin, _Tout and Qflow references are found in variables, that _Tin != _Tout and QFlow >0
* @return true if this EIStream is valid
*/
bool EIStream::isValid(MOOptVector* variables, QString &errMsg)
{
    bool ok = EIItem::isValid(variables,errMsg);

    //#TODO : do not recheck every times, store valid state
    bool ok_Tin,ok_Tout,okQflow;
    double num_Tin,num_Tout,numQFlow;

    num_Tin = this->_TinRef.getNumValue(variables,METemperature::K,ok_Tin,model());
    num_Tout = this->_ToutRef.getNumValue(variables,METemperature::K,ok_Tout,model());
    numQFlow = this->_QflowRef.getNumValue(variables,MEQflow::W,okQflow,model());

    bool okDT = (num_Tin!=num_Tout);
    bool okQPos = (numQFlow>=0);

    return (ok&&okDT&&okQPos&&ok_Tin&&ok_Tout&&okQflow);
}

bool EIStream::isHot(MOOptVector* variables)
{
    bool ok_Tin,ok_Tout;
    return _TinRef.getNumValue(variables,METemperature::K,ok_Tin,model())>_ToutRef.getNumValue(variables,METemperature::K,ok_Tout,model());
}


/**
* This function is used to replace reference by its numerical value extracted from variables vector.
* @return a list of references involved in _Tin, _Tout and Qflow.
*/
QStringList EIStream::references()
{
    QStringList refs;

    refs.push_back(this->_TinRef.reference());
    refs.push_back(this->_ToutRef.reference());
    refs.push_back(this->_QflowRef.reference());

    refs.removeAll(QString());

    return refs;
}

/**
* This function is used to fill numerical values from references and variables.
* In EIStream, this function concerns _TinNum, _ToutNum and _QflowNum fields.
* @return true if all references have been found in variables (or if there were no references), false otherwise
* After a successfull numerization, _numerized is set to true, otherwise to false.
*/
bool EIStream::numerize(MOOptVector* variables)
{
    bool ok1,ok2,ok3;
    _TinNum.setValue(_TinRef.getNumValue(variables,METemperature::K,ok1,model()),METemperature::K);
    _ToutNum.setValue(_ToutRef.getNumValue(variables,METemperature::K,ok2,model()),METemperature::K);
    _QflowNum.setValue(_QflowRef.getNumValue(variables,MEQflow::W,ok3,model()),MEQflow::W);

    _numerized = (ok1 && ok2 && ok3);

    if(!_numerized)
    {
        QString msg;
        msg.sprintf("Failed to numerize EIStream %s. Missing references values",name().toLatin1().data());
        infoSender.send(Info(msg,ListInfo::WARNING2));
    }
    return _numerized;
}

bool EIStream::numerized()
{
    return _numerized;
}

void EIStream::resetNumerize()
{
    _TinNum.setValue(0,0);
    _ToutNum.setValue(0,0);
    _QflowNum.setValue(0,0);
    _numerized = false;
}


/**
* Returns numerical value of Cp. Need numerization first. If numerization is not done, this function will do it
* (using variables parameter)
* If failed to numerize, ok is set to false, and returns -1.
*/
double EIStream::Cp(bool &ok,MOOptVector* variables)
{
    // try to numerize if not done
    // if failure, return -1;
    if(!numerized()&&!numerize(variables))
    {
        ok=false;
        return -1;
    }

    double tinval,toutval,qflowval;

    tinval = _TinNum.value(METemperature::K);
    toutval = _ToutNum.value(METemperature::K);
    qflowval = _QflowNum.value(MEQflow::W);

    if(tinval==toutval)
        return std::numeric_limits<double>::max();
    else
        return fabs(qflowval/fabs(toutval-tinval));
}

METemperature EIStream::TinNum(bool useCorrectedT)
{
    if(!useCorrectedT)
    {
        return _TinNum;
    }
    else
    {
        int factor;
        if(isHot())
            factor=-1;
        else
            factor = 1;
        return _TinNum+factor*_DTmin2;
    }
}


METemperature EIStream::ToutNum(bool useCorrectedT)
{
    if(!useCorrectedT)
    {
        return _ToutNum;
    }
    else
    {
        int factor;
        if(isHot())
            factor=-1;
        else
            factor = 1;
        return _ToutNum+factor*_DTmin2;;
    }

}
