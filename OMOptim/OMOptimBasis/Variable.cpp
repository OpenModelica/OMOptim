// $Id$
/**
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linkpings universitet, Department of Computer and Information Science,
 * SE-58183 Linkping, Sweden.
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

  @file Variable.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version

  */
#include "Variable.h"
#include "CSVBase.h"


Variable::Variable(void)
{
    _causality = UNKNOWN;
}

Variable::Variable(QString name)
{
    setName(name);
}

Variable::Variable(const Variable & var):MOItem(var)
{
    QVariant curValue;
    for(int i=0;i<nbFields;i++)
    {
        curValue = var.getFieldValue(i);
        setFieldValue(i,curValue);
    }
    _filledFields = var._filledFields;
    _editableFields = var._editableFields;
}

Variable::Variable(QDomElement & domEl)
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
}

Variable::~Variable(void)
{
}

Variable* Variable::clone() const
{
    return new Variable(*this);
}

void Variable::setDescription(QString description)
{
    if(!_filledFields.contains(Variable::DESCRIPTION))
        _filledFields.push_back(Variable::DESCRIPTION);
    _description = description;
}
//void Variable::setType(int type_)
//{
//        if(!_filledFields.contains(Variable::TYPE))
//        _filledFields.push_back(Variable::TYPE);
//
//    type = type_;
//}

void Variable::setValue(double value)
{
    if(!_filledFields.contains(Variable::VALUE))
        _filledFields.push_back(Variable::VALUE);
    _value = value;
}

double Variable::value() const
{
    return _value;
}
//
//void Variable::setMin(double min_)
//{
//    if(!_filledFields.contains(Variable::MIN))
//        _filledFields.push_back(Variable::MIN);
//    min = min_;
//}
//
//void Variable::setMax(double max_)
//{
//    if(!_filledFields.contains(Variable::MAX))
//        _filledFields.push_back(Variable::MAX);
//    max = max_;
//}
//void Variable::setCategory(int category_)
//{
//    if(!_filledFields.contains(Variable::CATEGORY))
//        _filledFields.push_back(Variable::CATEGORY);
//    category = category_;
//}
void Variable::setDataType(VariableType dataType)
{
    if(!_filledFields.contains(Variable::DATATYPE))
        _filledFields.push_back(Variable::DATATYPE);
    _dataType = dataType;
}

void Variable::setCausality(VariableCausality causality)
{
    if(!_filledFields.contains(Variable::CAUSALITY))
        _filledFields.push_back(Variable::CAUSALITY);
    _causality = causality;
}


QVariant Variable::getFieldValue(int ifield, int role) const
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
        case VALUE :
            return _value;
        case DESCRIPTION :
            return _description;
            /*case MIN :
   return min;
  case MAX :
   return max;
  case TYPE :
   return type;
  case CATEGORY :
   return category;*/
        case DATATYPE :
            if(role==Qt::DisplayRole)
            {
                switch(_dataType)
                {
                case OMREAL :
                    return "Real";
                case OMINTEGER :
                    return "Integer";
                case OMBOOLEAN :
                    return "Boolean";
                default :
                    return "-";
                }
            }
            else
                return _dataType;
        case CAUSALITY :
            if(role==Qt::DisplayRole)
            {
                switch(_causality)
                {
                case INPUT :
                    return "Input";
                case OUTPUT :
                    return "Output";
                case UNKNOWN:
                    return "-";
                default :
                    return "-";
                }
            }
            else
                return _causality;
        default :
            return "unknown field";
        }
    }
}

QString Variable::sFieldName(int ifield, int role)
{
    switch (ifield)
    {
    case NAME :
        return "Name";
    case MODEL :
        return "Model";
    case VALUE :
        return "Value";
    case DESCRIPTION :
        return "Description";
        /*case MIN :
   return "Min";
  case MAX :
   return "Max";
  case TYPE :
   return "Type";
  case CATEGORY :
   return "Category";*/
    case DATATYPE:
        return "Data type";
    case CAUSALITY:
        return "Causality";
    default :
        return "unknown field";
    }
}

QString Variable::description()
{
    return _description;
}

