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

     @file SPEA2AdaptInitBounded.h
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version 0.1 

  */

#ifndef EAADAPT1INITBOUNDED_H
#define EAADAPT1INITBOUNDED_H

//-----------------------------------------------------------------------------

#include <utils/eoRNG.h>
#include <eoInit.h>
#include <utils/eoRNG.h>
#include <utils/eoRealBounds.h>
#include "LowTools.h"


template <class EOT>
class SPEA2AdaptInitBounded : public eoInit<EOT>
{
 public:
     /** Ctor - from eoRealVectorBounds */
     SPEA2AdaptInitBounded(
         std::vector<eoRealInterval> & _doubleBounds,
         std::vector<eoIntInterval> & _intBounds,
         unsigned _nbBool,
         unsigned _initPopSize
         ):
    doubleBounds(_doubleBounds),
         intBounds(_intBounds)
     {
         nbBool = _nbBool;
         initPopSize = _initPopSize;
    }

  /** simply passes the argument to the uniform method of the bounds */
  virtual void operator()(EOT & _eo)
    {

        // init double values
        _eo.doubleVars.resize(doubleBounds.size());
        for(unsigned i=0;i<_eo.doubleVars.size();i++)
        {
            _eo.doubleVars.at(i) = doubleBounds.at(i).uniform();
        }

        // init int values
        _eo.intVars.resize(intBounds.size());
        for(unsigned i=0;i<_eo.intVars.size();i++)
        {
            _eo.intVars.at(i) = LowTools::round(intBounds.at(i).uniform());
        }

        // init bool values
        _eo.boolVars.resize(nbBool);
        for(unsigned i=0;i<_eo.boolVars.size();i++)
        {
            _eo.boolVars.at(i) = rng.flip();
        }

        // init double standard deviation
        _eo.dblStdDev.resize(doubleBounds.size());
        for(unsigned i=0;i<_eo.dblStdDev.size();i++)
        {
            //_eo.dblStdDev.at(i) = 0.013; // Hinterding, Self-Adaptive Genetic Algorithm
            _eo.dblStdDev.at(i) = doubleBounds.at(i).range()/(sqrt((double)12)*initPopSize);
        }

        // init int mutation probability
        double pIntMut = rng.uniform(1);
        double epsilon = 1E-40;
        if(pIntMut < epsilon)
            pIntMut = epsilon;
        if(pIntMut > 1-epsilon)
            pIntMut = 1-epsilon;
        
        _eo.intProbMut = pIntMut;

        _eo.invalidate();           // was MISSING!!!!
    }

 private:
  std::vector<eoRealInterval> doubleBounds;
  std::vector<eoIntInterval> intBounds;
  unsigned nbBool;
  unsigned initPopSize;
};


#endif 
