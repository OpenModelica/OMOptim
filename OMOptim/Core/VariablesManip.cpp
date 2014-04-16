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

     @file VariablesManip.cpp
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version

  */
#include "VariablesManip.h"
#include <math.h>

VariablesManip::VariablesManip(void)
{
}

VariablesManip::~VariablesManip(void)
{
}


/** Update values of vars : following indexes indicated in iScan, values are calculated from ScannedVariables
*   using value = min + iScan * step for each variable.
*/
void VariablesManip::updateScanValues(Variables *vars, ScannedVariables *scannedVars,QList<int> iScan)
{
    int iv,iov;
    double curMin,curStep,curValue;
    QString curName;
    Variable* var;

    for(iov=0;iov<scannedVars->size();iov++)
    {
        curName = scannedVars->at(iov)->name();
        var=vars->findItem(curName);

        if(var)
        {
            curMin = scannedVars->at(iov)->getFieldValue(ScannedVariable::SCANMIN).toDouble();
            curStep = scannedVars->at(iov)->getFieldValue(ScannedVariable::SCANSTEP).toDouble();
            curValue = curMin + iScan.at(iov) * curStep;

            var->setFieldValue(Variable::VALUE,curValue);
        }
        else
        {
            QString msg;
            msg.sprintf("in updateScanValues(), unable to find variable %s",curName.utf16());
            InfoSender::instance()->debug(msg);
        }
    }
}

int VariablesManip::nbScans(ScannedVariables *scannedVars)
{
    int nbScans = 1;
    for(int i=0;i<scannedVars->size();i++)
        nbScans = nbScans*scannedVars->at(i)->nbScans();

    return nbScans;
}

double VariablesManip::calculateObjValue(OptObjective* optObj,MOOptVector * oneSimFinalVars,bool & ok,int iPoint)
{
    VariableResult* varObj = oneSimFinalVars->findItem(optObj->name());
    ok = false;
    double result;
    if(!varObj)
    {
        InfoSender::instance()->send(Info("Could not find variable "+optObj->name()+". Setting value to 0",ListInfo::WARNING2));
        ok = false;
        return 0;
    }
    else
    {
        switch(optObj->scanFunction())
        {
        case OptObjective::SUM :
            result = VariablesManip::calculateScanSum(varObj,ok,iPoint);
            break;
        case OptObjective::AVERAGE :
            result = VariablesManip::calculateScanAverage(varObj,ok,iPoint);
            break;
        case OptObjective::DEVIATION :
            result = VariablesManip::calculateScanStandardDev(varObj,ok,iPoint);
            break;
        case OptObjective::MINIMUM :
            result = VariablesManip::extractMinimum(varObj,ok,iPoint);
            break;
        case OptObjective::MAXIMUM :
            result = VariablesManip::extractMaximum(varObj,ok,iPoint);
            break;
        default :
            result = varObj->finalValue(0,iPoint);
            ok=true;
            break;
        }
    }

    if(ok && (result>=optObj->min())
            &&(result<=optObj->max()))
        return result;
    else
    {
        ok = false;
        return 0;
    }
}

double VariablesManip::calculateScanSum(VariableResult *var,bool &ok, int iPoint)
{
    int nbScans = var->nbScans();
    if(nbScans==0)
    {
        ok=false;
        return 0;
    }

    double sum = 0;
    for(int iScan=0;iScan<nbScans;iScan++)
    {
        sum += var->finalValue(iScan,iPoint);
    }
    ok=true;
    return sum;
}
double VariablesManip::calculateScanAverage(VariableResult* var,bool &ok, int iPoint)
{
    int nbScans = var->nbScans();
    if(nbScans==0)
    {
        ok=false;
        return 0;
    }

    double sum = calculateScanSum(var,ok,iPoint);
    return sum/nbScans;
}

double VariablesManip::calculateScanStandardDev(VariableResult* var,bool &ok, int iPoint)
{
    double avg = calculateScanAverage(var,ok,iPoint);
    int nbScans = var->nbScans();
    if(nbScans==0)
    {
        ok=false;
        return 0;
    }

    double variance = 0;
    for(int iScan=0;iScan<nbScans;iScan++)
    {
        variance += pow(var->finalValue(iScan,iPoint)-avg,2);
    }
    variance = variance / nbScans;
    double stdDev = sqrt(variance);
    return stdDev;
}

double VariablesManip::extractMinimum(VariableResult* var,bool &ok, int iPoint)
{
    int nbScans = var->nbScans();
    double min;
    if(nbScans==0)
    {
        ok=false;
        return 0;
    }
    else
    {
        min = var->finalValue(0,iPoint);
    }

    for(int iScan=1;iScan<nbScans;iScan++)
    {
        min += std::min(min,var->finalValue(iScan,iPoint));
    }
    ok=true;
    return min;
}


double VariablesManip::extractMaximum(VariableResult* var,bool &ok, int iPoint)
{
    int nbScans = var->nbScans();
    double max;
    if(nbScans==0)
    {
        ok=false;
        return 0;
    }
    else
    {
        max = var->finalValue(0,iPoint);
    }

    for(int iScan=1;iScan<nbScans;iScan++)
    {
        max += std::max(max,var->finalValue(iScan,iPoint));
    }
    ok=true;
    return max;
}

