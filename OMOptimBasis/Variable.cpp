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

  @file Variable.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version

  */
#include "Variable.h"
#include "CSVBase.h"
#include "LowTools.h"

Variable::Variable(void)
{
    _causality = OMUNKNOWN;
    _protectedFields << Variable::NAME << Variable::DESCRIPTION << Variable::MODEL << DATATYPE ;
    // qDebug(QString("New "+getClassName()).toLatin1().data());
}

Variable::Variable(QString name)
{
    _causality = OMUNKNOWN;
    _protectedFields << Variable::NAME << Variable::DESCRIPTION << Variable::MODEL << DATATYPE ;
    setName(name);
    // qDebug(QString("New "+getClassName()).toLatin1().data());
}

Variable::Variable(const Variable & var):MOItem(var)
{
    QVariant curValue;
    for(int i=0;i<nbFields;i++)
    {
        curValue = var.getFieldValue(i);
        setFieldValue(i,curValue);
    }
    // qDebug(QString("New "+getClassName()).toLatin1().data());
}

Variable::Variable(QDomElement & domEl)
{
    _protectedFields << Variable::NAME << Variable::DESCRIPTION << Variable::MODEL << DATATYPE ;

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
    //  qDebug(QString("New "+getClassName()).toLatin1().data());
}

Variable::~Variable(void)
{
    // qDebug(QString("Remove "+getClassName()).toLatin1().data());
}

