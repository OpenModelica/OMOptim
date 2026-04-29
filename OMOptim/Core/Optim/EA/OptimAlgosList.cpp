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

#include "OptimAlgosList.h"
#include "OptimAlgo.h"
#include "NSGA2.h"
#include "SPEA2.h"
#include "SPEA2Adapt.h"
#include "SA1.h"
#include "PSO.h"


OptimAlgo* OptimAlgosList::getNewAlgo(Project* project,Problem* problem,OptimAlgosList::Type iAlgo)
{
    switch(iAlgo)
    {
    case OptimAlgosList::iNSGA2:
        return new NSGA2(project,problem);
    case OptimAlgosList::iSPEA2:
        return new SPEA2(project,problem);
    case OptimAlgosList::iSPEA2Adapt:
        return new SPEA2Adapt(project,problem);
    case OptimAlgosList::iSA1:
        return new SA1(project,problem);
    case OptimAlgosList::iPSO:
        return new PSO(project,problem);
    default:
        //Problem
        return NULL;
    }
}

QStringList OptimAlgosList::getCommonParametersNames()
{
    QSet<QString> names;
    QStringList curNames;
    for(int i=0;i<OptimAlgosList::nbAlgos;i++)
    {
        OptimAlgo* newAlgo;
        newAlgo = getNewAlgo(NULL,NULL,(OptimAlgosList::Type)i);

        curNames = newAlgo->parameters()->getItemNames();
        if(i==0)
        {
            QSet<QString> set(curNames.begin(), curNames.end());
            names = set;
        }
        else
        {
            QSet<QString> set(curNames.begin(), curNames.end());
            names = names.intersect(set);
        }
        delete newAlgo;
    }
    return names.values();
}

QMap<QString,QStringList> OptimAlgosList::getAlgosParametersNames()
{
    QMap<QString,QStringList> result;
    QStringList curNames;
    for(int i=0;i<OptimAlgosList::nbAlgos;i++)
    {
        OptimAlgo* newAlgo;
        newAlgo = getNewAlgo(NULL,NULL,(OptimAlgosList::Type)i);

        curNames = newAlgo->parameters()->getItemNames();

        result.insert(newAlgo->name(),curNames);
        delete newAlgo;
    }
    return result;
}

QStringList OptimAlgosList::getAlgoNames()
{
    QStringList names;
    for(int i=0;i<OptimAlgosList::nbAlgos;i++)
    {
        OptimAlgo* newAlgo;
        newAlgo = getNewAlgo(NULL,NULL,(OptimAlgosList::Type)i);

        names += newAlgo->name();
        delete newAlgo;
    }
    return names;
}
