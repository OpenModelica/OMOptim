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

 	@file EAStdMutation.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */

#ifndef EAStdMutation_H
#define EAStdMutation_H

//-----------------------------------------------------------------------------

#include <algorithm>    // swap_ranges
#include <es/eoReal.h>
#include <utils/eoRNG.h>
#include <utils/eoRealBounds.h>

//-----------------------------------------------------------------------------

/** changes all values of the std::vector by uniform choice with range epsilon
with probability p_change per variable
\class eoUniformMutation 
\ingroup parameteric
*/

#include "MOParameter.h"

template<class EOT> class EAStdMutation: public eoMonOp<EOT>
{
public:
	/**
	* Constructor with bounds
	* @param _bounds an eoRealVectorBounds that contains the bounds
	* @param _epsilon the range for uniform mutation - a double to be scaled
	* @param _p_change the one probability to change all coordinates
	*/
	EAStdMutation(
		std::vector<eoRealInterval> & _doubleBounds,
		std::vector<eoIntInterval> & _intBounds,
                MOParameters *_parameters
		):
	doubleBounds(_doubleBounds), intBounds(_intBounds)
	{
                parameters = _parameters;
	}

	/// The class name.
	virtual std::string className() const { return "EAStdMutation"; }

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
                double doublePMut = parameters->value("doublePMut",0.35).toDouble();
                double doubleMutEpsilon = parameters->value("doubleMutEpsilon",0.01).toDouble();

		for (unsigned lieu=0; lieu<_eo.doubleVars.size(); lieu++)
		{
			if (rng.flip(doublePMut))
			{
				// check the bounds
				double emin = _eo.doubleVars[lieu]-doubleMutEpsilon*doubleBounds[lieu].range();
				double emax = _eo.doubleVars[lieu]+doubleMutEpsilon*doubleBounds[lieu].range();
				emin = std::max<double>(doubleBounds[lieu].minimum(), emin);
				emax = std::min<double>(doubleBounds[lieu].maximum(), emax);

				_eo.doubleVars[lieu] = emin + (emax-emin)*rng.uniform();
				hasChanged = true;
			}
		}

		//************************
		// mutation for int
		//************************

                double intPMut = parameters->value("intPMut",0.35).toDouble();

		for (unsigned lieu=0; lieu<_eo.intVars.size(); lieu++)
		{
			if (rng.flip(intPMut))
			{
				// check the bounds
				double emin = intBounds[lieu].minimum();
				double emax = intBounds[lieu].maximum();
				
				int oldValue = _eo.intVars[lieu];
				_eo.intVars[lieu] = LowTools::round(emin + (emax-emin)*rng.uniform());
				
				if(_eo.intVars[lieu]!=oldValue)
					hasChanged = true;
			}
		}

		//************************
		// mutation for bool
		//************************
                double boolPMut = parameters->value("boolPMut",0.35).toDouble();
		for (unsigned lieu=0; lieu<_eo.boolVars.size(); lieu++)
		{
			if (rng.flip(boolPMut))
			{
				_eo.boolVars[lieu] = !_eo.boolVars[lieu];
				hasChanged = true;
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