Variable &Variable::operator =(const Variable & var)
{
    QVariant curValue;
    for(int i=0;i<nbFields;i++)
    {
        curValue = var.getFieldValue(i);
        setFieldValue(i,curValue);
    }
    _filledFields = var._filledFields;
    _protectedFields = var._protectedFields;
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

void Variable::setValue(double value)
{
    if(!_filledFields.contains(Variable::VALUE))
        _filledFields.push_back(Variable::VALUE);
    _value = value;
}

void Variable::setValue(QVariant value)
{
    if(!_filledFields.contains(Variable::VALUE))
        _filledFields.push_back(Variable::VALUE);
    _value = value;
}

double Variable::doubleValue(bool *ok) const
{
    return _value.toDouble(ok);
}

QVariant Variable::value() const
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

VariableCausality Variable::causality()
{
    return _causality;
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
//            if(isBoolean())
//                return QVariant((bool)_value);
//            else
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
                case OMINPUT :
                    return "Input";
                case OMOUTPUT :
                    return "Output";
                case OMUNKNOWN:
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
            _value = value;
//            if(isBoolean())
//                _value = value.toBool();
//            else
//                _value=value.toDouble();
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

bool Variable::isBoolean() const
{
    return _dataType == OMBOOLEAN;
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
    for(int i=0;i<VariableResult::nbFields;i++)
    {
        setIsProtectedField(i,true);
    }
    // qDebug(QString("New "+getClassName()).toLatin1().data());
}

VariableResult::VariableResult(const VariableResult& var):Variable(var)
{
    _finalValues = var._finalValues;
    _computedPoints = var._computedPoints;
    // qDebug(QString("New "+getClassName()).toLatin1().data());
}

VariableResult::VariableResult(const Variable& var):Variable(var)
{
    setFinalValue(0,0,doubleValue());
    // qDebug(QString("New "+getClassName()).toLatin1().data());
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
            this->updateValuesFromCsv(domValues.text());
        }
    // qDebug(QString("New "+getClassName()).toLatin1().data());
}

Variable VariableResult::extractPoint(int iPoint, int iScan)
{
    Variable result(*this);
    result.setValue(this->finalValue(iScan,iPoint));
    return result;
}



VariableResult::~VariableResult()
{
    for(int i=0;i<_finalValues.size();i++)
        _finalValues[i].clear();

    for(int i=0;i<_computedPoints.size();i++)
        _computedPoints[i].clear();

    // qDebug(QString("Remove "+getClassName()).toLatin1().data());
}

VariableResult* VariableResult::clone() const
{
    return new VariableResult(*this);
}

QVector<double> VariableResult::finalValues(int iScan) const
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

QVector<double> VariableResult::finalValuesAtPoint(int iPoint)
{
    QVector<double> result;
    for(int i=0;i<nbScans();i++)
        result.push_back(finalValue(i,iPoint));

    return result;
}
QVector<double> VariableResult::finalValuesAtScan(int iScan)
{
    if(iScan<_finalValues.size())
        return _finalValues.at(iScan);
    else
        return QVector<double>();
}



void VariableResult::setFinalValuesAtScan(int iScan,const QVector<double> & finalValues)
{
    if(iScan<finalValues.size())
    {
        _finalValues.replace(iScan,finalValues);
        _computedPoints.replace(iScan,QVector<bool>(finalValues.size(),true));
    }
    else
    {
        QString msg;
        msg = "Error setting final values of variable \""+name()+"\" , iScan : "+QString::number(iScan) + "\n";
        InfoSender::instance()->debug(msg);
    }
}

void VariableResult::setFinalValuesAtPoint(int iPoint,const QVector<double> &values)
{
    // resize if needed
    // scans (must be done before points)
    QVector<double> defaultValues(nbPoints(),0);
    QVector<bool> defaultComputed(nbPoints(),false);

    while(values.size()>_finalValues.size())
    {
        _finalValues.push_back(defaultValues);
        _computedPoints.push_back(defaultComputed);
    }
    // points
    for(int i=0;i<_finalValues.size();i++)
    {
        while(iPoint>=_finalValues.at(i).size())
        {
            _finalValues[i].push_back(0);
            _computedPoints[i].push_back(false);
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
    QVector<double> defaultValues(nbPoints(),0);
    QVector<bool> defaultComputed(nbPoints(),false);

    while(iScan>=_finalValues.size())
    {
        _finalValues.push_back(defaultValues);
        _computedPoints.push_back(defaultComputed);
    }
    // points

    for(int i=0;i<_finalValues.size();i++)
    {
        while(iPoint>=_finalValues[i].size())
        {
            _finalValues[i].push_back(0);
            _computedPoints[i].push_back(false);
        }
    }

    try{
        _finalValues[iScan][iPoint] = value;
        _computedPoints[iScan][iPoint] = computed;
    }
    catch(std::exception &e)
    {
        int a=2;
    }
}

//void VariableResult::appendFinalValue(double value,int iScan)
//{
//    if(iScan==_finalValues.size())
//    {
//        _finalValues.push_back(QVector<double>());
//        _computedPoints.push_back(QVector<bool>());
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

//void VariableResult::appendScanValues(QVector<double> values,QVector<bool> computedPoints)
//{
//    _finalValues.push_back(values);
//    _computedPoints.push_back(computedPoints);
//}

//void VariableResult::appendScanValue(double value,bool computedPoint)
//{
//    QVector<double> values;
//    QVector<bool> computedPoints;
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
        _finalValues[i].clear();
    _finalValues.clear();

    for(int i=0;i<_computedPoints.size();i++)
        _computedPoints[i].clear();
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
    QDomText text = doc.createTextNode(this->valuesToCSV());
    values.appendChild(text);
    cItem.appendChild(values);

    return cItem;
}


void VariableResult::updateValuesFromCsv(QString text)
{
    QStringList  scanLines = text.split("\n",QString::SkipEmptyParts);
    QStringList curLine;
    int iPoint=0;
    bool ok;
    double value;

    for (int iScan = 0; iScan<scanLines.size(); iScan++)
    {
        curLine = scanLines[iScan].split("\t",QString::SkipEmptyParts);

        for (int iCol = 0; iCol < curLine.size(); iCol++)
        {
            value = curLine[iCol].toDouble(&ok);
            if(ok)
                this->setFinalValue(iScan,iCol,value);
        }
        iPoint++;
    }
}

QString VariableResult::valuesToCSV()
{
    QString csv;
    double value;

    QStringList scanTexts;

    csv += "\n";

    // writing values
    for(int iScan = 0;iScan<this->nbScans();iScan++)
    {
        scanTexts.push_back(QString());
        for(int iPoint = 0; iPoint < nbPoints(); iPoint++)
        {
            if (this->isComputedPoint(0,iPoint))
            {
                value = this->finalValue(iScan,iPoint);
                scanTexts[iScan] += QString::number(value);
            }
            else
            {
                scanTexts[iScan] += "-";
            }
            scanTexts[iScan] += "\t";
        }
        scanTexts[iScan] += "\n";
    }

    // writing grouped by scans
    for(int iScan = 0;iScan<this->nbScans();iScan++)
    {
        csv+=scanTexts[iScan];
        csv+="\n";
    }
    return csv;
}



OptVariable::OptVariable()
{
    setEditableFields(QList<int>()<< OptVariable::VALUE << OptVariable::OPTMIN << OptVariable::OPTMAX);

    // qDebug(QString("New "+getClassName()).toLatin1().data());
}

OptVariable::OptVariable(QString name)
{
    setEditableFields(QList<int>()<< OptVariable::VALUE << OptVariable::OPTMIN << OptVariable::OPTMAX);
    setName(name);
    // qDebug(QString("New "+getClassName()).toLatin1().data());
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
    setEditableFields(QList<int>()<< OptVariable::VALUE << OptVariable::OPTMIN << OptVariable::OPTMAX);
    // qDebug(QString("New "+getClassName()).toLatin1().data());
}

OptVariable::~OptVariable()
{
    // qDebug(QString("Remove "+getClassName()).toLatin1().data());
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
    //_editableFields << OptVariable::VALUE << OptVariable::OPTMIN << OptVariable::OPTMAX;
    // qDebug(QString("New "+getClassName()).toLatin1().data());
}

OptVariable::OptVariable(const OptVariable & var):Variable(var)
{
    _optMin = var._optMin;
    _optMax = var._optMax;
    // qDebug(QString("New "+getClassName()).toLatin1().data());
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
                case OMINPUT :
                    return "Input";
                case OMOUTPUT :
                    return "Output";
                case OMUNKNOWN:
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
    setEditableFields(QList<int>()<<  ScannedVariable::VALUE << ScannedVariable::SCANMIN << ScannedVariable::SCANMAX << ScannedVariable::SCANSTEP);
    //_editableFields << ScannedVariable::VALUE << ScannedVariable::SCANMIN << ScannedVariable::SCANMAX << ScannedVariable::SCANSTEP;
    // qDebug(QString("New "+getClassName()).toLatin1().data());
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
    setEditableFields(QList<int>()<<  ScannedVariable::VALUE << ScannedVariable::SCANMIN << ScannedVariable::SCANMAX << ScannedVariable::SCANSTEP);
    // qDebug(QString("New "+getClassName()).toLatin1().data());
}

ScannedVariable::~ScannedVariable()
{
    // qDebug(QString("Remove "+getClassName()).toLatin1().data());
}


ScannedVariable::ScannedVariable(const ScannedVariable & var):Variable(var)
{
    _scanMin = var._scanMin;
    _scanMax = var._scanMax;
    _scanStep = var._scanStep;
    // qDebug(QString("New "+getClassName()).toLatin1().data());
}

ScannedVariable* ScannedVariable::clone() const
{
    return new ScannedVariable(*this);
}


ScannedVariable::ScannedVariable(const Variable & var):Variable(var)
{
    initScanExtremum();

    setEditableFields(QList<int>()<<  ScannedVariable::VALUE << ScannedVariable::SCANMIN << ScannedVariable::SCANMAX << ScannedVariable::SCANSTEP);

    // qDebug(QString("New "+getClassName()).toLatin1().data());
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
    int i = 0;
    if(_scanStep>0)
    {
        double d = _scanMax - _scanMin;
        double s = d / _scanStep;
        // fprintf(stderr, "%g - %g = %g", _scanMax, _scanMin, d);
        // fprintf(stderr, "%g / %g = %g", d, _scanStep, s);
        i = s + 1;
        if (i < 0)
          return 0;
        else
          return i;
    }
    else
        return i;
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
                case OMINPUT :
                    return "Input";
                case OMOUTPUT :
                    return "Output";
                case OMUNKNOWN :
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
//QVector<double> OptVariableResult::finalValues(int iScan) const
//{
//    return finalValues.at(iScan);
//}
//
//double OptVariableResult::finalValue(int iScan,int iPoint) const
//{
//    return finalValues.at(iScan).at(iPoint);
//}
//
//void OptVariableResult::setFinalValues(int iScan, const QVector<double> & finalValues_)
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
//void OptVariableResult::appendScanValues(QVector<double> _values,QVector<bool> _computedPoints)
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
