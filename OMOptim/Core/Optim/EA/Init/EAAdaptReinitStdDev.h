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