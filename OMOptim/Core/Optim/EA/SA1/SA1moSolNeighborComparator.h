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

#ifndef SA1MOSOLNEIGHBORCOMPARATOR_H
#define SA1MOSOLNEIGHBORCOMPARATOR_H

#include <EO.h>
#include <eoFunctor.h>

#include <neighborhood/moNeighbor.h>
#include <comparator/moComparator.h>


/**
 * Comparator of two neighbors
 */
template< class Neighbor >
class SA1moSolNeighborComparator : public moComparator<typename Neighbor::EOT, Neighbor>
{
public:
    typedef typename Neighbor::EOT EOT ;

    /**
     * Compare two neighbors
     * @param _sol the solution
     * @param _neighbor the neighbor
     * @return true if the neighbor is better than sol
     */
    virtual bool operator()( const EOT& _sol, const Neighbor& _neighbor) {
        return (_sol.fitness() < _neighbor.fitness());
    }

    /**
     * Test the equality between two neighbors
     * @param _sol the solution
     * @param _neighbor the neighbor
     * @return true if the neighbor is equal to the solution
     */
    virtual bool equals( const EOT& _sol, const Neighbor& _neighbor) {
        return (_sol.fitness() == _neighbor.fitness());
    }

    /**
     * Return the class Name
     * @return the class name as a std::string
     */
    virtual std::string className() const {
        return "SA1moSolNeighborComparator";
    }
};

#endif // SA1MOSOLNEIGHBORCOMPARATOR_H
