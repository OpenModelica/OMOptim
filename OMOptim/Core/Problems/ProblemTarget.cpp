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

 	@file ProblemTarget.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "ProblemTarget.h"


ProblemTarget::ProblemTarget(Project* project,EIReader* eiReader)
{
	_type = Problem::PROBLEMEI;
	_name="EI";

	_project = project;
	_eiReader = eiReader;
	_rootEI = new EIItem();
	_inputVars = new MOOptVector(false,false);

	_merResult = new MERResult(project,this,eiReader);
	_connConstrs = new EIConnConstrs(); // connnection constraints
}

ProblemTarget::ProblemTarget(const ProblemTarget &problem)
:ProblemEI(problem)
{
	_eiReader = problem._eiReader;
	_inputVars = problem._inputVars->clone();
	_rootEI = problem._rootEI->clone();
	if(problem._merResult)
		_merResult = new MERResult(*problem._merResult);

	_connConstrs = problem._connConstrs->clone();
}

ProblemTarget::~ProblemTarget(void)
{
	delete _rootEI;
}

QDomElement ProblemTarget::toXMLData(QDomDocument & doc)
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
	QDomElement cEI = _rootEI->toXmlData(doc);
	cProblem.appendChild(cEI);

	// Variables
	QDomElement cResultVars = _inputVars->toXmlData(doc,"ResultVars");
	cProblem.appendChild(cResultVars);

	// Conn constr
	QDomElement cConnConstr = _connConstrs->toXmlData(doc,"ConnConstrs");
	cProblem.appendChild(cConnConstr);

	return cProblem;
}

bool ProblemTarget::checkBeforeComp(QString & error)
{
	return true;
}

void ProblemTarget::clearInputVars()
{
	_inputVars->clear();
}

void ProblemTarget::updateInputVars(MOOptVector *addedVars)
{
	if(addedVars->getUsePoints())
		_inputVars->setUsePoints(true);
	
	if(addedVars->getUseScan())
		_inputVars->setUseScan(true);
	
	_inputVars->append(*addedVars,true);

	emit inputVarsModified();
}


void ProblemTarget::setInputVars(MOOptVector* variables)
{
	clearInputVars();
	updateInputVars(variables);
}

MERResult* ProblemTarget::getMERResult()
{
	return _merResult;
}

MOOptVector * ProblemTarget::inputVars()
{
	return _inputVars;
}
EIConnConstrs* ProblemTarget::connConstrs()
{
	return _connConstrs;
}

void ProblemTarget::launch(ProblemConfig config)
{

	// copy .mod file
	QSettings settings("MO", "Settings");
	QString milpFolder = settings.value("path/MILPFolder").toString();
	
	QDir dir(milpFolder);
	QFileInfo modFileInfo(dir,"MilpTarget.mod");

	if(!modFileInfo.exists())
	{
		infoSender.send(Info(modFileInfo.absoluteFilePath()+" does not exists",ListInfo::ERROR2));
		infoSender.send(Info(ListInfo::PROBLEMEIFAILED));
		emit finished(this);
		return;
	}

	QDir tempDir(config.tempDir);
	QString dataFilePath = tempDir.absoluteFilePath(name()+".dat");

	MilpTarget *milpTarget = new MilpTarget(_rootEI,_connConstrs,_eiReader,_inputVars,modFileInfo.absoluteFilePath(),dataFilePath);
	_result = milpTarget->launch();

	emit finished(this);
}

void ProblemTarget::launchMER(ProblemConfig config,bool includeUtilities)
{

	QList<METemperature> Tk;
	QList<EIStream*> eiStreams;
	QList<QList<MEQflow> > Qik;
	EITools::getTkQik(_eiReader,_inputVars,rootEI,Tk,eiStreams,Qik,!includeUtilities);
	

	CCTools::buildCCfromStreams(Tk,Qik,
		_merResult->curveHot,
		_merResult->curveCold);
	
	CCTools::buildGCCfromStreams(Tk,Qik,
		_merResult->curveGcc);
	
	double TPinch,MER,MERCold;
	CCTools::getValues(Tk,Qik,TPinch,MER,MERCold);
		
	_merResult->TPinch->setFinalValue(0,0,TPinch);
	_merResult->MER->setFinalValue(0,0,MER);
	_merResult->MERCold->setFinalValue(0,0,MERCold);
	_merResult->emitUpdated();
}
