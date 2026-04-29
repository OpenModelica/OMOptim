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

#ifndef ONESIMULATIONINTERFACE_H
#define ONESIMULATIONINTERFACE_H

#include "ProblemInterface.h"
#include "OneSimulation.h"
#include "OneSimResult.h"


class OneSimulationInterface : public ProblemInterface
{

public :

    virtual Problem* createNewProblem(ProjectBase*,const QStringList ,QString problemType);

    QWidget* createProblemTab(Problem *,QWidget* parent);
    QWidget* createResultTab(Result *,QWidget* parent);


    virtual ModelNeeds modelNeeds(QString){return ONEMODEL;}
    virtual QStringList problemTypes(){return QStringList(OneSimulation::className());}
    virtual QStringList resultTypes(){return QStringList(OneSimResult::className());}
    QString name(){return OneSimulation::className();}
    virtual Problem* loadProblem(QFileInfo saveFile,const QDomElement & domOMCase,ProjectBase *);
    virtual Result* loadResult(QFileInfo saveFile,const QDomElement & domOMCase,ProjectBase*);
};

#endif
