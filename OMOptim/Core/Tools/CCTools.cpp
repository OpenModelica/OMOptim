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

 	@file CCTools.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "CCTools.h"
#include <QtCore/QtAlgorithms>
#include <cmath>
#include "LowTools.h"
#include "MOCCCurve.h"
#include "assert.h"


CCTools::CCTools(void)
{
}

CCTools::~CCTools(void)
{
}

void CCTools::buildCCfromStreams(QList<METemperature> & Tk,
		QList<QList<MEQflow> > & Qik,
		MOCCCurve *hotCurve,
		MOCCCurve *coldCurve)
{
	
	if(Tk.size()==0)
		Tk.push_back(METemperature(0,0));

	QVector<double> Tall;
	QVector<double> HCold,HHot;
	double curQik;

	HCold.push_back(0);
	HHot.push_back(0);

	double curHCold, curHHot;

        curHHot=0;
        curHCold=0;

	for(int iK=0;iK<Tk.size()-1;iK++)
	{
		Tall.push_back(Tk.at(iK).value(METemperature::K));

		for(int iS=0;iS<Qik.size();iS++)
		{
                        curQik = Qik.at(iS).at(iK).value(MEQflow::W);

			if(curQik>0)
				curHCold += curQik;
			else
				curHHot -= curQik;
		}
		HHot.push_back(curHHot);
		HCold.push_back(curHCold);
	}
	Tall.push_back(Tk.at(Tk.size()-1).value(METemperature::K));

	// looking for Tpinch
        METemperature TPinch;
        double dHPinch;
	getPinch(Tall,HCold,HHot,TPinch,dHPinch);


	// offsetting Cold curve (in order to make curves cross at pinch)
	for(int iT=0;iT<Tall.size();iT++)
	{
		HCold[iT] = HCold[iT]-dHPinch;
	}

	// set results
	hotCurve->setData(HHot,Tall);
	coldCurve->setData(HCold,Tall);

	hotCurve->setType(MOCCCurve::CCHOT);
	coldCurve->setType(MOCCCurve::CCCOLD);
}

void CCTools::buildGCCfromStreams(QList<METemperature> & Tk,
		QList<QList<MEQflow> > & Qik,
		MOCCCurve *gccCurve)
{

	MOCCCurve *hotCurve = new MOCCCurve(MOCCCurve::CCHOT);
	MOCCCurve *coldCurve = new MOCCCurve(MOCCCurve::CCCOLD);
	buildCCfromStreams(Tk,Qik,hotCurve,coldCurve);


	QVector<double> dH;
	QVector<double> Tall;

	for(int i=0;i<Tk.size();i++)
	{
		Tall.push_back(Tk.at(i).value(METemperature::K));
		dH.push_back(coldCurve->XData()[i]-hotCurve->XData()[i]);
	}

	gccCurve->setData(dH,Tall);
	gccCurve->setType(MOCCCurve::GCC);
}




void CCTools::getPinch(QVector<double>  Tall,QVector<double>  HCold,QVector<double>  HHot, METemperature & TPinch, double & dHPinch)
{

	QVector<double> HColdAll, HHotAll;

	if(Tall.size()==0)
	{
		TPinch = 0;
		dHPinch = 0;
		return;
	}
	
	// Getting pinch point
	if(Tall.size()>0)
	{
		TPinch = Tall[0];
		dHPinch = HCold[0] - HHot[0];
		for(int iT=1;iT<Tall.size();iT++)
		{
			if(!LowTools::isNan(HCold[iT]) && !LowTools::isNan(HHot[iT]) && ((HCold[iT] - HHot[iT]) < dHPinch))
			{
				dHPinch = HCold[iT] - HHot[iT];
				TPinch = Tall[iT];
			}
		}
	}
	else
		TPinch = -1;
}

void CCTools::getValues(QList<METemperature> & Tk,
						QList<QList<MEQflow> > & Qik,
                                                METemperature &TPinch, MEQflow &MER, MEQflow &MERCold)
{

	MOCCCurve *hotCurve = new MOCCCurve(MOCCCurve::CCHOT);
	MOCCCurve *coldCurve = new MOCCCurve(MOCCCurve::CCCOLD);

	buildCCfromStreams(Tk,Qik,hotCurve,coldCurve);

	double dHPinch;

	QVector<double> HHot = hotCurve->XData();
	QVector<double> HCold = coldCurve->XData();
	QVector<double> Tall = hotCurve->YData();
	QVector<double> Tall2 = coldCurve->YData();
	assert(Tall==Tall2);
	
	getPinch(Tall,HHot,HCold,TPinch,dHPinch);


        MER.setValue(0);
        MERCold.setValue(0);

	if((HHot.size()==0)&&(HCold.size()>0))
	{
            MER.setValue(HCold.at(HCold.size()-1)-HCold.at(0),MEQflow::W);
                MERCold.setValue(0);
		return;
	}

	if((HHot.size()>0)&&(HCold.size()==0))
	{
                MER.setValue(0);
                MERCold.setValue(HHot.at(HHot.size()-1)-HHot.at(0),MEQflow::W);
		return;
	}

	if((HHot.size()>0)&&(HCold.size()>0))
	{
            MERCold.setValue(HCold.at(0)-HHot.at(0),MEQflow::W);
            MER.setValue(HCold.at(HCold.size()-1)-HHot.at(HHot.size()-1),MEQflow::W);
		return;
	}


	return;
}

void CCTools::projectHonT(QVector<double> &newT,QVector<double> &newH,QVector<double> &oldT, QVector<double> &oldH)
{
	int j;
	bool found;

	newH.clear();

	for(int i=0;i<newT.size();i++)
	{

		// getting newH value
		if(oldT.size()==0)
			newH.push_back(0);
		else
		{
			if(newT[i]<oldT[0])
				newH.push_back(oldH[0]);
			else
			{
				if(newT[i]>=oldT[oldT.size()-1])
					newH.push_back(oldH[oldH.size()-1]);
				else
				{
					j=0;
					found = false;
					while((j<oldT.size()-1) && (!found))
					{
						if(newT[i]>=oldT[j] && newT[i]<oldT[j+1])
						{
							found = true;
							newH.push_back(oldH[j] + (newT[i]-oldT[j])/(oldT[j+1]-oldT[j])*(oldH[j+1]-oldH[j]));
						}

						j++;
					}
				}
			}
		}
	}


}

double CCTools::getNumericalFieldValue(EIStream* stream,int iField,MOOptVector* variables,bool &ok)
{
	
	QString variableName = stream->getFieldValue(iField).toString();

	int iVar = variables->findItem(variableName);

	if(iVar==-1)
	{
		ok = false;
		return -1;
	}
	else
	{
		ok = true;
		return variables->items.at(iVar)->finalValue(variables->curScan(),variables->curPoint());
	}
}
