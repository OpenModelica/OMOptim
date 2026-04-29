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

#ifndef PSOBREED_H
#define PSOBREED_H
#include "eoBreed.h"
#include "utils/eoIntBounds.h"
#include "utils/eoRealBounds.h"




template<class EOT>
class PSOBreed
{

   public:

    void operator ()(eoPop <EOT> & pop, const eoPop <EOT> & pbest, const EOT & gbest, const std::vector<EOT> & velocity, const int & nbdouble, const int & nbint, const int & nbbool, const std::vector<eoRealInterval> & doubleBounds, const std::vector<eoIntInterval> & intBounds)
    {
        for(int i =0; i < pop.size(); i++)
        {
            for (int j=0; j< nbdouble; j++)
            {
                (pop[i].doubleVars)[j]            +=  (velocity[i].doubleVars)[j];

                // limit conditions
                if((pop[i].doubleVars)[j]   >  doubleBounds[j].maximum())
                    (pop[i].doubleVars)[j]  =  doubleBounds[j].maximum();

                else if ((pop[i].doubleVars)[j]  <  doubleBounds[j].minimum())
                    (pop[i].doubleVars)[j]       =  doubleBounds[j].minimum();

            }

            for(int j=0; j< nbint; j++)
            {
                (pop[i].intVars)[j]  += (velocity[i].intVars)[j];

                if((pop[i].intVars)[j]   >  intBounds[j].maximum())
                    (pop[i].intVars)[j]  =  intBounds[j].maximum();

                else if ((pop[i].intVars)[j]  <  intBounds[j].minimum())
                    (pop[i].intVars)[j]       =  intBounds[j].minimum();


            }

            for(int j=0; j< nbbool; j++)
            {
                (pop[i].boolVars)[j] = (velocity[i].boolVars)[j];
            }

            pop[i].invalidateObjectiveVector();
        }

    }

//void operator ()(eoPop <EOT> & pop1, eoPop <EOT> & pop2)
//{}

};


#endif // PSOBREED_H
