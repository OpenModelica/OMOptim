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

 	@file MyAlgoUtils.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#if !defined(_MYALGOUTILS_H)
#define _MYALGOUTILS_H

#include "MyAlgorithm.h"
#include "MyListAlgos.h"
#include <EA>

#include "Project.h"

namespace MyAlgoUtils{

	static MyAlgorithm* getNewAlgo(Project* _project,Problem* _problem,ModReader* _modReader,ModPlusCtrl* _modPlusReader,ModClass* _rootModClass,int iAlgo)
	{
		switch(iAlgo)
		{
		case MyListAlgos::iNSGA2:
			return new NSGA2(_project,_problem,_modReader,_modPlusReader,_rootModClass);
		case MyListAlgos::iSPEA2:
			return new SPEA2(_project,_problem,_modReader,_modPlusReader,_rootModClass);
		case MyListAlgos::iSPEA2Adapt:
			return new SPEA2Adapt(_project,_problem,_modReader,_modPlusReader,_rootModClass);
		default:
			//Problem
			return NULL;
		}
	}

	static QList<MyAlgorithm*> getNewAlgos(Problem *problem,ModReader* _modReader,ModPlusCtrl* _modPlusReader,ModClass* _rootModClass)
	{
		QList<MyAlgorithm*> algos;
		for(int i=0;i<MyListAlgos::nbMyAlgos;i++)
		{
			MyAlgorithm* newAlgo;
			newAlgo = getNewAlgo(problem->project(),problem,_modReader,_modPlusReader,_rootModClass,i);
			if(newAlgo->compatibleProblems().contains(problem->type()))
				algos.push_back(newAlgo);
			else
				delete newAlgo;
		}
		return algos;
	}
}

#endif
