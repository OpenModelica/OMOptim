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

#ifndef MODPLUSDYMOLAEXECTRL_H
#define MODPLUSDYMOLAEXECTRL_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QProcess>
#endif

#include "ModPlusExeCtrl.h"


class ModPlusDymolaExeCtrl: public ModPlusExeCtrl
{
public:
    ModPlusDymolaExeCtrl(Project* project,ModelPlus* model);
    ~ModPlusDymolaExeCtrl();

    ModPlusCtrl* clone();

    QString name();
    virtual ModPlusDymolaExeCtrl::Type type() const;

    void setDefaultParameters();
    bool readOutputVariables(MOVector<Variable> *finalVariables,QString path);
    bool readOutputVariablesDSFINAL(MOVector<Variable> *finalVariables, QString dsfinalFile);
    bool readOutputVariablesDSRES(MOVector<Variable> *finalVariables, QString dsresFile);
    bool readInitialVariables(MOVector<Variable> *initVariables, QFileInfoList filesToCopy,bool forceRecompile, QString dsinFile);
    bool simulate(QDir tempDir,MOVector<Variable> * updatedVars,MOVector<Variable> * outputVars,QFileInfoList filesTocopy,QFileInfoList moDependencies);

    bool setStopTime(double time);

protected :
    QProcess _simProcess;
    QString _dsfinalFile;
    QString _dsresFile;


};

#endif // MODPLUSDYMOLAEXECTRL_H
