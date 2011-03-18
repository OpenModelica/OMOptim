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

 	@file EAStdBounds.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#ifndef _EASTDBOUNDS_H
#define _EASTDBOUNDS_H


#include "Optimization.h"
#include "VariableType.h"

#include <QtCore/QObject>


class EAStdBounds
{
public :
inline static void setBounds(Optimization*,
					  QList<ModModelPlus*> _subModels,
					  std::vector<eoRealInterval> &doubleBounds,
						std::vector<eoIntInterval> &intBounds,
						int &nbDouble,int &nbInt,int &nbBool);


//inline static void setBounds(VariableDet*,
//					   QVector<QVector< int> > &index,
//					  std::vector<eoRealInterval> &doubleBounds,
//						std::vector<eoIntInterval> &intBounds,
//						int &nbDouble,int &nbInt,int &nbBool);
};





void EAStdBounds::setBounds(Optimization *_problem, QList<ModModelPlus*> _subModels,std::vector<eoRealInterval> &doubleBounds, std::vector<eoIntInterval> &intBounds, int &nbDouble, int &nbInt, int &nbBool)
{
	int nbVar = _problem->optimizedVariables()->items.size();
	OptVariable* curVar;
	eoRealInterval* realInterval;
	eoIntInterval* intInterval;
	double min,max;
	for(int i=0; i<nbVar; i++)
	{
		curVar = _problem->optimizedVariables()->items.at(i);
		min = curVar->getFieldValue(OptVariable::OPTMIN).toDouble();
		max = curVar->getFieldValue(OptVariable::OPTMAX).toDouble();

		switch(curVar->getFieldValue(OptVariable::DATATYPE).toInt())
		{
                case OMREAL:
			realInterval = new eoRealInterval(min,max);
			doubleBounds.push_back(*realInterval);
			nbDouble++;
			break;
                case OMINTEGER:
			intInterval = new eoIntInterval((int)min,(int)max);
			intBounds.push_back(*intInterval);
			nbInt++;
			break;

                case OMBOOLEAN:
			//No need to specify bounds for boolean...
			nbBool++;
			break;
		}
	}

	//Adding int variable corresponding to subModelSelection
	if(_subModels.size()>1)
	{
		intInterval = new eoIntInterval((int)0,_subModels.size()-1);
		intBounds.push_back(*intInterval);
		nbInt++;
	}
}

//
//void EAStdBounds::setBounds(VariableDet* _problem, QVector<QVector< int> > &index,
//					  std::vector<eoRealInterval> &doubleBounds,
//						std::vector<eoIntInterval> &intBounds,
//						int &nbDouble,int &nbInt,int &nbBool)
//{
//
//	int nbVars = _problem->fuzzyVars->getNbVars();
//
//	FuzzyVariable* curVar;
//	eoRealInterval* realInterval;
//	double min,max;
//	double curAbsPrec;
//	double curValue;
//
//	int nbPoints = _problem->fuzzyVars->nbPoints();
//	int iDouble=0;
//
//	for(int iVar=0; iVar<nbVars; iVar++)
//	{
//
//		// Point independent vars
//		if(_problem->fuzzyVars->getStatus(iVar)==FuzzyVariable::POINTINDEP)
//		{
//			// get interval
//			curValue = _problem->fuzzyVars->getValue(iVar);
//			curAbsPrec = _problem->fuzzyVars->getAbsPrecValue(iVar,0);
//
//			min = curValue - fabs(curAbsPrec);
//			max = curValue + fabs(curAbsPrec);
//
//			if(min<max)
//			{
//			// add interval
//			realInterval = new eoRealInterval(min,max);
//			doubleBounds.push_back(*realInterval);
//
//			// fill index table
//			index.push_back(QVector<int>(nbPoints,iDouble));
//			iDouble++;
//			}
//			else
//			{
//				// curAbsPrec = 0, fixed var -> no interval
//				index.push_back(QVector<int>(nbPoints,-1));
//			}
//		}
//
//		// Point dependent vars
//		if(_problem->fuzzyVars->getStatus(iVar)==FuzzyVariable::POINTDEP)
//		{
//			QVector<int> varIndex;
//			for(int iPoint=0;iPoint<nbPoints;iPoint++)
//			{
//				// get interval
//				curValue = _problem->fuzzyVars->getValue(iVar,iPoint);
//				curAbsPrec = _problem->fuzzyVars->getAbsPrecValue(iVar,iPoint);
//				
//				min = curValue - fabs(curAbsPrec);
//				max = curValue + fabs(curAbsPrec);
//
//				// add interval
//				if(min<max)
//				{
//					realInterval = new eoRealInterval(min,max);
//					doubleBounds.push_back(*realInterval);
//					// fill index table
//					varIndex.push_back(iDouble);
//					iDouble++;
//				}
//				else
//				{
//					// var absPrec = 0;
//					varIndex.push_back(-1);
//				}				
//			}
//			index.push_back(varIndex);
//		}
//	}
//	nbDouble = iDouble;
//	nbInt = 0;
//	nbBool = 0;
//}








#endif
