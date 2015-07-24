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

    @file OneSimResult.h
    @brief Comments for file documentation.
    @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
    Company : CEP - ARMINES (France)
    http://www-cep.ensmp.fr/english/
    @version

  */
#if !defined(_ONESIMRESULTS_H)
#define _ONESIMRESULTS_H
#include "Result.h"
#include "MOVector.h"
#include "MOOptVector.h"
#include "Variables.h"

#include <stdio.h>

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtCore/QObject>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>
#include <QtXml/QDomDocument>
#endif

#include "CSV.h"
#include "ModPlusCtrl.h"


class OneSimulation;

class OneSimResult : public Result
{
    Q_OBJECT

public:
    OneSimResult(Project* project);
    OneSimResult(Project* project, ModelPlus* modModelPlus, const OneSimulation & problem);
    OneSimResult(Project* project, const QDomElement & domResult,const OneSimulation &problem, bool &ok);
    virtual ~OneSimResult(void);

    static QString className(){return "OneSimResult";};
    virtual QString getClassName() const {return OneSimResult::className();};

    MOVector<Variable> *inputVariables(){return _inputVariables;};
    MOOptVector *finalVariables(){return _finalVariables;};
    ModPlusCtrl* modPlusCtrl();

    //******************
    // Methods
    //******************
    QDomElement toXmlData(QDomDocument &);

protected :
    //******************
    // Attributes
    //******************
    Project* _omProject; /// same adress than _project but casted to Project* instead of ProjectBase*
    Variables *_inputVariables;
    MOOptVector *_finalVariables;

    //Model
    ModelPlus* _ModelPlus;


signals:
    void valuesChanged();
};


#endif
