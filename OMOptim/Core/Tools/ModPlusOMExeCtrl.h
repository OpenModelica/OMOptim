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

#ifndef MODPLUSOMEXECTRL_H
#define MODPLUSOMEXECTRL_H

#include "ModPlusExeCtrl.h"

class Project;
class ModelPlus;
class ModPlusCtrl;
class QString;
class QDir;



class ModPlusOMExeCtrl: public ModPlusExeCtrl
{
public:
    ModPlusOMExeCtrl(Project* project,ModelPlus* model);
    ~ModPlusOMExeCtrl(void);

    ModPlusCtrl *clone();


    QString name();
    bool useMat();
    QString resFile();
    QString resMatFile();
    QString resCsvFile();
    ModPlusCtrl::Type type() const;

    virtual bool readInitialVariables(MOVector<Variable> *, QFileInfoList filesToCopy,bool forcerecompile, QString initFile="");

    bool simulate(QDir tempFolder, MOVector<Variable> *inputVars, MOVector<Variable> *outputVars, QFileInfoList filesToCopy, QFileInfoList moDependencies);
    bool start(QString exeFile,int maxnsec);
    bool readOutputVariables(MOVector<Variable> *,QString resFile="");

    bool setStopTime(double time);
};

#endif // MODPLUSOMEXECTRL_H
