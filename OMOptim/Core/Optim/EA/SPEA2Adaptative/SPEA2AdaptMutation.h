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

     @file EAAdapt1Mutation.h
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version 0.9

  */

#ifndef EAADAPT1MUTATION_H
#define EAADAPT1MUTATION_H

//-----------------------------------------------------------------------------

#include <algorithm>    // swap_ranges
#include <es/eoReal.h>
#include <utils/eoRNG.h>
#include <utils/eoRealBounds.h>

//-----------------------------------------------------------------------------

#include "MOParameter.h"
#include "LowTools.h"

/** changes all values of the std::vector by uniform choice with range epsilon
with probability p_change per variable
\class eoUniformMutation
\ingroup parameteric
*/

template<class EOT> class EAAdapt1Mutation: public eoMonOp<EOT>
{
public:
    /**
    * Constructor with bounds
    * @param _bounds an eoRealVectorBounds that contains the bounds
    * @param _epsilon the range for uniform mutation - a double to be scaled
    * @param _p_change the one probability to change all coordinates
    */
    EAAdapt1Mutation(
        std::vector<eoRealInterval> & _doubleBounds,
        std::vector<eoIntInterval> & _intBounds,
                MOParameters* _parameters
        ):
    doubleBounds(_doubleBounds), intBounds(_intBounds)
    {
                parameters = _parameters;
    }

    /// The class name.
    virtual std::string className() const { return "EA2Mutation"; }

    /**
    * Do it!
    * @param _eo The indi undergoing the mutation
    */
    bool operator()(EOT& _eo)
    {
        bool hasChanged=false;
        // sanity check
        if ((_eo.doubleVars.size() != doubleBounds.size())
            || (_eo.intVars.size() != intBounds.size()))
        {
            throw std::runtime_error("Invalid size of indi in eoUniformMutation");
        }

        //************************
        // mutation for double
        //************************
        double taup = 1 / sqrt((double)(2*sqrt((double)_eo.doubleVars.size())));
        double tau = 1 / sqrt((double)(2*_eo.doubleVars.size()));
        double rnd1 = LowTools::gaussRandom(0,1);
        double newSigma,rndi,newValue;


        for (unsigned i=0; i<_eo.doubleVars.size(); i++)
        {
            rndi =  LowTools::gaussRandom(0,1);
            newSigma = _eo.dblStdDev.at(i)*exp(taup*rnd1+tau*rndi);
            newValue = _eo.doubleVars.at(i)+newSigma*LowTools::gaussRandom(0,1);
            newValue = std::max<double>(doubleBounds[i].minimum(), newValue);
            newValue = std::min<double>(doubleBounds[i].maximum(), newValue);
            _eo.doubleVars[i] = newValue;
            _eo.dblStdDev[i]= newSigma;
            hasChanged = true;
        }

        //************************
        // mutation for int and bool
        //************************
        if((_eo.intVars.size() + _eo.boolVars.size())>0)
        {
            double gamma = 1/(sqrt((double)2*sqrt((double)(_eo.intVars.size()+_eo.boolVars.size()))));
            rnd1 = LowTools::gaussRandom(0,1);
            double oldP = _eo.intProbMut;
            double newP = 1/((1-oldP)/oldP*exp(-gamma*rnd1));

            double epsilon = 1E-40;
            newValue = std::max<double>(newValue, epsilon);
            newValue = std::min<double>(newValue, 1-epsilon);

            _eo.intProbMut = newP;
            hasChanged = true;


            for (unsigned i=0; i<_eo.intVars.size(); i++)
            {
                if(rng.uniform()<newP)
                {
                    double emin = intBounds[i].minimum();
                    double emax = intBounds[i].maximum();
                    _eo.intVars[i] = LowTools::round(emin + (emax-emin)*rng.uniform());
                }
            }
            for (unsigned i=0; i<_eo.boolVars.size(); i++)
            {
                if(rng.uniform()<newP)
                {
                    _eo.boolVars[i] = (rng.uniform()>=0.5);
                }
            }
        }

        return hasChanged;
    }

private:
    std::vector<eoRealInterval> doubleBounds;
    std::vector<eoIntInterval> intBounds;

        MOParameters *parameters;
};

#endif
