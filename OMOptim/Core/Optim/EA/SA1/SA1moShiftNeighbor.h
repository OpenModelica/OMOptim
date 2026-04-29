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

#ifndef SA1MOSHIFTNEIGHBOR_H
#define SA1MOSHIFTNEIGHBOR_H

#include <neighborhood/moIndexNeighbor.h>
#include <utils/eoRealBounds.h>
#include <utils/eoIntBounds.h>
#include <algorithm>    // swap_ranges
#include <es/eoReal.h>
#include <utils/eoRNG.h>
#include <utils/eoRealBounds.h>
#include "LowTools.h"

/**
 * Indexed Shift Neighbor
 */


template <class EOT, class Fitness=typename EOT::Fitness>
class SA1moShiftNeighbor: public moIndexNeighbor<EOT, Fitness>
{
public:
    typedef typename EOT::ObjectiveVector ObjectiveVector;
    using moIndexNeighbor<EOT, Fitness>::key;

    /**
     * Apply move on a solution regarding a key
     * @param _sol the solution to move
     */
    virtual void move(EOT & _sol) {
        unsigned int tmp ;
        int  size=_sol.doubleVars.size()+ _sol.intVars.size()+_sol.boolVars.size();
        int rndNbr = (int) (size*rng.uniform());
        if(rndNbr == size)
            rndNbr--;

        if(rndNbr <= _sol.doubleVars.size()-1)
            _sol.doubleVars[rndNbr] = _doubleBounds[rndNbr].minimum()+ (_doubleBounds[rndNbr].maximum() - _doubleBounds[rndNbr].minimum())*rng.uniform();

        else if(rndNbr <= (_sol.doubleVars.size() + _sol.intVars.size())-1)
        {
            int i = rndNbr - _sol.doubleVars.size();
            _sol.intVars[i] = _intBounds[i].minimum()+ LowTools::round((_intBounds[i].maximum() - _intBounds[i].minimum())*rng.uniform());
        }
        else
        {
            int i = rndNbr - _sol.doubleVars.size() - _sol.intVars.size();
            _sol.boolVars[i] = !_sol.boolVars[i];
        }



        _sol.invalidate();
        _tmp = _sol;
    }

    /**
     * fix two indexes regarding a key
     * @param _key the key allowing to compute the two indexes for the shift
     */
    void translate(unsigned int _key) {
        int step;
        int val = _key;
        int tmpSize = size * (size-1) / 2;
        // moves from left to right
        if (val <= tmpSize) {
            step = size - 1;
            first = 0;
            while ((val - step) > 0) {
                val = val - step;
                step--;
                first++;
            }
            second = first + val + 1;
        }
        // moves from right to left (equivalent moves are avoided)
        else {  /* val > tmpSize */
            val = val - tmpSize;
            step = size - 2;
            second = 0;
            while ((val - step) > 0) {
                val = val - step;
                step--;
                second++;
            }
            first = second + val + 1;
        }
    }

    void print() {
        std::cout << key << ": [" << first << ", " << second << "] -> " << (*this).fitness() << std::endl;
    }

    void setDoubleBounds(std::vector<eoRealInterval>  doubleBounds)
    {
        _doubleBounds = doubleBounds;
    }

    void setIntBounds(std::vector<eoIntInterval>  intBounds)
    {
        _intBounds = intBounds;
    }

    void setNbBool(int nbBool)
    {
        _nbBool = nbBool;
    }


    EOT & getTmp()
    {
       return _tmp;

    }

    const ObjectiveVector objectiveVector()
    {
       return _tmp.objectiveVector();
    }


    void objectiveVector(const ObjectiveVector & obj)
    {
        _tmp.objectiveVector(obj);
    }

    const Fitness fitness() const
    {
        return _tmp.fitness();
    }

    void fitness(const Fitness & fit)
    {
        _tmp.fitness(fit);

    }

private:
    unsigned int first;
    unsigned int second;
    unsigned int size;
    std::vector<eoRealInterval> _doubleBounds;
    std::vector<eoIntInterval> _intBounds;
    int _nbBool;
    EOT  _tmp;
};




#endif // SA1MOSHIFTNEIGHBOR_H
