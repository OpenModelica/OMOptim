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

// $Id$
 /**
     @file EAAdaptReinitStdDev.h
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version 0.9

  */

#ifndef EAAdapt1ReinitStdDev_H
#define EAAdapt1ReinitStdDev_H

//-----------------------------------------------------------------------------

#include <utils/eoRNG.h>
#include <eoInit.h>
#include <utils/eoRNG.h>
#include <utils/eoRealBounds.h>
#include "LowTools.h"


template <class EOT>
class EAAdaptReinitStdDev
{
public:
    static void reinitDblStdDev(eoPop<EOT> & _pop,std::vector<eoRealInterval> doubleBounds,int initPopSize)
    {
        EOT _eo;
        for(int iPop=0;iPop<_pop.size();iPop++)
        {
            _eo = _pop[iPop];
            for(unsigned j=0;j<_eo.dblStdDev.size();j++)
            {
                //_eo.dblStdDev.at(i) = 0.013; // Hinterding, Self-Adaptive Genetic Algorithm
                _eo.dblStdDev.at(j)= doubleBounds.at(j).range()/(sqrt((double)12)*initPopSize);
            }
        }
    }
};

#endif