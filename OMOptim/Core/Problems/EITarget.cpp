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
#include "EITarget.h"


EITarget::EITarget(Project* project,ModReader* modReader,MOomc* moomc)
    :EIProblem(project,modReader,moomc)
{
    _inputVars = new MOOptVector(false,false);
    _connConstrs = new EIConnConstrs(); // connnection constraints
}

EITarget::EITarget(Project* project,ModReader* modReader,MOomc* moomc,QDomElement domProblem)
    :EIProblem(project,modReader,moomc)
{

    _inputVars = new MOOptVector(false,false);
    _connConstrs = new EIConnConstrs(); // connnection constraints


    QDomElement domInfos = domProblem.firstChildElement("Infos");
    QString problemName = domInfos.attribute("name");

    // Infos
    setType((Problem::ProblemType)domInfos.attribute("type", "" ).toInt());
    setName(problemName);

    // EI
    QDomElement domEI = domProblem.firstChildElement("EIItem");
    EIControler::setItems(domEI,eiTree()->rootElement());

    // InputVars
    QDomElement domInputVars = domProblem.firstChildElement("InputVars");
    inputVars()->setItems(domInputVars);

    //**************
    // Result
    //**************
    QDomElement domResult = domProblem.firstChildElement("Result");
    if(!domResult.isNull())
    {
        EITargetResult* result = new EITargetResult(_project,this);
        result->setSuccess(true);

        // EI
        QDomElement domEI = domResult.firstChildElement("EIItem");
        EIControler::setItems(domEI,result->eiTree()->rootElement());

        // Values
        QDomElement cValues = domResult.firstChildElement("Values");
        QString totalCost = cValues.attribute("TotalCost");
        result->totalCost = totalCost.toDouble();

        // EIConns
        QDomElement domEIConns = domResult.firstChildElement("EIConns");
        result->eiConns()->setItems(domEIConns,result->eiTree());

        QDomElement cFiles = domResult.firstChildElement("Files");
        result->_logFileName = cFiles.attribute("LogFile");
        result->_resFileName = cFiles.attribute("ResFile");
        result->_sensFileName = cFiles.attribute("SensFile");

        setResult(result);
    }
}

EITarget::EITarget(const EITarget &problem)
    :EIProblem(problem)
{
    _inputVars = problem._inputVars->clone();
    _connConstrs = problem._connConstrs->clone();
}

EITarget::~EITarget(void)
{
}

QDomElement EITarget::toXmlData(QDomDocument & doc)
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
    QDomElement cEI = _eiTree->toXmlData(doc);
    cProblem.appendChild(cEI);

    // Variables
    QDomElement cResultVars = _inputVars->toXmlData(doc,"ResultVars");
    cProblem.appendChild(cResultVars);

    // Conn constr
    QDomElement cConnConstr = _connConstrs->toXmlData(doc,"ConnConstrs");
    cProblem.appendChild(cConnConstr);

    return cProblem;
}

bool EITarget::checkBeforeComp(QString & error)
{
    return true;
}


EIConnConstrs* EITarget::connConstrs()
{
    return _connConstrs;
}


void EITarget::launch(ProblemConfig config)
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

    // replace ei references by values
    EITree* filledEI = EIValueFiller::getFilledEI(eiTree(),inputVars(),_project);

    // set within process groups facts to 1
    EIControler::resetProcessFacts(filledEI->rootElement());

    MilpTarget *milpTarget = new MilpTarget(filledEI,_connConstrs,inputVars(),tempDir,modFileInfo.absoluteFilePath(),dataFilePath);
    _result = milpTarget->launch();

    delete filledEI;
    emit finished(this);
}


