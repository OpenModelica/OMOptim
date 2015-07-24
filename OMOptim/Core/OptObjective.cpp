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

    @file OptObjective.cpp
    @brief Comments for file documentation.
    @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
    Company : CEP - ARMINES (France)
    http://www-cep.ensmp.fr/english/
    @version

  */

#include "OptObjective.h"
#include <float.h>

OptObjective::OptObjective()
{
    QList<int> protectedFields;
    protectedFields << OptObjective::NAME << OptObjective::DESCRIPTION << OptObjective::MODEL;
    setProtectedFields(protectedFields);

    _scanFunction = NONE;
    initOptExtremum();
}
OptObjective::OptObjective(const Variable &var, OptObjective::Direction direction):Variable(var)
{
    // problem: fields doesnt have proper correspondence
    // filledFields is not consistent after Variable(var)
    for(int i=0;i<this->getNbFields();i++)
        if(_filledFields.contains(i))
            _filledFields.push_back(i);

    setFieldValue(OptObjective::DIRECTION,direction);
    _scanFunction = NONE;
    QList<int> protectedFields;
    protectedFields << OptObjective::NAME << OptObjective::DESCRIPTION << OptObjective::MODEL;
    setProtectedFields(protectedFields);
    initOptExtremum();
}

OptObjective::OptObjective(const OptObjective & obj):Variable(obj)
{
    _scanFunction = obj._scanFunction;
    _direction = obj._direction;
    _min = obj._min;
    _max = obj._max;
    _filledFields = obj._filledFields;
}

OptObjective::OptObjective(QDomElement & domEl)
{

    QList<int> protectedFields;
    protectedFields << OptObjective::NAME << OptObjective::DESCRIPTION << OptObjective::MODEL;
    setProtectedFields(protectedFields);
    _scanFunction = NONE;

    initOptExtremum();

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


OptObjective::~OptObjective(void)
{
}

OptObjective *OptObjective::clone() const
{
    return new OptObjective(*this);
}

void OptObjective::initOptExtremum()
{
    _min= -std::numeric_limits<double>::max();
    _max = std::numeric_limits<double>::max();
}

void OptObjective::setScanFunction(ScanFunction scanFunction)
{
    _scanFunction = scanFunction;

    if(!_filledFields.contains(OptObjective::SAMPLINGFUNCTION))
        _filledFields.append(OptObjective::SAMPLINGFUNCTION);
}

void OptObjective::setDirection(Direction dir)
{
    _direction = dir;

    if(!_filledFields.contains(OptObjective::DIRECTION))
        _filledFields.append(OptObjective::DIRECTION);
}

void OptObjective::setDescription(QString description)
{
    if(!_filledFields.contains(OptObjective::DESCRIPTION))
        _filledFields.push_back(OptObjective::DESCRIPTION);
    _description = description;
}


OptObjective::Direction OptObjective::direction()
{
    return _direction;
}

OptObjective::ScanFunction OptObjective::scanFunction()
{
    return _scanFunction;
}

bool OptObjective::check(QString &error)
{
    bool ok=true;
    error.clear();
    if(_min>=_max)
    {
        error.sprintf("Objective min value >= max value (%f >= %f)",_min,_max);
        ok=false;
    }

    //if(type==FIXED)
    //{
    //  error.sprintf("Objective value is considered as a model input. Its value probably won't be affected by variables modification");
    //  ok = false;
    //}

    return ok;
}

bool OptObjective::isMinimized()
{
    return _direction == MINIMIZE;
}
QString OptObjective::sFieldName(int ifield, int role)
{


    switch (ifield)
    {
    case NAME :
        return "Name";
    case MODEL :
        return "Model";
    case DESCRIPTION :
        return "Description";
    case DIRECTION :
        return "Direction";
    case MIN :
        return "Minimum";
    case MAX :
        return "Maximum";
    case SAMPLINGFUNCTION :
        return "Sampling Function";
    default :
        return "unknown field";
    }
}

QVariant OptObjective::getFieldValue(int ifield, int role) const
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
        case DESCRIPTION :
            return _description;
        case DIRECTION :
            if(role == Qt::DisplayRole)
            {
                if(_direction==OptObjective::MAXIMIZE)
                    return "Maximize";
                else
                    return "Minimize";
            }
            else
                return _direction;
        case MIN :
            if((role == Qt::DisplayRole)&&(_min==-std::numeric_limits<double>::max()))
                return "-";
            else
                return _min;
        case MAX:
            if((role == Qt::DisplayRole)&&(_max==std::numeric_limits<double>::max()))
                return "-";
            else
                return _max;
        case SAMPLINGFUNCTION :
            if(role == Qt::DisplayRole)
            {
                switch(_scanFunction)
                {
                case NONE :
                    return "None";
                case AVERAGE :
                    return "Average";
                case SUM :
                    return "Sum";
                case DEVIATION :
                    return "Standard deviation";
                case MINIMUM :
                    return "Minimum";
                case MAXIMUM :
                    return "Maximum";
                default :
                    return "-";
                }
            }
            else
                return _scanFunction;
        default :
            return "unknown field";
        }
    }
}

