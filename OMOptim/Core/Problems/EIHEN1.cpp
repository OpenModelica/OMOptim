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

        @file EIHEN1.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "EIHEN1.h"


EIHEN1::EIHEN1(Project* project,ModClassTree* modClassTree,MOomc* moomc)
    :EIProblem(project,modClassTree,moomc)
{
    _type = Problem::EIHEN1;
    EIHEN1Parameters::setDefaultParameters(_parameters);
}

EIHEN1::EIHEN1(const EIHEN1 &problem)
    :EIProblem(problem)
{
     _type = Problem::EIHEN1;
}

Problem* EIHEN1::clone()
{
    Problem* problem = new EIHEN1(*this);
    return problem;
}

EIHEN1::~EIHEN1(void)
{
}


QDomElement EIHEN1::toXmlData(QDomDocument & doc)
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

    // Parameters
    QDomElement cParameters = _parameters->toXmlData(doc,"Parameters");
    cProblem.appendChild(cParameters);

    return cProblem;
}

bool EIHEN1::checkBeforeComp(QString & error)
{
    return true;
}

QString EIHEN1::infos()
{
    QString infos;
    infos += "EIHEN1 solves a Heat Exchanger Network problem. Algorithm is based on following article : \n";
    infos += "Barbaro, A. & Bagajewicz, M. J. New rigorous one-step MILP formulation for heat exchanger network synthesis,";
    infos += "Computers & Chemical Engineering, 2005, 29, 1945-1976. \n \n";

    infos+= "- unchecked streams or groups are removed\n";
    infos+= "- DTmin/2 are not considered : original temperatures are considered in problem, not corrected\n";
}

Result* EIHEN1::launch(ProblemConfig config)
{

    // copy .mod file
    QSettings settings("MO", "Settings");
    QString milpFolder = settings.value("path/MILPFolder").toString();

    QDir dir(milpFolder);
    QFileInfo modFileInfo(dir,"MilpHEN.mod");

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

    // reset DTmin/2 to 0
    EIControler::resetAllDTMin_2(filledEI->rootElement());

    // remove unchecked eiitems
    filledEI->removeUnchecked();

    //replace eiTree wiyh filledEI
    setEITree(filledEI);

    MilpHEN1 *milpHEN1 = new MilpHEN1(filledEI,_parameters,_connConstrs,inputVars(),tempDir,modFileInfo.absoluteFilePath(),dataFilePath);
    EIHEN1Result* result = milpHEN1->launch();
    result->setProblem(this->clone());


    emit finished(this);


   return result;
}

