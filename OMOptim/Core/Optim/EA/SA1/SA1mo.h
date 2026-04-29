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

#ifndef SA1MO_H
#define SA1MO_H

#include <algo/moLocalSearch.h>
#include <SA1explorer.h>
#include <coolingSchedule/moCoolingSchedule.h>
#include <coolingSchedule/moSimpleCoolingSchedule.h>
#include <continuator/moTrueContinuator.h>
#include <eval/moEval.h>
#include <eoEvalFunc.h>
#include<SA1moLocalSearch.h>

/**
 * Simulated Annealing
 */
template<class Neighbor>
class SA1mo: public moLocalSearch<Neighbor>
{
public:

    typedef typename Neighbor::EOT EOT;
    typedef moNeighborhood<Neighbor> Neighborhood ;


    /**
     * Basic constructor for a simulated annealing
     * @param _neighborhood the neighborhood
     * @param _fullEval the full evaluation function
     * @param _eval neighbor's evaluation function
     * @param _initT initial temperature for cooling schedule (default = 10)
     * @param _alpha factor of decreasing for cooling schedule (default = 0.9)
     * @param _span number of iteration with equal temperature for cooling schedule (default = 100)
     * @param _finalT final temperature, threshold of the stopping criteria for cooling schedule (default = 0.01)
     */
    SA1mo(Neighborhood& _neighborhood, eoEvalFunc<EOT>& _fullEval, moEval<Neighbor>& _eval,  moContinuator<Neighbor>& _cont, double _initT=0.004, double _alpha=0.2, unsigned _span=100, double _finalT=0.00001):
            moLocalSearch<Neighbor>(explorer, _cont, _fullEval),
            defaultCool(_initT, _alpha, _span, _finalT),
            explorer(_neighborhood, _eval, defaultSolNeighborComp, defaultCool)
    {}


    /**
     * Simple constructor for a simulated annealing
     * @param _neighborhood the neighborhood
     * @param _fullEval the full evaluation function
     * @param _eval neighbor's evaluation function
     * @param _cool a cooling schedule
     */
    SA1mo(Neighborhood& _neighborhood, eoEvalFunc<EOT>& _fullEval, moEval<Neighbor>& _eval, moCoolingSchedule<EOT>& _cool):
            moLocalSearch<Neighbor>(explorer, trueCont, _fullEval),
            defaultCool(0, 0, 0, 0),
            explorer(_neighborhood, _eval, defaultSolNeighborComp, _cool)
    {}

    /**
     * General constructor for a simulated annealing
     * @param _neighborhood the neighborhood
     * @param _fullEval the full evaluation function
     * @param _eval neighbor's evaluation function
     * @param _cool a cooling schedule
     * @param _comp a solution vs neighbor comparator
     * @param _cont an external continuator
     */
    SA1mo(Neighborhood& _neighborhood, eoEvalFunc<EOT>& _fullEval, moEval<Neighbor>& _eval, moCoolingSchedule<EOT>& _cool, SA1moSolNeighborComparator<Neighbor>& _comp, moContinuator<Neighbor>& _cont):
            moLocalSearch<Neighbor>(explorer, _cont, _fullEval),
            defaultCool(0, 0, 0, 0),
            explorer(_neighborhood, _eval, _comp, _cool)
    {}



private:
    moTrueContinuator<Neighbor> trueCont;
    moSimpleCoolingSchedule<EOT> defaultCool;
    SA1moSolNeighborComparator<Neighbor> defaultSolNeighborComp;
    SA1explorer<Neighbor> explorer;
};







#endif // SA1MO_H
