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

 	@file EIMER.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "EIMER.h"


EIMER::EIMER(Project* project,ModReader* modReader,MOomc* moomc)
    :EIProblem(project,modReader,moomc)
{
	_inputVars = new MOOptVector(false,false);
}

EIMER::EIMER(const EIMER &problem)
:EIProblem(problem)
{
    _inputVars = problem._inputVars->clone();
}

EIMER::~EIMER(void)
{
}

QDomElement EIMER::toXmlData(QDomDocument & doc)
{
	QDomElement cProblem = doc.createElement(getClassName());
	//***********************
	// Problem definition
	//***********************
	QDomElement cInfos = doc.createElement("Infos");
	cProblem.appendChild(cInfos);
	cInfos.setAttribute("name", name());
	cInfos.setAttribute("type", type());
	
	// EI
        QDomElement cEI = eiTree()->toXmlData(doc);
	cProblem.appendChild(cEI);

	// Variables
	QDomElement cResultVars = _inputVars->toXmlData(doc,"ResultVars");
	cProblem.appendChild(cResultVars);

	return cProblem;
}

bool EIMER::checkBeforeComp(QString & error)
{
	return true;
}

void EIMER::setIncludeUtilities(bool include)
{
        _includeUtilities = include;
}
void EIMER::launch(ProblemConfig config)
{

    deleteResult();
    _result = new EIMERResult();
    EIMERResult *curResult = _result; // just a shortcut to avoid cast

    QList<METemperature> Tk;
    QList<EIStream*> eiStreams;
    QList<QList<MEQflow> > Qik;

    //replace ei references by values
    EITree* filledEI = EIValueFiller::getFilledEI(_eiTree,inputVars(),_project);

    EITools::getTkQik(_inputVars,filledEI->rootElement(),Tk,eiStreams,Qik,!_includeUtilities);


    CCTools::buildCCfromStreams(Tk,Qik,
                                curResult->curveHot,
                                curResult->curveCold);

    CCTools::buildGCCfromStreams(Tk,Qik,
                                 curResult->curveGcc);

    METemperature TPinch;
    MEQflow MER,MERCold;

    CCTools::getValues(Tk,Qik,TPinch,MER,MERCold);


    curResult->TPinch = TPinch;
    curResult->MER = MER;
    curResult->MERCold = MERCold;
    curResult->emitUpdated();

    emit finished(this);
}
