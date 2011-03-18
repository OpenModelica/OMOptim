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

 	@file SBCrossover.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#ifndef SBCROSSOVER_H
#define SBCROSSOVER_H

#include <eoOp.h>
#include <stdio.h>
#include <math.h>

#include "LowTools.h"

template<class EOT>
class SBCrossover: public eoQuadOp<EOT>
{
public:
  /// Ctor
  SBCrossover(EAConfig* _config):eoQuadOp<EOT>() {config = _config;}
  virtual std::string className() const {return "EA2Crossover";}
virtual bool operator()(EOT& _eoA, EOT& _eoB ) {
	
	// for double, int and bool, both eos are modified, new values are taken between two old values)
	// !! need to be studied and adapted to specific problems

	bool oneHasChanged = false;

	int iVar;
	double childValueA, childValueB;
	double parValueA, parValueB;
	
	// using SBX described in Sareni & al., Recombination and self-adapation in Multi-objective GA
	double beta;
	double eta=1;
	double u;
		
	//********************************
	// Crossover on double variables 
	//********************************
	for(int iVar=0;iVar<_eoA.doubleVars.size();iVar++)
	{
		u = rng.random(1);
		if(u>0.5)
			beta= pow(2*u,1/(eta+1));
		else
			beta = pow(1/(2*(1-u)),1/(eta+1));

		parValueA = _eoA.doubleVars.at(iVar);
		parValueB = _eoB.doubleVars.at(iVar);

		childValueA = 0.5*((1+beta)*parValueA + (1-beta)*parValueB);
		childValueB = 0.5*((1-beta)*parValueA + (1+beta)*parValueB);

		_eoA.doubleVars.at(iVar) = childValueA;
		_eoB.doubleVars.at(iVar) = childValueB;

		oneHasChanged = true;
	}
	
	//********************************
	// Crossover on int variables 
	//********************************
	for(int iVar=0;iVar<_eoA.intVars.size();iVar++)
	{
		u = rng.random(1);
		if(u>0.5)
			beta= pow(2*u,1/(eta+1));
		else
			beta = pow(1/(2*(1-u)),1/(eta+1));

		parValueA = (double)_eoA.intVars.at(iVar);
		parValueB = (double)_eoB.intVars.at(iVar);

		childValueA = 0.5*((1+beta)*parValueA + (1-beta)*parValueB);
		childValueB = 0.5*((1-beta)*parValueA + (1+beta)*parValueB);
		
		_eoA.intVars.at(iVar) = LowTools::round(childValueA);
		_eoB.intVars.at(iVar) = LowTools::round(childValueB);

		oneHasChanged = true;
	}

	//********************************
	// Crossover on bool variables 
	//********************************
	for(int iVar=0;iVar<_eoA.boolVars.size();iVar++)
	{
		iVar = LowTools::round(rng.random(_eoA.boolVars.size()-1));
		bool valB =_eoB.boolVars.at(iVar);
		bool valA =_eoA.boolVars.at(iVar);

		_eoA.boolVars.at(iVar)=valB;
		_eoB.boolVars.at(iVar)=valA;

		oneHasChanged = true;
	}
	
	return oneHasChanged;
}
private :

	EAConfig *config;


};

#endif