QString Variable::name(Variable::NameFormat format) const
{
    switch(format)
    {
    case Variable::FULL :
        if(!_model.isEmpty())
            return _model+"."+_name;
        else
            return _name;
    case Variable::SHORT :
        return _name;
    default :
        return QString();
    }
}

bool Variable::setFieldValue(int ifield,QVariant value)
{
    try{
        switch (ifield)
        {
        case NAME :
            _name=value.toString();
            break;
        case VALUE :
            _value=value.toDouble();
            break;
        case MODEL :
            _model = value.toString();
        case DESCRIPTION :
            _description = value.toString();
            break;
            /*case MIN :
   min=value_.toDouble();
   break;
  case MAX :
   max=value_.toDouble();
   break;
  case TYPE :Variable
   type=value_.toInt();
   break;
  case CATEGORY :
   category=value_.toInt();
   break;*/
        case DATATYPE :
            _dataType=(VariableType)value.toInt();
            break;
        case CAUSALITY :
            _causality=(VariableCausality)value.toInt();
            break;
        }
        if(!_filledFields.contains(ifield))
            _filledFields.push_back(ifield);
        return true;
    }
    catch(std::exception &e)
    {
        return false;
    }
}



QString Variable::getStrToolTip()
{
    QString result;
    for(int iF=0;iF<getNbFields();iF++)
    {
        result += getFieldName(iF);
        result += " : ";
        result += getFieldValue(iF).toString();
        result += " \n";
    }
    return result;

}

void Variable::setModel(QString model)
{
    _model = model;

    _filledFields << Variable::MODEL;
}

QString Variable::model() const
{
    return _model;
}

VariableResult::VariableResult()
{
    for(int i=0;i<Variable::nbFields;i++)
    {
        setIsEditableField(i,false);
    }
}

VariableResult::VariableResult(const VariableResult& var):Variable(var)
{
    _finalValues = var._finalValues;
    _computedPoints = var._computedPoints;
}

VariableResult::VariableResult(const Variable& var):Variable(var)
{
    setFinalValue(0,0,_value);
}

VariableResult::VariableResult(QDomElement & domEl)
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

        int iField = getFieldIndex(fieldName);
        if(iField!=-1)
            MOItem::setFieldValue(fieldName,QVariant(fieldValue));
    }

    QDomElement domValues = domEl.firstChildElement("Values");
    if(!domValues.isNull())
    {
        CSVBase::linesToOneVariableResultValues(this,domValues.text());
    }
}



VariableResult::~VariableResult()
{

}

VariableResult* VariableResult::clone() const
{
    return new VariableResult(*this);
}

std::vector<double> VariableResult::finalValues(int iScan) const
{
    return _finalValues.at(iScan);
}

double VariableResult::finalValue(int iScan,int iPoint) const
{
    if((iScan>=0)&&(iScan<_finalValues.size())
            && (iPoint>=0) && (iPoint<_finalValues.at(iScan).size()))
        return _finalValues.at(iScan).at(iPoint);

    else
    {
        QString msg;
        msg = "Error reading final value of variable \""+name()+"\" , iScan : "+QString::number(iScan) + " iPoint : "+QString::number(iPoint)+"\n";
        InfoSender::instance()->debug(msg);
        return LowTools::nan();
    }
}

std::vector<double> VariableResult::finalValuesAtPoint(int iPoint)
{
    std::vector<double> result;
    for(int i=0;i<nbScans();i++)
        result.push_back(finalValue(i,iPoint));

    return result;
}
std::vector<double> VariableResult::finalValuesAtScan(int iScan)
{
    if(iScan<_finalValues.size())
        return _finalValues.at(iScan);
    else
        return std::vector<double>();
}



void VariableResult::setFinalValuesAtScan(int iScan,const std::vector<double> & finalValues)
{
    if(iScan<finalValues.size())
    {
        _finalValues.at(iScan).clear();
        _computedPoints.at(iScan).clear();
        _finalValues.at(iScan).reserve(finalValues.size());
        _computedPoints.at(iScan).reserve(finalValues.size());

        _finalValues.push_back(finalValues);

        for(int i=0;i<finalValues.size();i++)
        {
            _computedPoints.at(iScan).push_back(true);
        }
    }
    else
    {
        QString msg;
        msg = "Error setting final values of variable \""+name()+"\" , iScan : "+QString::number(iScan) + "\n";
        InfoSender::instance()->debug(msg);
    }
}

