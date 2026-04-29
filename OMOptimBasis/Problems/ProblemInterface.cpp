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

#include <QStringList>
#include <QSet>

#include "ProblemInterface.h"
#include "Problem.h"
#include "Result.h"
#include "MOParameter.h"



ProblemInterface::ProblemInterface()
{
    initParameters();
}

bool ProblemInterface::saveProblem(Problem* problem)
{
    // Root element
    QDomDocument doc("OMCase");
    QDomElement domOMCase = doc.createElement("OMCase");
    doc.appendChild(domOMCase);

    QDomElement problemRoot = doc.createElement("OMProblem");
    QDomElement problemEl = problem->toXmlData(doc);
    domOMCase.appendChild(problemRoot);
    problemRoot.appendChild(problemEl);

    // Writing to file
    if(!problem->saveFileName().isEmpty())
    {
        QFile file(problem->entireSavePath());
        QFileInfo fileInfo(problem->entireSavePath());
        QDir dir = fileInfo.absoluteDir();
        dir.mkpath(dir.absolutePath());

        if(file.exists())
        {
            file.remove();
        }
        file.open(QIODevice::WriteOnly);
        QTextStream ts( &file );
        ts << doc.toString();
        file.close();
    }

    problem->setIsSaved(true);

    return true;
}

bool ProblemInterface::saveResult(Result* result)
{
    // Root element
    QDomDocument doc("OMCase");
    QDomElement domOMCase = doc.createElement("OMCase");
    doc.appendChild(domOMCase);

    // OMCase attributes
    domOMCase.setAttribute("IsAResult","true");
    domOMCase.setAttribute("ProblemType",result->problem()->getClassName());

    //*********************
    // Problem definition
    //*********************
    Problem* problem = result->problem();
    QDomElement problemRoot = doc.createElement("OMProblem");
    QDomElement problemEl = problem->toXmlData(doc);
    domOMCase.appendChild(problemRoot);
    problemRoot.appendChild(problemEl);


    //*********************
    // Result definition
    //*********************
    QDomElement resultRoot = doc.createElement("OMResult");
    QDomElement resultEl = result->toXmlData(doc);
    resultRoot.appendChild(resultEl);
    domOMCase.appendChild(resultRoot);

    // Writing to file
    if(!result->saveFileName().isEmpty())
    {
        QFile file(result->entireSavePath());
        QFileInfo fileInfo(result->entireSavePath());
        QDir dir = fileInfo.absoluteDir();
        dir.mkpath(dir.absolutePath());

        if(file.exists())
        {
            file.remove();
        }
        file.open(QIODevice::WriteOnly);
        QTextStream ts( &file );
        ts << doc.toString();
        file.close();
    }

    result->setIsSaved(true);

    return true;
}

void ProblemInterface::initParameters()
{
    _parameters = new MOParameters();
}

MOParameters* ProblemInterface::parameters()
{
    return _parameters;
}


