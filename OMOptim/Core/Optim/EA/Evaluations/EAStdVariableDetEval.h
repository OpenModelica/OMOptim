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

 	@file EAStdVariableDetEval.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#ifndef _EAStdVariableDetEval_H
#define _EAStdVariableDetEval_H

#include <EA>
#include "VariableDet.h"
#include <math.h>
#include "LowTools.h"

template<class EOT> class EAStdVariableDetEval : public moeoEvalFunc < EOT >
{

public:

	EAStdVariableDetEval(){};
	EAStdVariableDetEval(const EAStdVariableDetEval & _EAStdVariableDetEval){
		problem = _EAStdVariableDetEval.problem;
		project = _EAStdVariableDetEval.project;
		tempDir = _EAStdVariableDetEval.tempDir;
		index = index;
	};
	EAStdVariableDetEval(Project* _project,VariableDet* _problem,QVector<QVector<int > > _index,QString _tempDir)
	{
		project = _project;
		problem = _problem;
		tempDir = _tempDir;
		index = _index;


		/************************************
		OBJECTIVE FUNCTIONS DEFINITION
		************************************/
		unsigned int nbObj = _problem->fuzzyVars->nbPoints();
		std::vector<bool> _bObjectives;

		for(unsigned int iObj=0;iObj<nbObj;iObj++)
			_bObjectives.push_back(true);

		moeoObjectiveVectorTraits::setup(nbObj,_bObjectives);

	};

	void operator () (EOT & _eo)
	{
		if (_eo.invalidObjectiveVector())
		{
			moeoRealObjectiveVector < moeoObjectiveVectorTraits > objVec;

			Model* _model = problem->getModel();

			//Creating a new OneSimulation problem based on same model
			OneSimulation *oneSim = new OneSimulation(project,_model);
			//project->assignNewTempDir(oneSim);

			//Reading chromosome and placing it in overwritedvariables
			int nbVars = problem->fuzzyVars->getNbVars();


			// building variable vectors to simulate
			MOVector<Variable> *_vars = new MOVector<Variable>;
			for(int iVar=0;iVar<nbVars;iVar++)
			{
				_vars->addItem(problem->fuzzyVars->getVariable(iVar)->toStdVariable());
			}


			// simulate for each point

			QVector<QVector<double> > resultValues ; //[iPoint,iVar]


			int nbPoints = problem->fuzzyVars->nbPoints();
			double curValue;
			int corrIndex;
			for(int iPoint = 0;iPoint<nbPoints;iPoint++)
			{
				for(int iVar=0;iVar<nbVars;iVar++)
				{
					corrIndex = index.at(iVar).at(iPoint);
					if(corrIndex>-1)
					{
						curValue = _eo.doubleVars.at(corrIndex);
						_vars->items.at(iVar)->setFieldValue(Variable::VALUE,curValue);
					}
					else
					{
						curValue = problem->fuzzyVars->getValue(iVar,iPoint);
						_vars->items.at(iVar)->setFieldValue(Variable::VALUE,curValue);
					}
				}

				oneSim->overwritedVariables = _vars;

				//Launch simulation
				bool refillTempDir = false;
				ProblemConfig config(tempDir,refillTempDir);
				oneSim->launch(config);
				OneSimResult *result = oneSim->result();

				//Store results
				QVector<double> curPointResult;
				if(!result->isSuccess())
				{
					// penalty, store 100% deviation
					curPointResult = QVector<double>(nbVars,LowTools::nan()); // add NAN  as result
				}
				else
				{
					curPointResult.clear();
					for(int iVar=0;iVar<nbVars;iVar++)
					{
						int iFinalVar = result->finalVariables->findItem(problem->fuzzyVars->getVariableName(iVar));

						if(iFinalVar<0)
							curPointResult.push_back(LowTools::nan());
						else
							curPointResult.push_back(result->finalVariables->items.at(iFinalVar)->getFieldValue(Variable::VALUE).toDouble());
					}
				}

				resultValues.push_back(curPointResult);
			}



			// Calculate Deviations
			double curPointDev;
			double curVarDev;
			double curVarAbsPrec;
			double curVarValue;

			for(int iPoint=0;iPoint<nbPoints;iPoint++)
			{
				curPointDev = 0;
				for(int iVar=0;iVar<nbVars;iVar++)
				{ 
					if(LowTools::isNan(resultValues.at(iPoint).at(iVar)))
					{
						curVarDev=1;
					}
					else
					{
						curVarAbsPrec = problem->fuzzyVars->getAbsPrecValue(iVar,iPoint);
						curVarValue = problem->fuzzyVars->getValue(iVar,iPoint);

						if((curVarAbsPrec==0)&&(resultValues.at(iPoint).at(iVar)!=curVarValue))
							curVarDev=1;
						else
							curVarDev = fabs(resultValues.at(iPoint).at(iVar)-curVarValue)/curVarAbsPrec;
					}
					curPointDev += curVarDev;
				}
				objVec[iPoint] = curPointDev;
			}
			_eo.objectiveVector(objVec);
		}
	}


protected:
	VariableDet* problem;
	Project* project;



	std::vector<OneSimResult*> *resultPoints;
	QString tempDir;

	QVector<QVector<int > > index;
};

#endif