void VariableResult::setFinalValuesAtPoint(int iPoint,const std::vector<double> &values)
{
    // resize if needed
    // scans (must be done before points)
    std::vector<double> defaultValues(nbPoints(),0);
    std::vector<bool> defaultComputed(nbPoints(),false);

    if(values.size()>=nbScans())
    {
        _finalValues.resize(values.size(),defaultValues);
        _computedPoints.resize(values.size(),defaultComputed);
    }
    // points
    if(iPoint>=nbPoints())
    {
        for(int i=0;i<_finalValues.size();i++)
        {
            _finalValues.at(i).resize(iPoint+1,0);
            _computedPoints.at(i).resize(iPoint+1,false);
        }
    }

    // set values
    for(int iScan=0;iScan<values.size();iScan++)
    {
        setFinalValue(iScan,iPoint,values.at(iScan));
    }
}

void VariableResult::setFinalValueAtPoint(int iPoint,double value)
{
    setFinalValue(0,iPoint,value);
}



void VariableResult::setFinalValue(int iScan,int iPoint,double value,bool computed)
{
    // resize if needed
    // scans (must be done before points)
    std::vector<double> defaultValues(nbPoints(),0);
    std::vector<bool> defaultComputed(nbPoints(),false);

    if(iScan>=nbScans())
    {
        _finalValues.resize(iScan+1,defaultValues);
        _computedPoints.resize(iScan+1,defaultComputed);
    }
    // points
    if(iPoint>=nbPoints())
    {
        for(int i=0;i<_finalValues.size();i++)
        {
            _finalValues.at(i).resize(iPoint+1,0);
            _computedPoints.at(i).resize(iPoint+1,false);
        }
    }

    _finalValues.at(iScan).at(iPoint) = value;
    _computedPoints.at(iScan).at(iPoint) = computed;
}

//void VariableResult::appendFinalValue(double value,int iScan)
//{
//    if(iScan==_finalValues.size())
//    {
//        _finalValues.push_back(std::vector<double>());
//        _computedPoints.push_back(std::vector<bool>());
//    }
//    if(iScan>=_finalValues.size())
//    {
//        QString msg;
//        msg.sprintf("appdendFinalValue with iScan > finalValues.nbScans (variable : %s",name().utf16());
//        InfoSender::instance()->send(Info(msg,ListInfo::INFODEBUG));
//    }

//    _finalValues.at(iScan).push_back(value);
//    _computedPoints.at(iScan).push_back(true);
//}

//void VariableResult::appendScanValues(std::vector<double> values,std::vector<bool> computedPoints)
//{
//    _finalValues.push_back(values);
//    _computedPoints.push_back(computedPoints);
//}

//void VariableResult::appendScanValue(double value,bool computedPoint)
//{
//    std::vector<double> values;
//    std::vector<bool> computedPoints;
//    values.push_back(value);
//    computedPoints.push_back(computedPoint);
//    _finalValues.push_back(values);
//    _computedPoints.push_back(computedPoints);
//}

bool VariableResult::isComputedPoint(int iScan, int iPoint) const
{
    if((iScan<_computedPoints.size())
            && (iPoint<_computedPoints.at(iScan).size()))
        return _computedPoints.at(iScan).at(iPoint);
    else
        return false;
}

int VariableResult::nbPoints() const
{
    int size=0;
    for(int i=0;i<_finalValues.size();i++)
        size = std::max<double>(size,_finalValues.at(i).size());

    return size;
}
int VariableResult::nbScans() const
{
    return _finalValues.size();
}

void VariableResult::clearFinalValues()
{
    for(int i=0;i<_finalValues.size();i++)
        _finalValues.at(i).clear();
    _finalValues.clear();

    for(int i=0;i<_computedPoints.size();i++)
        _computedPoints.at(i).clear();
    _computedPoints.clear();
}

