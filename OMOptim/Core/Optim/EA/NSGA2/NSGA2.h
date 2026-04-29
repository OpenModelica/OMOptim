/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
 * VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the OSMC (Open Source Modelica Consortium)
 * Public License (OSMC-PL) are obtained from OSMC, either from the above
 * address, from the URLs:
 * http://www.openmodelica.org or
 * https://github.com/OpenModelica/ or
 * http://www.ida.liu.se/projects/OpenModelica,
 * and in the OpenModelica distribution.
 *
 * GNU AGPL version 3 is obtained from:
 * https://www.gnu.org/licenses/licenses.html#GPL
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

/*
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)
 * @file NSGA2.h
 * @brief Comments for file documentation.
 * @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 * Company : CEP - ARMINES (France)
 * http://www-cep.ensmp.fr/english/
 * @version 0.9
 */


// $Id$

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
#include "Results/EAStdResult.h"

#include "Chromosome/EOStd.h"

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
    NSGA2(Project*,Problem*);
    NSGA2(Project*,Problem*,MOParameters*);
    NSGA2(const NSGA2 &);
    EABase* clone() const;

    bool acceptMultiObjectives();

    Result* launch(QString tempDir);
    QString name();
    void setDefaultParameters();

private :
    inline Result* buildResult(moeoUnboundedArchive<EOStd> & );



};



Result* NSGA2::buildResult(moeoUnboundedArchive<EOStd> & arch)
{
    Result* result = (Result*)EAStdResult<EOStd>::buildOptimResult(_project,(Optimization*)_problem,
                                                                   /*_subBlocks,*/arch);
    return result;
}





#endif
