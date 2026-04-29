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
 * @file SA1.h
 * @brief Comments for file documentation.
 * @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 * Company : CEP - ARMINES (France)
 * http://www-cep.ensmp.fr/english/
 * @version
 */


// $Id: SA1.h 9677 2011-08-24 13:09:00Z hubert.thieriot $

#if !defined(_PSO_H)
#define _PSO_H

#include "EABase.h"
#include "Project.h"
#include "Problem.h"
#include "ModItemsTree.h"
#include "Result.h"
#include "EAStdResult.h"
#include "Optimization.h"
#include "OptimAlgo.h"
#include "EOStd.h"






/********************************************************
Multi-objective Particle Swarm Optimization (PSO) implementation.
It allows multi-objectives but only real variables (no integer)
**********************************************************/


class PSO : public EABase
{
public :
        PSO();
        PSO(Project*,Problem*);
        PSO(Project*,Problem*,MOParameters*);
        PSO(const PSO &);
        EABase* clone() const;

        bool acceptMultiObjectives();

        Result* launch(QString tempDir);
        QString name();
        void setDefaultParameters();

private :
        inline Result* buildResult(moeoUnboundedArchive<EOStd> & );


};



Result* PSO::buildResult(moeoUnboundedArchive<EOStd> & arch)
{
        Result* result;
            result = (Result*)EAStdResult<EOStd>::buildOptimResult(_project,(Optimization*)_problem,/*_subBlocks,*/
                        arch);

        return result;
}


#endif