void VariableResult::clearFinalValuesAtIpoint(int iPoint)
{
    for(int iScan=0;iScan<_finalValues.size();iScan++)
        setFinalValue(iScan,iPoint,0,false);
}
QDomElement VariableResult::toXmlData(QDomDocument & doc)
{
    // Root element
    QDomElement cItem = doc.createElement(getClassName());
    QString fieldName;
    QString fieldValue;
    for(int iF=0;iF<getNbFields();iF++)
    {
        if(iF!=VariableResult::VALUE)
        {
            fieldName = getFieldName(iF);
            fieldName.replace(" ",XMLTools::space());
            fieldValue = getFieldValue(iF).toString();
            fieldValue.replace(" ",XMLTools::space());
            cItem.setAttribute(fieldName,fieldValue);
        }
    }

    QDomElement values = doc.createElement("Values");
    QDomText text = doc.createTextNode(CSVBase::oneVariableResultToValueLines(this));
    values.appendChild(text);
    cItem.appendChild(values);

    return cItem;
}



OptVariable::OptVariable()
{
    _editableFields << OptVariable::VALUE << OptVariable::OPTMIN << OptVariable::OPTMAX;
}

OptVariable::OptVariable(QDomElement & domEl)
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
    _editableFields << OptVariable::VALUE << OptVariable::OPTMIN << OptVariable::OPTMAX;
}

OptVariable::~OptVariable()
{
}

/**
* This function is used to check if an OptVariable is correctly filled.
* @sa MOItem::check()
*/
bool OptVariable::check(QString &error)
{
    bool ok=true;
    error.clear();
    if(_optMin>=_optMax)
    {
        error.sprintf("Opt min value >= max value (%f>%f)",_optMin,_optMax);
        ok=false;
    }

    //if(type!=FIXED)
    //{
    //    error.sprintf("Variable is not considered as a model input. Its value probably won't affect simulation");
    //    ok = false;
    //}

    return ok;
}

OptVariable::OptVariable(const Variable & var):Variable(var)
{
    initOptExtremum();
    _editableFields << OptVariable::VALUE << OptVariable::OPTMIN << OptVariable::OPTMAX;
}

OptVariable::OptVariable(const OptVariable & var):Variable(var)
{
    _optMin = var._optMin;
    _optMax = var._optMax;
}

OptVariable* OptVariable::clone() const
{
    return new OptVariable(*this);
}

void OptVariable::initOptExtremum()
{
    // should take half of maximum/minimum values since
    // amplitude should still be inf to max
    switch(_dataType)
    {
    case OMREAL :
        _optMin = -std::numeric_limits<double>::max()/2;
        _optMax = std::numeric_limits<double>::max()/2;
        break;
    case OMINTEGER :
        _optMin = -std::numeric_limits<int>::max()/2;
        _optMax = std::numeric_limits<int>::max()/2;
        break;
    case OMBOOLEAN :
        _optMin = 0;
        _optMax = 1;
        break;
    }
}




QVariant OptVariable::getFieldValue(int ifield, int role) const
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
        case VALUE :
            return _value;
        case DESCRIPTION :
            return _description;
            /*case MIN :
   return min;
  case MAX :
   return max;
  case TYPE :
   return type;
  case CATEGORY :
   return category;*/
        case DATATYPE :
            if(role==Qt::DisplayRole)
            {
                switch(_dataType)
                {
                case OMREAL :
                    return "Real";
                case OMINTEGER :
                    return "Integer";
                case OMBOOLEAN :
                    return "Boolean";
                default :
                    return "-";
                }
            }
            else
                return _dataType;
        case CAUSALITY :
            if(role==Qt::DisplayRole)
            {
                switch(_causality)
                {
                case INPUT :
                    return "Input";
                case OUTPUT :
                    return "Output";
                case UNKNOWN:
                    return "-";
                default :
                    return "-";
                }
            }
            else
                return _causality;
        case OPTMIN :
            if((role == Qt::DisplayRole)&&(_optMin==-std::numeric_limits<double>::max()))
                return "-";
            else
                return _optMin;
        case OPTMAX :
            if((role == Qt::DisplayRole)&&(_optMax==std::numeric_limits<double>::max()))
                return "-";
            else
                return _optMax;
        default :
            return "unknown field";
        }
    }
}

