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

 	@file Result.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "Result.h"
#include "LowTools.h"

Result::Result()
{
	_project = NULL;
	_modModelPlus = NULL;
	_success = false;
}
Result::Result(Project* project,ModModelPlus* modModelPlus,Problem* problem,ModReader* modReader,ModPlusCtrl* modPlusReader)
{

	_project = project;
	_modModelPlus = modModelPlus;
	_modReader = modReader;
	_modPlusReader = modPlusReader;
	_problem = problem;

	_success = false;
	_algo = problem->getCurAlgo();

}

Result::Result(const Result &result)
{
	_project = result._project;
	_modModelPlus = result._modModelPlus;
	_modReader = result._modReader;
	_modPlusReader = result._modPlusReader;

	_problem = result._problem;
	_success = result._success;
	_algo = _problem->getCurAlgo();


	_name = result._name;
	_filesToCopyNames = result._filesToCopyNames;
	
	_computationTime = result._computationTime;
	_hour = result._hour;
	_date = result._date;
}

Result::~Result(void)
{
}

QString Result::getFieldName(int field,int role)
{
	return "name";
}
unsigned Result::getNbFields()
{
	return 1;
}

QStringList Result::filesToCopyNames()
{
	return _filesToCopyNames;
}

void Result::setName(QString name)
{
	_name = name;
}

Project* Result::project()
{
	return _project;
}
ModModelPlus* Result::modModelPlus()
{
	return _modModelPlus;
}

ModModel* Result::modModel()
{
	return _modModelPlus->modModel();
}

Problem* Result::problem()
{
	return _problem;
}

void Result::setModModelPlus(ModModelPlus* modModelPlus)
{
	_modModelPlus = modModelPlus;
}
void Result::setProject(Project* project)
{
	_project = project;
}

void Result::setProblem(Problem * problem)
{
	_problem = problem;
}

void Result::setAlgo(MyAlgorithm* algo)
{
	_algo = algo;
}

bool Result::isSuccess()
{
	return _success;
}

void Result::setSuccess(bool success)
{
	_success = success;
}



