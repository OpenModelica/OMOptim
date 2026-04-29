/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
 * VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the OSMC (Open Source Modelica Consortium)
 * Public License (OSMC-PL) are obtained from OSMC, either from the above
 * address, from the URLs:
 * http://www.openmodelica.org or
 * https://github.com/OpenModelica/ or
 * http://www.ida.liu.se/projects/OpenModelica,
 * and in the OpenModelica distribution.
 *
 * GNU AGPL version 3 is obtained from:
 * https://www.gnu.org/licenses/licenses.html#GPL
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

#include "onesimulationinterface.h"

#include "TabOneSim.h"
#include "TabResOneSim.h"


Problem* OneSimulationInterface::createNewProblem(ProjectBase* projectBase,const QStringList modelsList,QString problemType)

{
    Q_ASSERT(problemType==OneSimulation::className());

    if(modelsList.size()!=1)
    {
        InfoSender::instance()->send(Info("Model for one simulation problem not defined",ListInfo::ERROR2));
        return NULL;
    }
    else
    {
        Project* project = dynamic_cast<Project*>(projectBase);
        if(!project)
            return NULL;
        else
            return new OneSimulation(project,project->modelPlus(modelsList.at(0)));
    }
}


QWidget* OneSimulationInterface::createProblemTab(Problem * problem,QWidget* parent)
{
    OneSimulation* oneSim = dynamic_cast<OneSimulation*>(problem);
    return new TabOneSim(oneSim,parent);
}

QWidget* OneSimulationInterface::createResultTab(Result* result,QWidget* parent)
{
    OneSimResult* oneSimResult = dynamic_cast<OneSimResult*>(result);
    return new TabResOneSim(oneSimResult,parent);
}


Problem* OneSimulationInterface::loadProblem(QFileInfo saveFile,const QDomElement & domOMCase, ProjectBase * projectBase)
{
    if(domOMCase.isNull() || domOMCase.tagName()!="OMCase" )
        return NULL;

    Project* project = dynamic_cast<Project*>(projectBase);
    if(!project)
        return NULL;

   QDomElement domOMProblem = domOMCase.firstChildElement("OMProblem");


    Problem* problem = NULL;
    bool ok = true;

    QDomElement domProblem = domOMProblem.firstChildElement(OneSimulation::className());

    problem = new OneSimulation(domProblem,project,ok);

    if(!ok)
    {
        delete problem;
        problem = NULL;
    }
    else
    {
        problem->setEntireSavePath(saveFile.absoluteFilePath());
    }

    return problem;
}



Result* OneSimulationInterface::loadResult(QFileInfo saveFile,const QDomElement & domOMCase, ProjectBase * projectBase)
{
    if(domOMCase.isNull() || domOMCase.tagName()!="OMCase" )
        return NULL;


    Project* project = dynamic_cast<Project*>(projectBase);
    if(!project)
        return NULL;


    // read problem
    bool ok;
    QDomElement domOMProblem = domOMCase.firstChildElement("OMProblem");
    QDomElement domProblem = domOMProblem.firstChildElement(OneSimulation::className());
    OneSimulation oneSim(domProblem,project,ok);

    if(!ok)
    {
        InfoSender::instance()->send( Info(ListInfo::RESULTFILECORRUPTED,saveFile.filePath()));
        return NULL;
    }

    // create result
    QDomElement domOMResult = domOMCase.firstChildElement("OMResult");
    QDomElement domResult = domOMResult.firstChildElement(OneSimResult::className());
    Result* result = new OneSimResult(project,domResult,oneSim,ok);

    if(!result)
    {
        InfoSender::instance()->send( Info(ListInfo::RESULTFILECORRUPTED,saveFile.filePath()));
        return NULL;
    }

    // attribute problem to result
    if(result)
    {
        // attribute file path to result
        result->setEntireSavePath(saveFile.filePath());
    }

    return result;
}
