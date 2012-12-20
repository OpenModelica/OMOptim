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

  @file EAStdOptimizationEval.h
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version 0.9 0.1

  */
#ifndef _EAStdOptimizationEval_H
#define _EAStdOptimizationEval_H

#include "Project.h"
#include "Optimization.h"
#include "OMEAProgress.h"
#include "VariablesManip.h"
#include "float.h"
#include "Optimization.h"
#include "moeo"
#include "LowTools.h"

/** \class EAStdOptimizationEval is a function for evaluating fitness of an individual.
          * For each individual, it :
          *    -creates a corresponding OneSimulation problem.
          *    -set corresponding inputs
          *    -read results and set corresponding values to objectives
          */


template<class EOT> class EAStdOptimizationEval : public moeoEvalFunc < EOT >
{

public:

    EAStdOptimizationEval(){};

    /**
     * \brief Copy constructor
     */
    EAStdOptimizationEval(const EAStdOptimizationEval & EAStdOptimizationEval)
    {
        _problem = EAStdOptimizationEval._problem;
        _project = EAStdOptimizationEval._project;
        _modItemsTree = EAStdOptimizationEval._modItemsTree;
        _tempDir = EAStdOptimizationEval._tempDir;
        _subModels = EAStdOptimizationEval._subModels;
        _nbObj = EAStdOptimizationEval._nbObj;
        _bObjectives = EAStdOptimizationEval._bObjectives;
    }


    /**
     * \brief Ctor.
     */
    EAStdOptimizationEval(Project* project,Optimization* problem,QList<QList<ModelPlus*> > subModels,QString tempDir
                           ,ModItemsTree* modItemsTree)
    {
        _project = project;
        _problem = problem;
        _modItemsTree = modItemsTree;
        _tempDir = tempDir;
        _subModels = subModels;

        /************************************
        OBJECTIVE FUNCTIONS DEFINITION
        ************************************/
        _nbObj = _problem->objectives()->size();
        OptObjective::Direction objDirection;

        for(unsigned int iObj=0;iObj<_nbObj;iObj++)
        {
            objDirection = problem->objectives()->at(iObj)->direction();
            if(objDirection == OptObjective::MINIMIZE)
            {
                _bObjectives.push_back(true);
            }
            else
            {
                _bObjectives.push_back(false);
            }
        }
        moeoObjectiveVectorTraits::setup(_nbObj,_bObjectives);

    }

    void operator () (EOT & eo)
    {
        if (eo.invalidObjectiveVector())
        {
            moeoRealObjectiveVector< moeoObjectiveVectorTraits > objVec;

            QList<ModelPlus*> models;


            int iSubModel = -1;
            if(_subModels.size()>1)
            {
                iSubModel = eo.intVars.at(eo.intVars.size()-1);
                models = _subModels.at(iSubModel);
            }
            else
            {
                QStringList modelsNames = _problem->models();
                for(int i=0;i<modelsNames.size();i++)
                    models.push_back(_project->modelPlus(modelsNames.at(i)));
            }


            //******************************
            // get variables
            //******************************
            Variables* overwritedVariables = new Variables(true);
            Variable *curVar;
            int iDouble = 0;
            int iBool = 0;
            int iInt = 0;

            int nbVars = _problem->optimizedVariables()->size();
            for(int i=0;i<nbVars;i++)
            {
                curVar = new Variable(*_problem->optimizedVariables()->at(i));
                switch(curVar->getFieldValue(Variable::DATATYPE).toInt())
                {
                case OMREAL :
                    curVar->setValue(eo.doubleVars.at(iDouble));
                    iDouble++;
                    break;
                case OMBOOLEAN :
                    curVar->setValue(eo.boolVars.at(iBool));
                    iBool++;
                    break;
                case OMINTEGER :
                    curVar->setValue(eo.intVars.at(iInt));
                    iInt++;
                    break;
                }
                overwritedVariables->addItem(curVar);
            }

            // copying relevant scanned variables
            ScannedVariables* scannedVariables = _problem->scannedVariables()->clone();

            /************************************
                    Evaluate
            ************************************/
            bool evaluationOk = true;
            MOOptVector* resultVariables = _problem->evaluate(models,overwritedVariables,scannedVariables,evaluationOk);


            /************************************
            Read results
            ************************************/

            bool readingObjOk = true;
            if(evaluationOk)
            {
                //Recover Objective values
                int nbObj = _problem->objectives()->size();
                int iObj=0;
                double curObjResult;
                OptObjective* curObj;

                while(readingObjOk && (iObj<nbObj))
                {
                    curObj = _problem->objectives()->at(iObj);

                    //looking for its value in resultVariables
                    curObjResult = VariablesManip::calculateObjValue(curObj,resultVariables,readingObjOk,0);
                    objVec[iObj]=curObjResult;

                    iObj++;
                }

                // recover saved variables
                QString curVarName;
                QString curModelName;
                VariableResult* curVarResult;
                for(int i=0;i<_problem->savedVars()->size();i++)
                {
                    curVarName=_problem->savedVars()->at(i)->name(Variable::SHORT);
                    curModelName=_problem->savedVars()->at(i)->model();
                    curVarResult = resultVariables->findVariable(curModelName,curVarName);
                    if(curVarResult)
                        eo.savedVars.push_back(curVarResult->finalValue(0,0));
                    else
                        eo.savedVars.push_back(LowTools::nan());
                }
            }

            if (!evaluationOk || !readingObjOk)
            {
                // Penalty
                objVec = worstObjVec();
            }

            eo.objectiveVector(objVec);

            //************
            // free memory
            //************
            delete resultVariables;
            delete overwritedVariables;
            delete scannedVariables;
        }
    }

    /** \brief creates the worst objective vector as to be unconsidered in population
      * In EAStdOptimizationEval, is attributed to all failing simulations
      */
    moeoRealObjectiveVector< moeoObjectiveVectorTraits > worstObjVec()
    {
        moeoRealObjectiveVector< moeoObjectiveVectorTraits > objVec;

        for(int i=0;i<_nbObj;i++)
        {
            if(_bObjectives[i])
                objVec[i]=std::numeric_limits<double>::infinity();
            else
                objVec[i]=-std::numeric_limits<double>::infinity();
        }
        return objVec;
    }

protected:
    Optimization* _problem;
    Project* _project;
    ModItemsTree* _modItemsTree;
    QList<QList<ModelPlus*> > _subModels;
    std::vector<OneSimResult*> *_resultPoints;
    QString _tempDir;
    int _nbObj;
    std::vector<bool> _bObjectives;
};
#endif