QString OptVariable::sFieldName(int ifield, int role)
{
    switch (ifield)
    {
    case NAME :
        return "Name";
    case MODEL :
        return "Model";
    case VALUE :
        return "Value";
    case DESCRIPTION :
        return "Description";
        /*case MIN :
   return "Min";
  case MAX :
   return "Max";
  case TYPE :
   return "Type";
  case CATEGORY :
   return "Category";*/
    case DATATYPE:
        return "Data type";
    case CAUSALITY:
        return "Causality";
    case OPTMIN:
        return "Opt Minimum";
    case OPTMAX:
        return "Opt Maximum";
    default :
        return "unknown field";
    }
}



bool OptVariable::setFieldValue(int ifield,QVariant value)
{
    bool isDouble;

    try{
        switch (ifield)
        {
        case NAME :
            _name=value.toString();
            break;
        case MODEL :
            _model=value.toString();
            break;
        case VALUE :
            _value=value.toDouble();
            break;
        case DESCRIPTION :
            _description = value.toString();
            break;
            /*case MIN :
   min=value_.toDouble();
   break;
  case MAX :
   max=value_.toDouble();
   break;
  case TYPE :
   type=value_.toInt();
   break;
  case CATEGORY :
   category=value_.toInt();
   break;*/
        case DATATYPE :
            _dataType=(VariableType)value.toInt();
            break;
        case CAUSALITY :
            _causality=(VariableCausality)value.toInt();
            break;
        case OPTMIN :
            _optMin = value.toDouble(&isDouble);
            if(!isDouble)
                _optMin = -std::numeric_limits<double>::max();
            break;
        case OPTMAX :
            _optMax = value.toDouble(&isDouble);
            if(!isDouble)
                _optMax = std::numeric_limits<double>::max();
            break;
        }
        if(!_filledFields.contains(ifield))
            _filledFields.push_back(ifield);
        return true;
    }
    catch(std::exception &e)
    {
        return false;
    }
}




ScannedVariable::ScannedVariable()
{
    _editableFields << ScannedVariable::VALUE << ScannedVariable::SCANMIN << ScannedVariable::SCANMAX << ScannedVariable::SCANSTEP;
}

ScannedVariable::ScannedVariable(QDomElement & domEl)
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

    _editableFields << ScannedVariable::VALUE << ScannedVariable::SCANMIN << ScannedVariable::SCANMAX << ScannedVariable::SCANSTEP;
}

ScannedVariable::~ScannedVariable()
{
}


ScannedVariable::ScannedVariable(const ScannedVariable & var):Variable(var)
{
    _scanMin = var._scanMin;
    _scanMax = var._scanMax;
    _scanStep = var._scanStep;
}

ScannedVariable* ScannedVariable::clone() const
{
    return new ScannedVariable(*this);
}


ScannedVariable::ScannedVariable(const Variable & var):Variable(var)
{
    initScanExtremum();

    _editableFields << ScannedVariable::VALUE << ScannedVariable::SCANMIN << ScannedVariable::SCANMAX << ScannedVariable::SCANSTEP ;
}

/**
* This function is used to check if an ScannedVariable is correctly filled.
* @sa MOItem::check()
*/
bool ScannedVariable::check(QString &error)
{
    bool ok=true;
    error.clear();

    if(_scanMin>_scanMax)
    {
        error.sprintf("Scan min value > max value (%f>%f)",_scanMin,_scanMax);
        return false;
    }

    //if(type!=FIXED)
    //{
    //    error.sprintf("Variable is not considered as a model input. Its value probably won't affect simulation");
    //    ok = false;
    //}
    return ok;
}

void ScannedVariable::initScanExtremum()
{
    _scanMin = -std::numeric_limits<double>::max();
    _scanMax = std::numeric_limits<double>::max();

    //if(min < -std::numeric_limits<double>::max())
    //    scanMin = -std::numeric_limits<double>::max();
    //else
    //    scanMin = min;

    //if(max > std::numeric_limits<double>::max())
    //    scanMax = std::numeric_limits<double>::max();
    //else
    //    scanMax = max;

    _scanStep = 1;
}

