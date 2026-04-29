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

#ifndef UPDATEPBEST_H
#define UPDATEPBEST_H

#include "eoPop.h"
#include "comparator/moeoParetoObjectiveVectorComparator.h"
#include <utils/eoRNG.h>







template <class EOT>
class UpdatePbest
{
public:
    typedef typename EOT::ObjectiveVector ObjectiveVector;

    void operator() (eoPop <EOT> & pbest, const eoPop <EOT> & _pop, moeoParetoObjectiveVectorComparator<ObjectiveVector> & pareto)
    {
        for(int i =0; i< _pop.size(); i++)
        {
            if (pareto(pbest[i].objectiveVector(), _pop[i].objectiveVector()))
            {
                pbest[i].doubleVars=_pop[i].doubleVars;
                pbest[i].intVars=_pop[i].intVars;
                pbest[i].boolVars=_pop[i].boolVars;
                pbest[i].objectiveVector(_pop[i].objectiveVector());
            }
            else if(! pareto(_pop[i].objectiveVector(), pbest[i].objectiveVector()))
            {
                int j = eo::rng.flip();

                if(j)
                {
                    pbest[i].doubleVars=_pop[i].doubleVars;
                    pbest[i].intVars=_pop[i].intVars;
                    pbest[i].boolVars=_pop[i].boolVars;
                    pbest[i].objectiveVector(_pop[i].objectiveVector());
                }

            }

        }


    }




};


#endif // UPDATEPBEST_H
