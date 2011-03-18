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
#include "MyEAProgress.h"
#include "VariablesManip.h"
#include "float.h"

#ifdef USEEI
	#include "ProblemTarget.h"
#endif


//#include "reportingHook.h"



template<class EOT> class EAStdOptimizationEval : public moeoEvalFunc < EOT >
{

public:

	EAStdOptimizationEval(){};
	EAStdOptimizationEval(const EAStdOptimizationEval & _EAStdOptimizationEval){
		problem = _EAStdOptimizationEval.problem;
		project = _EAStdOptimizationEval.project;
		modReader = _EAStdOptimizationEval.modReader;
		modPlusReader = _EAStdOptimizationEval.modPlusReader;
		rootModClass = _EAStdOptimizationEval.rootModClass;


		tempDir = _EAStdOptimizationEval.tempDir;
		subModels = _EAStdOptimizationEval.subModels;
		previousSubModel = _EAStdOptimizationEval.previousSubModel;
		_nbObj = _EAStdOptimizationEval._nbObj;
		_bObjectives = _EAStdOptimizationEval._bObjectives;
	};

	EAStdOptimizationEval(Project* _project,Optimization* _problem,QList<ModModelPlus*> _subModels,QString _tempDir
		,ModReader* _modReader,ModPlusCtrl* _modPlusReader,ModClass* _rootModClass)
	{
		project = _project;
		problem = _problem;
		modReader = _modReader;
		modPlusReader = _modPlusReader;
		rootModClass = _rootModClass;

		tempDir = _tempDir;
		subModels = _subModels;
		previousSubModel = -1;

		/************************************
		OBJECTIVE FUNCTIONS DEFINITION
		************************************/
		_nbObj = problem->objectives()->items.size();
		OptObjective::Direction objDirection;

		for(unsigned int iObj=0;iObj<_nbObj;iObj++)
		{
			objDirection = problem->objectives()->items.at(iObj)->direction();
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

	};

	void operator () (EOT & _eo)
	{
		if (_eo.invalidObjectiveVector())
		{
			moeoRealObjectiveVector< moeoObjectiveVectorTraits > objVec;

			ModModelPlus* _model;

			int iSubModel = -1;
			if(subModels.size()>1)
			{
				iSubModel = _eo.intVars.at(_eo.intVars.size()-1);
				_model = subModels.at(iSubModel);
			}
			else
				_model = problem->modModelPlus();

			/************************************
			Creating a new OneSimulation
			************************************/
			OneSimulation *oneSim = new OneSimulation(project,rootModClass,modReader,modPlusReader,_model);
			
			//Reading chromosome and placing it in overwritedvariables
			int nbVar = problem->optimizedVariables()->items.size();
			Variable* curVar;

			int iDouble = 0;
			int iInt = 0;
			int iBool = 0;

			for(int i=0;i<nbVar;i++)
			{
				curVar = new Variable(*problem->optimizedVariables()->items.at(i));
				switch(curVar->getFieldValue(Variable::DATATYPE).toInt())
				{
                                case OMREAL :
					curVar->setValue(_eo.doubleVars.at(iDouble));
					iDouble++;
					break;
                                case OMBOOLEAN :
					curVar->setValue(_eo.boolVars.at(iBool));
					iBool++;
					break;
                                case OMINTEGER :
					curVar->setValue(_eo.intVars.at(iInt));
					iInt++;
					break;
				}
				oneSim->overwritedVariables()->addItem(curVar);
			}

			oneSim->scannedVariables()->cloneFromOtherVector(problem->scannedVariables());


			/************************************
			Launch OneSimulation
			************************************/
			bool refillTempDir = false;
			QDir _tempDir(tempDir);
			if(_tempDir.entryList().isEmpty())
				refillTempDir = true;
			else
			{
				if((subModels.size()>1)&&(iSubModel!=previousSubModel))
					refillTempDir = true;
			}
			ProblemConfig config(tempDir,refillTempDir);
			oneSim->launch(config);
			OneSimResult *result = oneSim->result();

			previousSubModel = iSubModel;

			/************************************
			Launch EI problem
			************************************/
			#ifdef USEEI
			if(problem->useEI())
			{
				if(problem->eiProblem()->getClassName()=="ProblemTarget")
				{
					ProblemTarget* problemTarget = dynamic_cast<ProblemTarget*>(problem->eiProblem());
					problemTarget->setInputVars(result->finalVariables());
					problemTarget->launch(ProblemConfig());
				}
			}
			#endif

			/************************************
			Read results
			************************************/
			QString objName;
			int iVarObj;
			OptObjective* curObj;
			bool resultOk = true;

			if (!result->isSuccess())
			{
				//emit sendInfo( Info(ListInfo::ONESIMULATIONFAILED));
				resultOk = false;
			}
			else
			{
				//emit sendInfo( Info(ListInfo::ONESIMULATIONSUCCESS));
				//Recover Objective values
				int nbObj = problem->objectives()->items.size();
				int iObj=0;
				double curObjResult;
				
				while(resultOk && (iObj<nbObj))
				{
					curObj = problem->objectives()->items.at(iObj);
					//looking for its value in finalVariables
					curObjResult = VariablesManip::calculateObjValue(curObj,result->finalVariables(),resultOk,0);
					objVec[iObj]=curObjResult;
					
					iObj++;
				}
			}

			if (!resultOk)
			{
				// Penalty
				objVec = worstObjVec();
			}

			_eo.objectiveVector(objVec);
			delete oneSim;
		}
	}

	moeoRealObjectiveVector< moeoObjectiveVectorTraits > worstObjVec()
	{
		moeoRealObjectiveVector< moeoObjectiveVectorTraits > _objVec;

		for(int i=0;i<_nbObj;i++)
		{
			if(_bObjectives[i])
				_objVec[i]=std::numeric_limits<double>::infinity();
			else
				_objVec[i]=-std::numeric_limits<double>::infinity();
		}
		return _objVec;
	}

protected:
	Optimization* problem;
	Project* project;
	ModReader* modReader;
	ModPlusCtrl* modPlusReader;
	ModClass* rootModClass;
	QList<ModModelPlus*> subModels;
	std::vector<OneSimResult*> *resultPoints;
	QString tempDir;
	int previousSubModel;
	int _nbObj;
	std::vector<bool> _bObjectives;
};

#endif