int ScannedVariable::nbScans()
{
    //#TOCHECK
    if(_scanStep>0)
        return (int)(_scanMax-_scanMin)/_scanStep + 1;
    else
        return 0;
}



QVariant ScannedVariable::getFieldValue(int ifield, int role) const
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
        case VALUE :
            return _value;
        case DESCRIPTION :
            return _description;
            /*    case MIN :
   return min;
  case MAX :
   return max;
  case TYPE :
   return type;
  case CATEGORY :
   return category;*/
        case DATATYPE :
            if(role==Qt::DisplayRole)
            {
                switch(_dataType)
                {
                case OMREAL :
                    return "Real";
                case OMINTEGER :
                    return "Integer";
                case OMBOOLEAN :
                    return "Boolean";
                default :
                    return "-";
                }
            }
            else
                return _dataType;
        case CAUSALITY :
            if(role==Qt::DisplayRole)
            {
                switch(_causality)
                {
                case INPUT :
                    return "Input";
                case OUTPUT :
                    return "Output";
                case UNKNOWN :
                    return "-";
                default :
                    return "-";
                }
            }
            else
                return _causality;
        case SCANMIN :
            return _scanMin;
        case SCANMAX :
            return _scanMax;
        case SCANSTEP :
            return _scanStep;
        default :
            return "unknown field";
        }
    }
}

QString ScannedVariable::sFieldName(int ifield, int role)
{
    switch (ifield)
    {
    case NAME :
        return "Name";
    case MODEL :
        return "Model";
    case VALUE :
        return "Value";
    case DESCRIPTION :
        return "Description";
        /*case MIN :
   return "Min";
  case MAX :
   return "Max";
  case TYPE :
   return "Type";
  case CATEGORY :
   return "Category";*/
    case DATATYPE:
        return "Data type";
    case CAUSALITY:
        return "Causality";
    case SCANMIN:
        return "Scan Minimum";
    case SCANMAX:
        return "Scan Maximum";
    case SCANSTEP:
        return "Scan Step";
    default :
        return "unknown field";
    }
}



bool ScannedVariable::setFieldValue(int ifield,QVariant value)
{
    try{
        switch (ifield)
        {
        case NAME :
            _name=value.toString();
            break;
        case MODEL :
            _model=value.toString();
            break;
        case VALUE :
            _value=value.toDouble();
            break;
        case DESCRIPTION :
            _description = value.toString();
            break;
            /*    case MIN :
   min=value_.toDouble();
   break;
  case MAX :
   max=value_.toDouble();
   break;
  case TYPE :
   type=value_.toInt();
   break;
  case CATEGORY :
   category=value_.toInt();
   break;*/
        case DATATYPE :
            _dataType=(VariableType)value.toInt();
            break;
        case CAUSALITY :
            _causality=(VariableCausality)value.toInt();
            break;
        case SCANMIN :
            _scanMin=value.toDouble();
            break;
        case SCANMAX :
            _scanMax=value.toDouble();
            break;
        case SCANSTEP :
            _scanStep=value.toDouble();
            break;
        }
        if(!_filledFields.contains(ifield))
            _filledFields.push_back(ifield);
        return true;
    }
    catch(std::exception &e)
    {
        return false;
    }
}




