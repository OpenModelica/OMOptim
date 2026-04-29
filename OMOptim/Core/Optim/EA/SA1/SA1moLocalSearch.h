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

#ifndef SA1MOLOCALSEARCH_H
#define SA1MOLOCALSEARCH_H
#include <explorer/moNeighborhoodExplorer.h>
#include <continuator/moContinuator.h>
#include <neighborhood/moNeighborhood.h>
#include <eoEvalFunc.h>
#include <eoOp.h>
#include <EOStd.h>
#include<SA1explorer.h>
#include<SA1moNeighborhoodExplorer.h>
/**
 * the main algorithm of the local search
 */
template<class Neighbor>
class SA1moLocalSearch: public eoMonOp<typename Neighbor::EOT>
{
public:
    typedef moNeighborhood<Neighbor> Neighborhood;
    typedef SA1moNeighborhoodExplorer<Neighbor> NeighborhoodExplorer;
    typedef typename Neighbor::EOT EOT ;

    /**
     * Constructor of a moLocalSearch
     * @param _searchExpl a neighborhood explorer
     * @param _cont an external continuator (can be a checkpoint!)
     * @param _fullEval a full evaluation function
     */
    SA1moLocalSearch(NeighborhoodExplorer& _searchExpl, moContinuator<Neighbor> & _cont, eoEvalFunc<EOT>& _fullEval) : searchExplorer(_searchExpl), cont(&_cont), fullEval(_fullEval) { } ;

    /**
     * Run the local search on a solution
     * @param _solution the related solution
     */
    virtual bool operator() (EOT & _solution) {

        if (_solution.invalid())
            fullEval(_solution);

        // initialization of the parameter of the search (for example fill empty the tabu list)
        searchExplorer.initParam(_solution);

        // initialization of the external continuator (for example the time, or the number of generations)
        cont->init(_solution);

        bool b;

        do {
            // explore the neighborhood of the solution
            searchExplorer(_solution);
            // if a solution in the neighborhood can be accepted
            if (searchExplorer.accept(_solution)) {
                searchExplorer.move(_solution);
                searchExplorer.moveApplied(true);
            }
            else
                searchExplorer.moveApplied(false);

            // update the parameter of the search (for ex. Temperature of the SA)
            searchExplorer.updateParam(_solution);

            b=(*cont)(_solution);
        } while (b && searchExplorer.isContinue(_solution));

        searchExplorer.terminate(_solution);

        cont->lastCall(_solution);

        return true;
    };



    /**
     * Set an external continuator
     * @param _cont the external continuator
    */
    void setContinuator(moContinuator<Neighbor> & _cont) {
        cont = &_cont ;
    }

    /**
     * external continuator object
     *
     * @overload
     * @return the external continuator
    */
    moContinuator<Neighbor>* getContinuator() const {
        return cont ;
    }

protected:
    // make the exploration of the neighborhood according to a local search heuristic
    SA1moNeighborhoodExplorer<Neighbor>& searchExplorer ;

    // external continuator
    moContinuator<Neighbor> * cont ;

    //full evaluation function
    eoEvalFunc<EOT>& fullEval;
};

#endif // SA1MOLOCALSEARCH_H
