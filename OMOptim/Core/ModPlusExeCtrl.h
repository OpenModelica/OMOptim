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

#ifndef MODPLUSEXECTRL_H
#define MODPLUSEXECTRL_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QDir>
#endif

#include "ModPlusCtrl.h"
#include "ModExePlus.h"

class PlugInterface;

class ModPlusExeCtrl : public ModPlusCtrl
{
public:

    enum Output{CSV,MAT};
    enum Parameters{STOPTIME,MAXSIMTIME,SOLVER,TOLERANCE,STEPSIZE,STARTTIME,OUTPUT};
    enum Solvers{DASSL,EULER};


    ModPlusExeCtrl(Project* project,ModelPlus* model);
    virtual ~ModPlusExeCtrl(void);

    virtual ModPlusCtrl::Type type() const = 0;
    QString name();


    // Variables functions
    virtual bool readInitialVariables(MOVector<Variable> *, QFileInfoList filesToCopy,bool forcerecompile = false, QString initFile="") =0;
    void setInputVariablesXml(QDomDocument & doc, QString modelName, MOVector<Variable> *variables);
//    void setInputParametersXml(QDomDocument &doc,MOParameters *parameters);

    // compatible models
    virtual QList<ModelPlus::ModelType> compatibleModels();

    // Parameters
    void setDefaultParameters();

    // Compile function
    bool isCompiled(){return true;}
    bool compile(const QFileInfoList & moDeps = QFileInfoList(), QFileInfoList filesToCopy = QFileInfoList()){return true;}

    bool createInitFile(const QStringList & moDeps = QStringList());

    // Simulate function
    bool getInputVariablesFromXmlFile(QString filePath, QString modModelName, MOVector<Variable> * variables);
    bool getInputVariablesFromXmlFile(const QDomDocument & doc , QString modModelName,MOVector<Variable> * variables);
    Variable* variableFromFmi(const QDomElement & el,QString modModelName,  bool & ok);
    virtual bool simulate(QDir tempDir, MOVector<Variable> * inputVars, MOVector<Variable> * outputVars, QFileInfoList filesToCopy ,QFileInfoList moDependencies) = 0;
    virtual bool start(QString tempExeFile, int maxNSec);

    //read output resfiles
    bool getFinalVariablesFromFile(QString fileName_, MOVector<Variable> *variables,QString _modelName);
    bool getFinalVariablesFromFile(QTextStream *text, MOVector<Variable> * variables,QString _modelName);
    virtual bool readOutputVariables(MOVector<Variable> *,QString outputfile=""){return true;}
//    void readResultsFromXmlDoc(const QDomDocument & doc, MOVector<Variable> * variables, QString modelName);
    QFileInfo initFile() const;
    QFileInfo exeFile() const;
protected :


};


#endif // MODPLUSEXECTRL_H