//
//
//OptVariableiableResult::OptVariableResult()
//{
//    _editableFields.clear();
//
//}
//
//OptVariableResult::OptVariableResult(OptVariable *optVar_)
//{
//    for(int i=0;i<OptVariable::nbFields;i++)
//    {
//        setFieldValue(i,optVar_->getFieldValue(i));
//    }
//
//    _filledFields = optVar_->_filledFields;
//
//    _editableFields.clear();
//}
//
//OptVariableResult::OptVariableResult(OptVariableResult *optVarRes_)
//{
//    for(int i=0;i<OptVariableResult::nbFields;i++)
//    {
//        setFieldValue(i,optVarRes_->getFieldValue(i));
//    }
//
//    _filledFields = optVarRes_->_filledFields;
//
//    for(int i=0;i<optVarRes_->nbScans();i++)
//        setFinalValues(i,optVarRes_->finalValues(i));
//
//    _editableFields.clear();
//}
//
//
//
//OptVariableResult::~OptVariableResult()
//{
//}
//
//OptVariableResult* OptVariableResult::clone()
//{
//    OptVariableResult* newVar = new OptVariableResult();
//
//    // copying fields
//    for(int i=0;i<nbFields;i++)
//    {
//        newVar->setFieldValue(i,getFieldValue(i));
//    }
//    newVar->_filledFields = _filledFields;
//
//    //copying final values
//    for(int i=0;i<nbScans();i++)
//        newVar->setFinalValues(i,finalValues(i));
//
//    return newVar;
//}
//
//OptVariable* OptVariableResult::getEquivOptVariable()
//{
//    OptVariable* newVar = new OptVariable();
//
//    // copying fields
//    for(int i=0;i<OptVariable::nbFields;i++)
//    {
//        newVar->setFieldValue(i,getFieldValue(i));
//    }
//
//    return newVar;
//}
//
//std::vector<double> OptVariableResult::finalValues(int iScan) const
//{
//    return finalValues.at(iScan);
//}
//
//double OptVariableResult::finalValue(int iScan,int iPoint) const
//{
//    return finalValues.at(iScan).at(iPoint);
//}
//
//void OptVariableResult::setFinalValues(int iScan, const std::vector<double> & finalValues_)
//{
//    if(iScan<finalValues.size())
//    {
//        finalValues.at(iScan).clear();
//        computedPoints.at(iScan).clear();
//        finalValues.at(iScan).reserve(finalValues_.size());
//        computedPoints.at(iScan).reserve(finalValues_.size());
//
//        finalValues.push_back(finalValues_);
//
//        for(int i=0;i<finalValues_.size();i++)
//        {
//            computedPoints.at(iScan).push_back(true);
//        }
//    }
//    else
//    {
//        QString msg;
//        msg = "Error setting final values of variable \""+name()+"\" , iScan : "+QString::number(iScan) + "\n";
//        InfoSender::instance()->debug(msg);
//    }
//}
//
//void OptVariableResult::setFinalValueAtPoint(double _value,int iScan,int iPoint)
//{
//    finalValues.at(iScan).at(iPoint) = _value;
//    computedPoints.at(iScan).at(iPoint) = true;
//}
//
//
//void OptVariableResult::appendFinalValue(double _value,int iScan)
//{
//        finalValues.at(iScan).push_back(_value);
//        computedPoints.at(iScan).push_back(true);
//}
//
//void OptVariableResult::appendScanValues(std::vector<double> _values,std::vector<bool> _computedPoints)
//{
//        finalValues.push_back(_values);
//        computedPoints.push_back(_computedPoints);
//}
//
//bool OptVariableResult::isComputedPoint(int iScan, int iPoint) const
//{
//    if((iScan<computedPoints.size())
//        && (iPoint<computedPoints.at(iScan).size()))
//        return computedPoints.at(iScan).at(iPoint);
//    else
//        return false;
//}
//
//int OptVariableResult::nbPoints() const
//{
//    int size=0;
//    for(int i=0;i<finalValues.size();i++)
//        size = std::max<double>(size,finalValues.at(i).size());
//
//    return size;
//}
//
//int OptVariableResult::nbScans() const
//{
//    return finalValues.size();
//}
//
//
//void OptVariableResult::clearFinalValues()
//{
//    for(int i=0;i<finalValues.size();i++)
//        finalValues.at(i).clear();
//    finalValues.clear();
//
//    for(int i=0;i<computedPoints.size();i++)
//        computedPoints.at(i).clear();
//    computedPoints.clear();
//}

