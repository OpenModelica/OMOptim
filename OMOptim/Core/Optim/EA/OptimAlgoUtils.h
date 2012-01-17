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

 	@file MyAlgoUtils.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 

  */
#if !defined(_OPTIMALGOUTILS_H)
#define _OPTIMALGOUTILS_H

#include "OptimAlgo.h"
#include "OptimAlgosList.h"
#include <EA>

#include "Project.h"

namespace OptimAlgoUtils{

        static OptimAlgo* getNewAlgo(Project* _project,Problem* _problem,ModItemsTree* _modClassTree,int iAlgo)
	{
		switch(iAlgo)
		{
                case OptimAlgosList::iNSGA2:
                        return new NSGA2(_project,_problem,_modClassTree);
                case OptimAlgosList::iSPEA2:
                        return new SPEA2(_project,_problem,_modClassTree);
                case OptimAlgosList::iSPEA2Adapt:
                        return new SPEA2Adapt(_project,_problem,_modClassTree);
                case OptimAlgosList::iSA1:
                        return new SA1(_project,_problem,_modClassTree);
		default:
			//Problem
			return NULL;
		}
	}

        static QList<OptimAlgo*> getNewAlgos(Problem *problem,ModItemsTree* modClassTree)
	{
                QList<OptimAlgo*> algos;
                for(int i=0;i<OptimAlgosList::nbMyAlgos;i++)
		{
                        OptimAlgo* newAlgo;
                        newAlgo = getNewAlgo(problem->project(),problem,modClassTree,i);
				algos.push_back(newAlgo);
		}
		return algos;
	}
}

#endif
