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

#include "ProblemInterfaces.h"
#include "Problem.h"
#include "Result.h"
#include "ProblemInterface.h"

bool ProblemInterfaces::addProblemInterface(ProblemInterface* itf)
{
    QStringList problemNames = itf->problemTypes();
    bool oneAdded = false;
    for(int i=0;i<problemNames.size();i++)
    {
        if(!this->contains(problemNames.at(i)))
        {
            insert(problemNames.at(i),itf);
            //emit modified();
            oneAdded = true;
        }
    }

    QStringList resultNames = itf->resultTypes();
    for(int i=0;i<resultNames.size();i++)
    {
        if(!this->contains(resultNames.at(i)))
        {
            insert(resultNames.at(i),itf);
            //emit modified();
            oneAdded = true;
        }
    }
    return oneAdded;
}

bool ProblemInterfaces::addProblemInterfaces(QList<ProblemInterface*> itfs)
{
    bool addedSome = false;
    bool allOk = true;

    for(int i=0;i<itfs.size();i++)
    {
        QStringList problemNames = itfs.at(i)->problemTypes();

        for(int j=0;j<problemNames.size();j++)
        {
            if(this->contains(problemNames.at(j)))
            {
                allOk=false;

            }
            else
            {
                insert(problemNames.at(j),itfs.at(i));
                //emit modified();
                addedSome=true;
            }
        }

    }

    return allOk;
}

bool ProblemInterfaces::removeProblemInterface(QString interfaceName)
{
    int nbRemoved = this->remove(interfaceName);
    if(nbRemoved>0)
    {
        return true;
    }
    return false;
}


ProblemInterface* ProblemInterfaces::interfaceOf(Problem* problem)
{
    return QMap<QString,ProblemInterface*>::value(problem->getClassName(),NULL);
}

ProblemInterface* ProblemInterfaces::interfaceOf(Result* result)
{
    return QMap<QString,ProblemInterface*>::value(result->getClassName(),NULL);
}

ProblemInterface* ProblemInterfaces::interfaceOf(QString caseType)
{
    return QMap<QString,ProblemInterface*>::value(caseType,NULL);
}

QList<ProblemInterface*> ProblemInterfaces::uniqueInterfaces()
{
    // The map stores each interface under several keys (one per problem type and
    // per result type), so values() contains the same interface pointer multiple
    // times. Deduplicate so callers (e.g. the Problems menu) see each interface once.
    QList<ProblemInterface*> res;
    const QList<ProblemInterface*> all = values();
    for(int i=0;i<all.size();i++)
    {
        if(!res.contains(all.at(i)))
            res.append(all.at(i));
    }
    return res;
}
