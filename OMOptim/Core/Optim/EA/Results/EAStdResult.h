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
#include "Evaluations/EAStdOptimizationEval.h"


template<class EOT> class EAStdResult
{
public :
    inline static OptimResult* buildOptimResult(Project*,Optimization* ,/*QList<BlockSubstitutions*>,*/
                                                moeoUnboundedArchive<EOT> & arch);
};

template<class EOT>
OptimResult* EAStdResult<EOT>::buildOptimResult(Project* project,Optimization* problem,/*QList<BlockSubstitutions*> subBlocks,*/
                                                moeoUnboundedArchive<EOT> & arch)
{
    if(arch.size()==0)
        return NULL;


    OptimResult *result = new OptimResult(project,*problem);

    result->setName(problem->name()+" result");
  //  result->_subBlocks = subBlocks;

    int nbScans = VariablesManip::nbScans(problem->scannedVariables());
    bool useScan = (nbScans>1);



    // ***********************************************************
    // Filling and Sizing recomputed variables (without values)
    // ***********************************************************
    result->recomputedVariables()->clear();
    result->recomputedVariables()->setUseScan(useScan);
    VariableResult *curRecompVar;
    for(int iM=0;iM<problem->models().size();iM++)
    {
        ModelPlus* modelPlus = project->modelPlus(problem->models().at(iM));
        for (int i=0;i< modelPlus->variables()->size();i++)
        {
            curRecompVar = new VariableResult(*modelPlus->variables()->at(i));
            curRecompVar->clearFinalValues();
            result->recomputedVariables()->addItem(curRecompVar);
        }
    }

    // define optVariableResult from optVariables
    // and add in recomputedVariables if not present
    VariableResult *curOptVarRes;

    int nbOptVar = problem->optimizedVariables()->size();
    for (int iOptVar = 0; iOptVar < nbOptVar; iOptVar++)
    {
        curOptVarRes = new VariableResult(*problem->optimizedVariables()->at(iOptVar));
        curOptVarRes->clearFinalValues();
        result->optVariablesResults()->addItem(curOptVarRes);
        if(!result->recomputedVariables()->findItem(curOptVarRes->name(Variable::FULL)))
            result->recomputedVariables()->addItem(curOptVarRes->clone());
    }

    // define optObjectiveResult from optObjective
    // and add in recomputedVariables if not present
    VariableResult *curOptObjRes;
    int nbOptObj = problem->objectives()->size();
    for (int iOptObj = 0; iOptObj < nbOptObj; iOptObj++)
    {
        curOptObjRes = new VariableResult(*problem->objectives()->at(iOptObj));
        curOptObjRes->clearFinalValues();
        result->optObjectivesResults()->addItem(curOptObjRes);
        if(!result->recomputedVariables()->findItem(curOptObjRes->name(Variable::FULL)))
            result->recomputedVariables()->addItem(curOptObjRes->clone());
    }

    // and add savedVariables in recomputedVariables if not present
    int nbSavedVars= problem->savedVars()->size();
    for (int iSavVar = 0; iSavVar < nbSavedVars; iSavVar++)
    {
        curOptVarRes = new VariableResult(*problem->savedVars()->at(iSavVar));
        curOptVarRes->clearFinalValues();
        if(!result->recomputedVariables()->findItem(curOptVarRes->name(Variable::FULL)))
            result->recomputedVariables()->addItem(curOptVarRes->clone());
        delete curOptVarRes;
    }


    // *******************************************************************
    // Filling OptVariables Values
    // and corresponding recomputedVariables if nScans == 0
    // *******************************************************************
    EOT *curResultPoint ;
    OptVariable* curOptVar;
    VariableResult* corrVar;
    int iCorrRecompVar; //index of corresponding variable in result->recomputedVariable

    int iDouble=0;
    int    iInt=0;
    int    iBool=0;


    bool fillRecompValues = !useScan;


    for(int iVar=0; iVar<nbOptVar; iVar++)
    {
        curOptVar = problem->optimizedVariables()->at(iVar);
        corrVar = result->recomputedVariables()->findItem(curOptVar->name());

        if(iCorrRecompVar<0)
        {
            QString msg;
            msg.asprintf("Unable to find variable %s for filling result",curOptVar->name().utf16());
            InfoSender::instance()->debug(msg);
        }
        else
        {
            int dataType = curOptVar->getFieldValue(OptVariable::DATATYPE).toInt();

            //For each result point
            for(int iPoint=0;iPoint<arch.size();iPoint++)
            {
                curResultPoint = &arch.at(iPoint);

                switch(dataType)
                {
                case OMREAL:
                    result->optVariablesResults()->at(iVar)->setFinalValue(0,iPoint,
                                curResultPoint->doubleVars.at(iDouble),true);
                    corrVar->setFinalValue(0,iPoint,
                                curResultPoint->doubleVars.at(iDouble),true);
                    break;
                case OMINTEGER:
                    result->optVariablesResults()->at(iVar)->setFinalValue(0,iPoint,
                                (double)curResultPoint->intVars.at(iInt),true);
                    corrVar->setFinalValue(0,iPoint,
                                (double)curResultPoint->intVars.at(iInt),true);
                    break;
                case OMBOOLEAN:
                    result->optVariablesResults()->at(iVar)->setFinalValue(0,iPoint,
                                (double)curResultPoint->boolVars.at(iBool),true);
                    corrVar->setFinalValue(0,iPoint,
                                (double)curResultPoint->boolVars.at(iBool),true);
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




//    // *******************************************************************
//    // Storing subModelChosen for each point
//    // *******************************************************************
//    if(subBlocks.size()>0)
//    {
//        for(int i=0;i<arch.size();i++)
//        {
//            curResultPoint = &arch.at(i);
//            result->_iSubModels.push_back(curResultPoint->intVars.at(curResultPoint->intVars.size()-1));
//        }
//    }

    // *******************************************************************
    // Filling OptObjectives Values
    // and corresponding recomputedVariables if nScans == 0
    // *******************************************************************
    moeoRealObjectiveVector<moeoObjectiveVectorTraits> resObjVector;
    int nbObj = resObjVector.size();
    VariableResult* curObjResult;
    for(int iObj=0;iObj<nbObj;iObj++)
    {
        curObjResult = result->optObjectivesResults()->at(iObj);
        corrVar = result->recomputedVariables()->findItem(curObjResult->name());
        if(iCorrRecompVar<0)
        {
            QString msg;
            msg.asprintf("Unable to find objective %s in in recomputed variables",curObjResult->name().utf16());
            InfoSender::instance()->debug(msg);
        }
        else
        {
            qDebug(QString("Found recomp value of obj " + curObjResult->name()).toLatin1().data());
            for(int iPoint=0;iPoint<arch.size();iPoint++)
            {
                curResultPoint = &arch.at(iPoint);
                resObjVector = curResultPoint->objectiveVector();
                result->optObjectivesResults()->at(iObj)->setFinalValue(0,iPoint,resObjVector.at(iObj),true);
                if(fillRecompValues)
                {
                    qDebug(QString("Fill Recomp value of obj " + curObjResult->name()).toLatin1().data());
                    corrVar->setFinalValue(0,iPoint,resObjVector.at(iObj),true);
                }
            }
        }
    }

    // *******************************************************************
    // Filling from savedVar Values
    // and corresponding recomputedVariables if nScans == 0
    // *******************************************************************
    Variable* curVar;
    for(int iSavedVar=0;iSavedVar<nbSavedVars;iSavedVar++)
    {
        curVar = problem->savedVars()->at(iSavedVar);
        corrVar = result->recomputedVariables()->findItem(curVar->name());
        if(iCorrRecompVar>-1)
        {
            for(int iPoint=0;iPoint<arch.size();iPoint++)
            {
                curResultPoint = &arch.at(iPoint);
                if(fillRecompValues)
                {
                    corrVar->setFinalValue(0,iPoint,curResultPoint->savedVars.at(iSavedVar),true);
                }
            }
        }
    }

    result->setSuccess(true);
    return result;
}

#endif
