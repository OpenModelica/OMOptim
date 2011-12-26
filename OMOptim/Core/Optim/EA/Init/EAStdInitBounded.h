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

 	@file EAStdInitBounded.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 
 	   
    
	Slight modification of file eoRealInitBounded.h (eo Project)
	(c) EEAAX 2000 - Maarten Keijzer 2000
	Contact: Marc.Schoenauer@polytechnique.fr
             mak@dhi.dk
 	
 	THIS PROGRAM IS PROVIDED UNDER THE TERMS OF THIS OSMC PUBLIC
	LICENSE (OSMC-PL). ANY USE, REPRODUCTION OR DISTRIBUTION OF
	THIS PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE OF THE OSMC
	PUBLIC LICENSE.
	 
	The OpenModelica software and the Open Source Modelica
	Consortium (OSMC) Public License (OSMC-PL) are obtained
	from Linkopings University, either from the above address,
	from the URL: http://www.ida.liu.se/projects/OpenModelica
	and in the OpenModelica distribution.
	 
	This program is distributed  WITHOUT ANY WARRANTY; without
	even the implied warranty of  MERCHANTABILITY or FITNESS
	FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
	IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS
	OF OSMC-PL.
	
	See the full OSMC Public License conditions for more details.
	 
	For more information about the Qt-library visit TrollTech's webpage 
	regarding the Qt licence: http://www.trolltech.com/products/qt/licensing.html
  */
// -*- mode: c++; c-indent-level: 4; c++-member-init-indent: 8; comment-column: 35; -*-


#ifndef EASTDINITBOUNDED_H
#define EASTDINITBOUNDED_H

//-----------------------------------------------------------------------------

#include <utils/eoRNG.h>
#include <eoInit.h>
#include <utils/eoRNG.h>
#include <utils/eoRealBounds.h>
#include "LowTools.h"


template <class EOT>
class EAStdInitBounded : public eoInit<EOT>
{
 public:
	 /** Ctor - from eoRealVectorBounds */
	 EAStdInitBounded(
		 std::vector<eoRealInterval> & _doubleBounds,
		 std::vector<eoIntInterval> & _intBounds,
		 unsigned _nbBool
		 ):
	doubleBounds(_doubleBounds),
		 intBounds(_intBounds)
	 {
		 nbBool = _nbBool;
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

		_eo.invalidate();		   // was MISSING!!!!
    }

 private:
  std::vector<eoRealInterval> doubleBounds;
  std::vector<eoIntInterval> intBounds;
  unsigned nbBool;
};


#endif 
