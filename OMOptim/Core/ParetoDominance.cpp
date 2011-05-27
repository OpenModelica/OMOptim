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
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)

 	@file ParetoDominance.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "ParetoDominance.h"

ParetoDominance::ParetoDominance()
{

		
}

ParetoDominance::~ParetoDominance(void)
{
}


QList<int> ParetoDominance::getParetoSet(MOVector<OptObjective>* _objs,MOOptVector* _objResults)
{
	if((_objResults->items.size()==0)||(_objs->items.size()!=_objResults->items.size()))
		return QList<int>();

	// Fulling set
	QList<int> paretoSet;
	for(int i=0;i<_objResults->items.at(0)->nbPoints();i++)
		paretoSet.push_back(i);

	// Removing points by points when dominated
	int index=0;
	int index2;
	bool indexDominated;
	while(index<paretoSet.size())
	{
		indexDominated = false;
		index2=0;
		while(!indexDominated && index2<paretoSet.size())
		{
			if(index!=index2)
				indexDominated = dominates(_objs,_objResults,paretoSet.at(index2),paretoSet.at(index));
			
			index2++;
		}
		if(indexDominated)
		{
			paretoSet.removeAt(index);
		}
		else
		{
			index++;
		}
	}

	return paretoSet;
}





bool ParetoDominance::dominates(MOVector<OptObjective>* _objs,MOOptVector* _objResults, int iPoint1, int iPoint2)
{
	// return true if iPoint1 better than iPoint2
	double tolerance = 1e-10;

	QList<bool> dom;
	
	if(_objResults->items.size()==0)
		return false;

	for (unsigned int iObj=0; iObj<_objResults->items.size(); iObj++)
	{
		// first, we have to check if the 2 objective values are not equal for the ith objective
		if ( fabs(_objResults->items.at(iObj)->finalValue(0,iPoint1) - _objResults->items.at(iObj)->finalValue(0,iPoint2)) > tolerance)
		{
			// if the ith objective have to be minimized...
			if (_objs->items.at(iObj)->isMinimized())
			{
				if (_objResults->items.at(iObj)->finalValue(0,iPoint1) > _objResults->items.at(iObj)->finalValue(0,iPoint2))
				{
					dom.push_back(false);		// iPoint1 is not better than iPoint2
				}
				else
				{
					dom.push_back(true);	// iPoint2 is not better than iPoint1
				}
			}
			// if the ith objective have to be maximized...
			else
			{
				if (_objResults->items.at(iObj)->finalValue(0,iPoint1) < _objResults->items.at(iObj)->finalValue(0,iPoint2))
				{
					dom.push_back(false);		// iPoint1 is not better than iPoint2
				}
				else
				{
					dom.push_back(true);	// iPoint2 is not better than iPoint1
				}
			}
		}
		else
			dom.push_back(true);
	}
	
	bool allDoms=true;
	for(int i=0;i<dom.size();i++)
	{
		allDoms = allDoms && dom.at(i);
	}

	return allDoms;
}
