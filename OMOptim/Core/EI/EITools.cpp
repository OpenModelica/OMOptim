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

 	@file EITools.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "EITools.h"
#include <cmath>

EITools::EITools(void)
{
}

EITools::~EITools(void)
{
}



void EITools::getTkQik(MOOptVector *variables,
					   EIItem* rootEI,QList<METemperature> & Tk,
					   QList<EIStream*> & eiStreams, QList<QList<MEQflow> > & Qik, bool onlyProcess)
{
	// in this function, Quk are multiplied by facMul of EIGroup (if EIGroup is enabled, else not added)

	QList<EIStream*> eiProcessStreams;
	QList<EIStream*> eiUtilityStreams;
	QList<QList<MEQflow> > Qpk;// Qpk.at(iStream).at(iDTk)
	QList<QList<MEQflow> > Quk;// Quk.at(iStream).at(iDTk)
	QMultiMap<EIGroupFact*,EIStream*> factStreamMap;// multimap <unit multiplier, Streams concerned>,
	QMap<EIGroupFact*,EIGroupFact*> factRelation; // map<child unit multiplier, parent unit multiplier> for constraint (e.g. fchild <= fparent * fchildmax)
	QMap<EIGroupFact*,EIGroup*> factGroupMap;


    getTkQpkQuk(variables,rootEI,Tk,eiProcessStreams, Qpk,
						  eiUtilityStreams, Quk, factStreamMap, factRelation, factGroupMap);

	Qik = Qpk;
	eiStreams = eiProcessStreams;
	EIGroup* curGroup;
	EIGroupFact* curGroupFact;
	QList<MEQflow> curQk;
	// copying utility 
	if(!onlyProcess)
	{
		for(int iS=0;iS<eiUtilityStreams.size();iS++)
		{
			curGroupFact = factStreamMap.key(eiUtilityStreams.at(iS));
			curGroup = factGroupMap.value(curGroupFact);

			// multiply heatload by multiplication factor
			if(curGroup->isChecked())
			{
				curQk.clear();
				for(int ik=0;ik<Quk.at(iS).size();ik++)
					curQk.push_back(MEQflow(Quk.at(iS).at(ik).value(0)*curGroupFact->value,MEQflow::W));
				Qik.push_back(curQk);
				eiStreams.push_back(eiUtilityStreams.at(iS));
			}
		}
	}
}

void EITools::getTkQpkQuk(MOOptVector *variables,
						  EIItem* rootEI,QList<METemperature> & Tk,
						  QList<EIStream*> & eiProcessStreams, QList<QList<MEQflow> > & Qpk,// Qpk.at(iStream).at(iDTk)
						  QList<EIStream*> & eiUtilityStreams, QList<QList<MEQflow> > & Quk,// Quk.at(iStream).at(iDTk)
						  QMultiMap<EIGroupFact*,EIStream*> &factStreamMap, // multimap <unit multiplier, Streams concerned>,
						  QMap<EIGroupFact*,EIGroupFact*> &factRelation, // map<child unit multiplier, parent unit multiplier> for constraint (e.g. fchild <= fparent * fchildmax)
						  QMap<EIGroupFact*,EIGroup*> &factGroupMap
						  )
{

	//***************
	// get Tk
	//***************
    EIReader::getValidTk(rootEI,Tk,variables);

	//*************************
	// get Quk,Qpk and streams
	//*************************
    QList<EIStream*> streams = EIReader::getValidStreams(rootEI,variables,true);
	EIStream* curStream;
	EIGroupFact* curGroupFact;
	EIGroupFact* curGroupFact2;
	EIGroup* curGroup;
	EIGroup* curGroup2;

	bool ok1;
	bool ok2;
	bool ok3;
    bool ok;
	double TinProv,ToutProv,QflowProv,DTmin2prov;
	QList<MEQflow> DQprov;
    MEQflow curDQ;
    QString msg;
    bool isHotStream;

	for(int iS=0;iS<streams.size();iS++)
	{
		curStream = streams.at(iS);
        TinProv = curStream->TinNum.value(METemperature::K);
        ToutProv = curStream->ToutNum.value(METemperature::K);
        QflowProv  = curStream->QflowNum.value(MEQflow::W);
		DTmin2prov = curStream->getFieldValue(EIStream::DTMIN2).toDouble();

        if(TinProv==ToutProv)
        {
            msg.clear();
            msg.sprintf("In stream %s, Tin = Tout. This stream won't be considered",curStream->name().utf16());
            infoSender.send(Info(msg,ListInfo::WARNING2));
            ok=false;
        }

		if(TinProv<ToutProv)
		{
			//Cold stream
			TinProv += DTmin2prov;
			ToutProv += DTmin2prov;
            isHotStream = false;
		}
		else
		{
			//Hot stream
			TinProv += -DTmin2prov;
			ToutProv += -DTmin2prov;
            isHotStream = true;
		}

		DQprov.clear();
		double DT;
		for(int iT=0;iT<Tk.size()-1;iT++)
		{
            curDQ = EIReader::getIntervalQFlow(Tk.at(iT+1),Tk.at(iT),curStream);
            if(isHotStream)// curDQ should be negative if Tout<Tin
                curDQ = curDQ*-1;
            DQprov.push_back(curDQ);
			}

		// utility/process stream
        EIReader::getFirstParentGroupFact(curStream,curGroupFact,curGroup);

		// if is part of a utility
		if(curGroupFact)
		{
			eiUtilityStreams.push_back(curStream);
			Quk.push_back(DQprov);
			factStreamMap.insert(curGroupFact,curStream);
			factGroupMap.insert(curGroupFact,curGroup);

			// check if utility group is part of another utility group
            EIReader::getFirstParentGroupFact(curGroup,curGroupFact2,curGroup2);
			if(curGroupFact2)
				factRelation.insert(curGroupFact,curGroupFact2); //add relation in order to be taken into account
		}
		else
		{
			// stream is a process stream
			eiProcessStreams.push_back(curStream);
			Qpk.push_back(DQprov);
		}
	}
}


double EITools::DTlm(METemperature T1in,METemperature T1out,METemperature T2in,METemperature T2out)
{
    bool T1hoter = T1in > T2out;
    if(T1hoter)
        return DTlm(T1in-T2out,T1out-T2in);
    else
        return DTlm(T2in-T1out,T2out-T1in);
}

double EITools::DTlm(double dT1,double dT2)
{
    METemperature result;

    if(dT1==dT2)
        return dT1;

    if((dT1==0)||(dT2==0))
    {
        infoSender.send(Info("Unable to compute DTlm with dT=1",ListInfo::WARNING2));
        return 0;
    }

    return (dT1-dT2)/log(dT1/dT2);
}