bool OptObjective::setFieldValue(int ifield,QVariant value)
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
        case DESCRIPTION :
            _description=value.toString();
            break;
        case DIRECTION :
            _direction= (Direction)value.toInt();
            break;
        case MIN :
            _min = value.toDouble(&isDouble);
            if(!isDouble)
                _min = -std::numeric_limits<double>::max();
            break;
        case MAX :
            _max = value.toDouble(&isDouble);
            if(!isDouble)
                _max = std::numeric_limits<double>::max();
            break;
        case SAMPLINGFUNCTION :
            _scanFunction = (ScanFunction)value.toInt();
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


OptObjectiveResult::OptObjectiveResult():OptObjective()
{
}

OptObjectiveResult::OptObjectiveResult(const OptObjective & optObj):OptObjective(optObj)
{
}

OptObjectiveResult::OptObjectiveResult(QDomElement & domEl):OptObjective()
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

QString OptObjectiveResult::sFieldName(int ifield, int role)
{
    switch (ifield)
    {
    case NAME :
        return "Name";
    case MODEL :
        return "Model";
    case DESCRIPTION :
        return "Description";
    case DIRECTION :
        return "Direction";
    case MIN:
        return "Minimum";
    case MAX:
        return "Maximum";
    case SAMPLINGFUNCTION:
        return "Scan Function";
    default :
        return "unknown field";
    }
}

QVariant OptObjectiveResult::getFieldValue(int ifield, int role) const
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
        case DESCRIPTION :
            return _description;
        case DIRECTION :
            if(role == Qt::DisplayRole)
            {
                if(_direction==OptObjective::MAXIMIZE)
                    return "Maximize";
                else
                    return "Minimize";
            }
            else
                return _direction;
        case MIN :
            if((role == Qt::DisplayRole)&&(_min==-std::numeric_limits<double>::max()))
                return "-";
            else
                return _min;
        case MAX :
            if((role == Qt::DisplayRole)&&(_max==std::numeric_limits<double>::max()))
                return "-";
            else
                return _max;
        case SAMPLINGFUNCTION :
            if(role == Qt::DisplayRole)
            {
                switch(_scanFunction)
                {
                case NONE :
                    return "None";
                case AVERAGE :
                    return "Average";
                case SUM :
                    return "Sum";
                case DEVIATION :
                    return "Deviation";
                default :
                    return "-";
                }
            }
            else
                return _scanFunction;
        default :
            return "unknown field";
        }
    }
}

bool OptObjectiveResult::setFieldValue(int ifield,QVariant value)
{
    bool isDouble;
    switch (ifield)
    {
    case NAME :
        _name=value.toString();
        break;
    case MODEL :
        _model=value.toString();
        break;
    case DESCRIPTION :
        _description=value.toString();
        break;
    case DIRECTION :
        _direction= (Direction)value.toInt();
        break;
    case MIN :
        _min = value.toDouble(&isDouble);
        if(!isDouble)
            _min = -std::numeric_limits<double>::max();
        break;
    case MAX:
        _max = value.toDouble(&isDouble);
        if(!isDouble)
            _max = std::numeric_limits<double>::max();
        break;
    case SAMPLINGFUNCTION :
        _scanFunction= (ScanFunction)value.toInt();
        break;
    }
    if(!_filledFields.contains(ifield))
        _filledFields.push_back(ifield);
    return true;
}

std::vector<double> OptObjectiveResult::finalValues() const
{
    return _finalValues;
}


void OptObjectiveResult::setFinalValues(const std::vector<double> & finalValues)
{

    _finalValues.clear();
    _computedPoints.clear();
    _finalValues.reserve(finalValues.size());

    for(int i=0;i<finalValues.size();i++)
    {
        _finalValues.push_back(finalValues.at(i));
        _computedPoints.push_back(true);
    }
}

void OptObjectiveResult::setFinalValueAtPoint(double newvalue,int index)
{
    if(index>=_finalValues.size())
    {
        _computedPoints.resize(index+1,false);
        _finalValues.resize(index+1,-1);
    }

    _finalValues.at(index)=newvalue;
    _computedPoints.at(index)=true;
}

//void OptObjectiveResult::appendFinalValue(double value)
//{
//    _finalValues.push_back(value);
//    _computedPoints.push_back(true);
//}

bool OptObjectiveResult::isComputedPoint(int i) const
{
    if(i<_computedPoints.size())
        return _computedPoints.at(i);
    else
        return false;
}

double OptObjectiveResult::finalValue(int point) const
{
    return _finalValues.at(point);
}

int OptObjectiveResult::nbPoints() const
{
    return _finalValues.size();
}

void OptObjectiveResult::clearFinalValues()
{
    _finalValues.clear();
    _computedPoints.clear();
}

OptObjective* OptObjectiveResult::equivOptObjective()
{
    OptObjective* newVar = new OptObjective();

    // copying fields
    for(int i=0;i<OptObjective::nbFields;i++)
    {
        newVar->setFieldValue(i,getFieldValue(i));
    }

    return newVar;
}

OptObjectiveResult* OptObjectiveResult::clone() const
{
    OptObjectiveResult* newObj = new OptObjectiveResult();

    // copying fields
    for(int i=0;i<nbFields;i++)
    {
        newObj->setFieldValue(i,getFieldValue(i));
    }
    newObj->_filledFields = _filledFields;
    newObj->_protectedFields = _protectedFields;

    //copying final values
    newObj->setFinalValues(_finalValues);

    return newObj;
}

