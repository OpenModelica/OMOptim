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

#ifndef SA1MONEIGHBORHOODEXPLORER_H
#define SA1MONEIGHBORHOODEXPLORER_H



//EO inclusion
#include <eoFunctor.h>

#include <neighborhood/moNeighborhood.h>
#include <eval/moEval.h>
#include <neighborhood/moDummyNeighborhood.h>
#include <eval/moDummyEval.h>
#include <EOStd.h>
/**
 * Explore the neighborhood
 */
template< class Neighbor >
class SA1moNeighborhoodExplorer : public eoUF<typename Neighbor::EOT&, void>
{
public:
    typedef moNeighborhood<Neighbor> Neighborhood;
    typedef typename Neighbor::EOT EOT;
    typedef typename EOT::Fitness Fitness ;

    SA1moNeighborhoodExplorer():neighborhood(dummyNeighborhood), eval(dummyEval), isMoved(false) {}

    /**
     * Constructor with a Neighborhood and evaluation function
     * @param _neighborhood the neighborhood
     * @param _eval the evaluation function
     */
    SA1moNeighborhoodExplorer(Neighborhood& _neighborhood, moEval<Neighbor>& _eval):neighborhood(_neighborhood), eval(_eval), isMoved(false) {}

    /**
     * Init Search parameters
     * @param _solution the solution to explore
     */
    virtual void initParam (EOT& _solution) = 0 ;

    /**
     * Update Search parameters
     * @param _solution the solution to explore
     */
    virtual void updateParam (EOT& _solution) = 0 ;

    /**
     * Test if the exploration continue or not
     * @param _solution the solution to explore
     * @return true if the exploration continue, else return false
     */
    virtual bool isContinue(EOT& _solution) = 0 ;

    /**
     * Move a solution
     * @param _solution the solution to explore
     */
    virtual void move(EOT& _solution) = 0 ;

    /**
     * Test if a solution is accepted
     * @param _solution the solution to explore
     * @return true if the solution is accepted, else return false
     */
    virtual bool accept(EOT& _solution) = 0 ;

    /**
     * Terminate the search
     * @param _solution the solution to explore
     */
    virtual void terminate(EOT& _solution) = 0 ;

    /**
     * Getter of "isMoved"
     * @return true if move is applied
     */





    bool moveApplied() {
        return isMoved;
    }

    /**
     * Setter of "isMoved"
     * @param _isMoved
     */
    void moveApplied(bool _isMoved) {
        isMoved=_isMoved;
    }

protected:
    moDummyNeighborhood<Neighbor> dummyNeighborhood;
    moDummyEval<Neighbor> dummyEval;
    Neighborhood & neighborhood;
    moEval<Neighbor>& eval;
    bool isMoved;
};

#endif // SA1MONEIGHBORHOODEXPLORER_H
