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

    @file OneSimulation.h
    @brief Comments for file documentation.
    @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
    Company : CEP - ARMINES (France)
    http://www-cep.ensmp.fr/english/
    @version

  */
#if !defined(_ONESIMULATION_H)
#define _ONESIMULATION_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QDomElement>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#endif

#include "Problem.h"
#include "ModPlusCtrl.h"
#include "ModelPlus.h"

class Project;
class ModPlusCtrls;
class ModModelPlus;
class Variables;
class ScannedVariables;

/**
  * Basic problem which consists in simulating a model with modified input variables.
  * Model is simulated using a controler (ModPlusCtrl).
  * The result of a OneSimulation problem is a OneSimResult which contains
  * final variables.
  * OneSimulation inherits the abstract class Problem.
  */
class OneSimulation : public Problem
{

public:
    //OneSimulation(void);
    OneSimulation(Project*,ModelPlus* ModPlus);
    OneSimulation(const OneSimulation &s);
    OneSimulation(QDomElement domProblem,Project* project,bool &ok);

    Problem* clone() const;
    virtual ~OneSimulation(void);

    static QString className(){return "OneSimulation";};
    virtual QString getClassName() const {return OneSimulation::className();};


    ModPlusCtrl* ctrl();
    ModPlusCtrls* ctrls();
    ModPlusCtrl::Type ctrlType();
    void setCtrlType(ModPlusCtrl::Type);
    void setCtrls(const ModPlusCtrls &);

    //overwrited functions
    bool checkBeforeComp(QString & error);
    Result* launch(ProblemConfig _config);
    void store(QString destFolder, QString tempDir);
    QDomElement toXmlData(QDomDocument & doc);


    bool canBeStoped();
    void stop();



    // get functions
    Variables *overwritedVariables() const {return _overwritedVariables;}
    ScannedVariables *scannedVariables() const {return _scannedVariables;}
    ModelPlus *modelPlus() const;
    QString model() const;

//public slots :
    //    void setCtrlType();

    bool setOverwritedVariableValue(QString modelName, QString varName, double value);
protected :

    Project* _omProject; /// same adress than _project but casted to Project* instead of ProjectBase*

    ModelPlus* _ModelPlus;

    Variables *_overwritedVariables;
    ScannedVariables *_scannedVariables;

    // Simulation controlers
    ModPlusCtrls* _ctrls;


};


#endif
