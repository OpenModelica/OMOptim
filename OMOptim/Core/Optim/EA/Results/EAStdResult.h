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

  @file EAStdResult.h
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version 0.9

  */
#ifndef _EASTDRESULT_H
#define _EASTDRESULT_H


#include <stdio.h>
#include <eo>
#include <moeo>
#include <es/eoRealInitBounded.h>
#include <es/eoRealOp.h>
#include <cmath>

#include "VariableType.h"
#include "OptimResult.h"
#include "MOParameter.h"
#include "Problem.h"
#include "Optimization.h"
#include "EAStdOptimizationEval.h"


template<class EOT> class EAStdResult
{
public :
    inline static OptimResult* buildOptimResult(Project*,Optimization* ,QList<BlockSubstitutions*>,
                                                moeoUnboundedArchive<EOT> & arch);
};

template<class EOT>
OptimResult* EAStdResult<EOT>::buildOptimResult(Project* project,Optimization* problem,QList<BlockSubstitutions*> subBlocks,
                                                moeoUnboundedArchive<EOT> & arch)
{
    if(arch.size()==0)
        return NULL;

    OptimResult *result = new OptimResult(project,problem->modModelPlus(),*problem,problem->getCurAlgo()->clone());
    result->setName(problem->name()+" result");
    result->_subBlocks = subBlocks;

    int nbScans = VariablesManip::nbScans(problem->scannedVariables());
    bool useScan = (nbScans>1);

    //first define optVariableResult from optVariables
    VariableResult *curOptVarRes;
    int nbOptVar = problem->optimizedVariables()->items.size();
    for (int iOptVar = 0; iOptVar < nbOptVar; iOptVar++)
    {
        curOptVarRes = new VariableResult(*problem->optimizedVariables()->items.at(iOptVar));
        result->optVariablesResults()->addItem(curOptVarRes);
    }

    //first define optObjectiveResult from optObjective
    VariableResult *curOptObjRes;
    int nbOptObj = problem->objectives()->items.size();
    for (int iOptObj = 0; iOptObj < nbOptObj; iOptObj++)
    {
        curOptObjRes = new VariableResult(*problem->objectives()->items.at(iOptObj));
        result->optObjectivesResults()->addItem(curOptObjRes);
    }

    // ***********************************************************
    // Filling and Sizing recomputed variables (without values)
    // ***********************************************************
    result->recomputedVariables()->items.clear();
    result->recomputedVariables()->setUseScan(useScan);
    VariableResult *curRecompVar;
    for (int i=0;i< problem->modModelPlus()->variables()->items.size();i++)
    {
        curRecompVar = new VariableResult(*problem->modModelPlus()->variables()->items.at(i));
        result->recomputedVariables()->addItem(curRecompVar);
    }

    // *******************************************************************
    // Filling OptVariables Values
    // and corresponding recomputedVariables if nScans == 0
    // *******************************************************************
    EOT *curResultPoint ;
    OptVariable* curOptVar;
    int iCorrRecompVar; //index of corresponding variable in result->recomputedVariable

    int iDouble=0;
    int	iInt=0;
    int	iBool=0;


    bool fillRecompValues = !useScan;

    for(int iVar=0; iVar<nbOptVar; iVar++)
    {
        curOptVar = problem->optimizedVariables()->items.at(iVar);
        iCorrRecompVar = result->recomputedVariables()->findItem(curOptVar->name());

        if(iCorrRecompVar<0)
        {
            QString msg;
            msg.sprintf("Unable to find variable %s in result file",curOptVar->name().utf16());
            InfoSender::instance()->debug(msg);
        }
        else
        {
            int dataType = curOptVar->getFieldValue(OptVariable::DATATYPE).toInt();

            //For each result point
            for(int i=0;i<arch.size();i++)
            {
                curResultPoint = &arch.at(i);

                switch(dataType)
                {
                case OMREAL:
                    result->optVariablesResults()->items[iVar]->appendFinalValue(
                                curResultPoint->doubleVars.at(iDouble),0);
                    result->recomputedVariables()->items[iCorrRecompVar]->appendFinalValue(
                                curResultPoint->doubleVars.at(iDouble),0);
                    break;
                case OMINTEGER:
                    result->optVariablesResults()->items[iVar]->appendFinalValue(
                                (double)curResultPoint->intVars.at(iInt),0);
                    result->recomputedVariables()->items[iCorrRecompVar]->appendFinalValue(
                                (double)curResultPoint->intVars.at(iInt),0);
                    break;
                case OMBOOLEAN:
                    result->optVariablesResults()->items[iVar]->appendFinalValue(
                                (double)curResultPoint->boolVars.at(iBool),0);
                    result->recomputedVariables()->items[iCorrRecompVar]->appendFinalValue(
                                (double)curResultPoint->boolVars.at(iBool),0);
                    break;
                }
            }

            // increasing index
            switch(dataType)
            {
            case OMREAL:
                iDouble++;
                break;
            case OMINTEGER:
                iInt++;
                break;
            case OMBOOLEAN:
                iBool++;
                break;
            }
        }
    }




    // *******************************************************************
    // Storing subModelChosen for each point
    // *******************************************************************
    if(subBlocks.size()>0)
    {
        for(int i=0;i<arch.size();i++)
        {
            curResultPoint = &arch.at(i);
            result->_iSubModels.push_back(curResultPoint->intVars.at(curResultPoint->intVars.size()-1));
        }
    }

    // *******************************************************************
    // Filling OptObjectives Values
    // and corresponding recomputedVariables if nScans == 0
    // *******************************************************************
    moeoRealObjectiveVector<moeoObjectiveVectorTraits> resObjVector;
    int nbObj = resObjVector.size();
    for(int iObj=0;iObj<nbObj;iObj++)
    {
        iCorrRecompVar = result->recomputedVariables()->findItem(result->optObjectivesResults()->items[iObj]->name());

        for(int i=0;i<arch.size();i++)
        {
            curResultPoint = &arch.at(i);
            resObjVector = curResultPoint->objectiveVector();
            result->optObjectivesResults()->items[iObj]->appendFinalValue(resObjVector.at(iObj),0);
            if(fillRecompValues)
                result->recomputedVariables()->items[iCorrRecompVar]->appendFinalValue(resObjVector.at(iObj),0);
        }
    }

    result->setSuccess(true);
    return result;
}

#endif
