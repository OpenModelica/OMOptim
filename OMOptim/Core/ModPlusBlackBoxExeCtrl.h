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

#ifndef MODPLUSBLACKBOXEXECTRL_H
#define MODPLUSBLACKBOXEXECTRL_H

#include "ModPlusCtrl.h"
#include "ModPlusExeCtrl.h"


class Project;
class ModelPlus;
class PlugInterface;


class ModPlusBlackBoxExeCtrl : public ModPlusExeCtrl
{
public:
    ModPlusBlackBoxExeCtrl(Project* project,ModelPlus* model);
    ~ModPlusBlackBoxExeCtrl(void);

    virtual ModPlusCtrl::Type type() const;
    ModPlusCtrl *clone();
    QString name();

    //Load Plugin
    PlugInterface* loadPlugInterface() const;

    //Variables Functions
    virtual bool readInitialVariables(MOVector<Variable> *,QFileInfoList filesToCopy, bool forcerecompile, QString initFile="");
    void setInputVarsInInitFileUsingPluginFunc(QString tempInitFile, QString modelName, MOVector<Variable> *variables);
    bool simulate(QDir tempDir, MOVector<Variable> * inputVars, MOVector<Variable> * outputVars, QFileInfoList filesToCopy ,QFileInfoList moDependencies);
    bool readInitialVarsFromPluginFunc(QString filename, QString modelName,MOVector<Variable> *initVariables);
    bool setStopTime(double time);

    //read output resfiles
    bool getOutputVarsUsingPluginFunc(QString fileName, MOVector<Variable> * variables, QString modelName);
    void readResultsFromXmlDoc(const QDomDocument &doc, MOVector<Variable> * variables, QString modelName);
    QString resFileNameFromPlugFunc() const;

private:
    PlugInterface* _plugInter;

};

#endif // MODPLUSBLACKBOXEXECTRL_H
