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

 	@file SPEA2.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#if !defined(_SPEA2_H)
#define _SPEA2_H

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


/********************************************************
First multi-objective genetic algorithm implementation.
SPEA2 use NSGAII algorithm.
It allows multi-objectives but only real variables (no integer)
**********************************************************/
#include "EOStd.h"

class SPEA2 : public EABase
{
public : 
	SPEA2();
	SPEA2(Project*,Problem*,ModReader*,ModPlusCtrl*,ModClass* _rootClass);
	SPEA2(Project*,Problem*,ModReader*,ModPlusCtrl*,ModClass* _rootClass,EAConfig*);
	SPEA2(const SPEA2 &);

	SPEA2* clone();

	QList<int> compatibleProblems();

	Result* launch(QString tempDir);
	QString name();
	void setDefaultParameters();

private :
	inline Result* buildResult(moeoUnboundedArchive<EOStd> &, EAConfig* );



};

Result* SPEA2::buildResult(moeoUnboundedArchive<EOStd> & arch, EAConfig* config)
{
	Result* result;
	switch(_problem->type())
	{
	case Problem::OPTIMIZATION :
		result = (Result*)EAStdResult<EOStd>::buildOptimResult(_project,(Optimization*)_problem,_subBlocks,_modReader,_modPlusReader,arch,config);
		break;
	}
	return result;
}






#endif
