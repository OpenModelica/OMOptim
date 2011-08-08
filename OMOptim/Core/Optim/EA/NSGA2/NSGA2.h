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

 	@file NSGA2.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#if !defined(_NSGA2_H)
#define _NSGA2_H

#include "EABase.h"
#include <stdio.h>
#include <eo>
#include <moeo>
#include <es/eoRealInitBounded.h>
#include <es/eoRealOp.h>

// the stopping criterion
#include <do/make_continue_moeo.h>
// outputs (stats, population dumps, ...)
#include <do/make_checkpoint_moeo.h>

#include <do/make_ea_moeo.h>


#include "Project.h"
#include "Problem.h"
#include "OneSimulation.h"
#include "Optimization.h"
//#include "VariableDet.h"
#include "OptimResult.h"
#include "EAStdResult.h"

#include "EOStd.h"

#include "NSGA2Parameters.h"

/********************************************************
First multi-objective genetic algorithm implementation.
NSGA2 use NSGAII algorithm.
It allows multi-objectives but only real variables (no integer)
**********************************************************/


class NSGA2 : public EABase
{
public : 
	NSGA2();
        NSGA2(Project*,Problem*,ModClassTree*,ModPlusCtrl*);
        NSGA2(Project*,Problem*,ModClassTree*,ModPlusCtrl*,MOParameters*);
	NSGA2(const NSGA2 &);
	NSGA2* clone();

	QList<int> compatibleOMCases();

	Result* launch(QString tempDir);
	QString name();
	void setDefaultParameters();

private :
        inline Result* buildResult(moeoUnboundedArchive<EOStd> & );



};



Result* NSGA2::buildResult(moeoUnboundedArchive<EOStd> & arch)
{
	Result* result;
	switch(_problem->type())
	{
	case Problem::OPTIMIZATIONTYPE :
		result = (Result*)EAStdResult<EOStd>::buildOptimResult(_project,(Optimization*)_problem,_subBlocks,
                        _modClassTree,_modPlusCtrl,arch,_parameters);
		break;
	}
	return result;
}





#endif
