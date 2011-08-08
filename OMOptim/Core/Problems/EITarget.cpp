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


EITarget::EITarget(Project* project,ModClassTree* modClassTree,MOomc* moomc)
    :EIProblem(project,modClassTree,moomc)
{
     _type = Problem::EITARGETTYPE;
     EITargetParameters::setDefaultParameters(_parameters);
}

EITarget::EITarget(Project* project,ModClassTree* modClassTree,MOomc* moomc,QDomElement domProblem)
    :EIProblem(project,modClassTree,moomc,domProblem)
{
    _type = Problem::EITARGETTYPE;

    // Parameters
    EITargetParameters::setDefaultParameters(_parameters);
    QDomElement cParameters = domProblem.firstChildElement("Parameters");
    _parameters->update(cParameters);
}

EITarget::EITarget(const EITarget &problem)
    :EIProblem(problem)
{

    _type = Problem::EITARGETTYPE;
}

Problem* EITarget::clone()
{
    Problem* problem = new EITarget(*this);
    return problem;
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
    QDomElement cConnConstr = _connConstrs->toXmlData(doc,_connConstrs->getClassName());
    cProblem.appendChild(cConnConstr);

    return cProblem;
}

bool EITarget::checkBeforeComp(QString & error)
{
    return eiTree()->isValid(error,inputVars());
}



Result* EITarget::launch(ProblemConfig config)
{
    emit begun(this);
    EITargetResult* result = NULL;


    QString error;
    bool ok = checkBeforeComp(error);
    if(!ok)
        infoSender.send(Info(error,ListInfo::WARNING2));
    else
    {

        QDir tempDir(config.tempDir);
        QString dataFilePath = tempDir.absoluteFilePath(name()+".dat");

        // replace ei references by values
        EITree* filledEI = EIValueFiller::getFilledEI(eiTree(),inputVars(),_project);

        // set within process groups facts to 1
        EIControler::resetProcessFacts(filledEI->rootElement());

        MilpTarget *milpTarget = new MilpTarget(filledEI,_connConstrs,inputVars(),tempDir,dataFilePath);
        result = milpTarget->launch();
        result->setProblem(this->clone());

        delete filledEI;
    }
    emit finished(this);
    return result;
}