//
//
//FuzzyVariable::FuzzyVariable()
//{
//    _editableFields << FuzzyVariable::VALUE << FuzzyVariable::STATUS << FuzzyVariable::PRECSTATUS << FuzzyVariable::PRECVALUE;
//}
//
//FuzzyVariable::~FuzzyVariable()
//{
//}
//
//// Cloning function
//FuzzyVariable::FuzzyVariable(const FuzzyVariable & _var):Variable(_var)
//{
//    status = POINTDEP;
//    precStatus = PRECFIXED;
//    precValue = 0;
//}
//
//FuzzyVariable::FuzzyVariable(const Variable & _var):Variable(_var)
//{
//    status = POINTDEP;
//    precStatus = PRECFIXED;
//    precValue = 0;
//
//    _editableFields << FuzzyVariable::VALUE << FuzzyVariable::STATUS << FuzzyVariable::PRECSTATUS << FuzzyVariable::PRECVALUE;
//}
//
//FuzzyVariable::FuzzyVariable(QDomElement & domEl)
//{
//    QDomNamedNodeMap attributes = domEl.attributes();
//    QString fieldName;
//    QString fieldValue;
//
//    for(int i=0;i<attributes.count();i++)
//    {
//        fieldName = attributes.item(i).toAttr().name();
//        fieldName.replace(XMLTools::space()," ");
//        fieldValue = attributes.item(i).toAttr().value();
//        fieldValue.replace(XMLTools::space()," ");
//
//        MOItem::setFieldValue(fieldName,QVariant(fieldValue));
//    }
//
//    _editableFields << FuzzyVariable::VALUE << FuzzyVariable::STATUS << FuzzyVariable::PRECSTATUS << FuzzyVariable::PRECVALUE;
//}
//
//
//
//QVariant FuzzyVariable::getFieldValue(int ifield, int role) const
//{
//     if (!_filledFields.contains(ifield)&&(role==Qt::DisplayRole))
//        return QString("-");
//    else
//    {
//        switch (ifield)
//        {
//        case NAME :
//            return name;
//        case VALUE :
//            return value;
//        case DESCRIPTION :
//            return description;
//        case MIN :
//            return min;
//        case MAX :
//            return max;
//        case TYPE :
//            return type;
//        case CATEGORY :
//            return category;
//        case DATATYPE :
//            return dataType;
//        case STATUS :
//            return status;
//        case PRECSTATUS :
//            return precStatus;
//        case PRECVALUE :
//            return precValue;
//        default :
//            return "unknown field";
//        }
//    }
//}
//
//QString FuzzyVariable::sFieldName(int ifield, int role)
//{
//    switch (ifield)
//    {
//        case NAME :
//            return "Name";
//        case VALUE :
//            return "Value";
//        case DESCRIPTION :
//            return "Description";
//        case MIN :
//            return "Min";
//        case MAX :
//            return "Max";
//        case TYPE :
//            return "Type";
//        case CATEGORY :
//            return "Category";
//        case DATATYPE:
//            return "Data type";
//        case STATUS:
//            return "Status";
//        case PRECSTATUS:
//            return "Prec. Stat.";
//        case PRECVALUE:
//            return "Prec. Val.";
//        default :
//            return "unknown field";
//    }
//}
//
//
//
//bool FuzzyVariable::setFieldValue(int ifield,QVariant value_)
//{
//    try{
//    switch (ifield)
//    {
//        case NAME :
//            name=value_.toString();
//            break;
//        case VALUE :
//            value=value_.toDouble();
//            break;
//        case DESCRIPTION :
//            description = value_.toString();
//            break;
//        case MIN :
//            min=value_.toDouble();
//            break;
//        case MAX :
//            max=value_.toDouble();
//            break;
//        case TYPE :
//            type=value_.toInt();
//            break;
//        case CATEGORY :
//            category=value_.toInt();
//            break;
//        case DATATYPE :
//            dataType=value_.toInt();
//            break;
//        case STATUS :
//            status=value_.toInt();
//            break;
//        case PRECSTATUS :
//            precStatus = value_.toInt();
//            break;
//        case PRECVALUE :
//            precValue = value_.toDouble();
//            break;
//    }
//    if(!_filledFields.contains(ifield))
//        _filledFields.push_back(ifield);
//    return true;
//    }
//    catch(std::exception &e)
//    {
//        return false;
//    }
//}
