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

 	@file SPEA2Adapt.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#if !defined(_SPEA2ADAPT_H)
#define _SPEA2ADAPT_H


#include <time.h>
#include <stdio.h>
#include <eo>
#include <moeo>
#include <es/eoRealInitBounded.h>
#include <es/eoRealOp.h>
#include <do/make_continue_moeo.h>
#include <do/make_checkpoint_moeo.h>
#include <do/make_ea_moeo.h>


#include "EABase.h"
#include "Project.h"
#include "Problem.h"
#include "OneSimulation.h"
#include "Optimization.h"
#include "OptimResult.h"
#include "Results/EAStdResult.h"
#include "SPEA2Adaptative/SPEA2AdaptMutation.h"
#include "Init/EAAdaptReinitStdDev.h"
#include "Chromosome/EOAdapt.h"

#include "SPEA2Adaptative/SPEA2AdaptParameters.h"


/********************************************************
SPEA2Adapt use SPEA2 algorithm but is auto-adaptive
(mutation amplitude varies between generations).
**********************************************************/

class SPEA2Adapt : public EABase
{
public : 
	SPEA2Adapt();
        SPEA2Adapt(Project*,Problem*,ModClassTree*);
        SPEA2Adapt(Project*,Problem*,ModClassTree*,MOParameters*);
	SPEA2Adapt(const SPEA2Adapt &);
        EABase* clone() const;

        bool acceptMultiObjectives();

	Result* launch(QString tempDir);
	QString name();
	void setDefaultParameters();

private :
        inline Result* buildResult(moeoUnboundedArchive<EOAdapt> & );
};

Result* SPEA2Adapt::buildResult(moeoUnboundedArchive<EOAdapt> & arch)
{
        Result* result = (Result*)EAStdResult<EOAdapt>::buildOptimResult(_project,(Optimization*)_problem,
                                                                         _subBlocks,arch);

	return result;
}

#endif
