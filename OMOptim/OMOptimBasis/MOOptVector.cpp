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

  @file MOOptVector.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version

  */
#include "MOOptVector.h"


MOOptVector::MOOptVector(bool owner,bool useScan, bool usePoints)
    :MOVector<VariableResult>(owner)
{
    _useScan = useScan;
    _usePoints = usePoints;

    _curScan=0;
    _curPoint=0;

    _nbScans = 0;
    _nbPoints = 0;


    connect(this,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SIGNAL(modified()));
}



int MOOptVector::nbPoints()
{
    if(items.size()==0)
        return 0;
    else
    {
        int maxNbPoints = 0;
        for(int i=0;i<items.size();i++)
        {
            maxNbPoints = std::max<int>(maxNbPoints,items.at(i)->nbPoints());
        }
        return maxNbPoints;
    }
}

int MOOptVector::nbScans()
{
    if(items.size()==0)
        return 0;
    else
    {
        int maxNbScans = 0;
        for(int i=0;i<items.size();i++)
        {
            maxNbScans = std::max<int>(maxNbScans,items.at(i)->nbScans());
        }
        return maxNbScans;
    }
}


void MOOptVector::setCurPoint(int curPoint)
{
    if(_usePoints && (curPoint!=_curPoint))
    {
        _curPoint = curPoint;
        emit curScanChanged();
        reset();
    }
}


void MOOptVector::setCurScan(int curScan)
{
    if(_useScan && (curScan!=_curScan))
    {
        _curScan = curScan;
        emit curScanChanged();
        reset();
    }
}



QVariant MOOptVector::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {

        if(index.column()==VariableResult::VALUE)
        {
            if(!isAvailablePoint(index.row(),((int)_useScan)*_curScan,((int)_usePoints)*_curPoint))  // several,none or non-calculated point selected
            {
                return QString("-");
            }
            else
            {
                return items.at(index.row())->finalValue(((int)_useScan)*_curScan,((int)_usePoints)*_curPoint);
            }
        }
        else
        {

            return items.at(index.row())->getFieldValue(index.column(),role);
        }
    }
    return QVariant();
}


bool MOOptVector::isAvailablePoint(int iVar,int iScan,int iPoint) const
{
    int nbPoints = items.at(iVar)->nbPoints();
    int nbScans = items.at(iVar)->nbScans();
    if(
            (iVar<0) || (iVar>=items.size())
            || (iPoint<0) || (iPoint>=nbPoints)
            || (iScan<0) || (iScan>=nbScans))
        return false;
    else
    {
        bool isComputed = items.at(iVar)->isComputedPoint(iScan,iPoint);
        return isComputed;
    }
}

MOOptVector* MOOptVector::clone() const
{

    MOOptVector* newVector = new MOOptVector(_owner,_useScan,_usePoints);

    int i;
    VariableResult* newItem;
    for(i=0;i<items.size();i++)
    {
        newItem = new VariableResult(*items.at(i));
        newVector->addItem(newItem);
    }

    newVector->_curPoint = _curPoint;
    newVector->_curScan = _curScan;
    newVector->_usePoints = _usePoints;
    newVector->_useScan = _useScan;

    return newVector;
}
double MOOptVector::value(const QString &name,bool &ok,int iScan,int iPoint)
{
    int iVar = this->findItem(name);
    if(iVar==-1)
    {
        ok = false;
        return 0;
    }

    return this->at(iVar)->value();

}

void MOOptVector::append(const MOOptVector &vector,bool makeACopy)
{
    //    int iCurItem;
    for(int i=0;i<vector.items.size();i++)
    {

        //        iCurItem = findItem(vector.items.at(i)->name());
        //        if(iCurItem==-1)
        //        {
        if(makeACopy)
            addItem(new VariableResult(*vector.items.at(i)));
        else
            addItem(vector.items.at(i));
        //        }
        //        else
        //        {
        //            InfoSender::instance()->debug("replace item in vector (name : "+vector.items.at(i)->name()+")");
        //            delete items.at(iCurItem);
        //            if(makeACopy)
        //                items.replace(iCurItem,new VariableResult(*vector.items.at(i)));
        //            else
        //                items.replace(iCurItem,vector.items.at(i));
        //        }
    }
}

void MOOptVector::clearAtiPoint(int iPoint)
{
    for(int iVar=0;iVar<items.size();iVar++)
    {
        items.at(iVar)->clearFinalValuesAtIpoint(iPoint);
    }
}

void MOOptVector::setUsePoints(bool use)
{
    if(_usePoints!=use)
    {
        _usePoints = use;
        emit usePointsChanged();
    }
}


void MOOptVector::setUseScan(bool use)
{
    if(_useScan!=use)
    {
        _useScan = use;
        emit useScanChanged();
    }
}


bool MOOptVector::getUsePoints()
{
    return _usePoints;
}


bool MOOptVector::getUseScan()
{
    return _useScan;
}


int MOOptVector::curScan()
{
    return _curScan;
}

int MOOptVector::curPoint()
{
    return _curPoint;
}

void MOOptVector::addItem(VariableResult* item)
{
    // Add an item pointer in Vector
    int index=items.size();
    insertRow(index);//,createIndex(0,0));
    beginInsertRows(QModelIndex(),index,index);
    items.push_back(item);
    endInsertRows();
}

VariableResult *MOOptVector::findVariable(QString model, QString shortVarName)
{
    int iVar=0;
    bool found = 0;
    while(!found && (iVar<this->size()))
    {
        found = ((this->at(iVar)->name(Variable::SHORT)==shortVarName) && (this->at(iVar)->model()==model));
        if(!found)
            iVar++;

    }
    if(found)
        return this->at(iVar);
    else
        return NULL;

}

QString MOOptVector::toCSV(int iPoint)
{
    QString csv;
    VariableResult* var;
    for(int iV=0;iV<size();iV++)
    {
        var = items.at(iV);
        csv+=var->name()+"\t";
    }
    csv+="\n";
    for(int iS=0;iS<nbScans();iS++)
    {
        for(int iV=0;iV<size();iV++)
        {
            var = items.at(iV);
            csv+=QString::number(var->finalValue(iS,iPoint))+"\t";
        }
        csv+="\n";
    }
    return csv;

}
