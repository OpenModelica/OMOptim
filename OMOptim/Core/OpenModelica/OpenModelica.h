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

/*
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)
 * @file OpenModelica.h
 * @brief Comments for file documentation.
 * @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 * Company : CEP - ARMINES (France)
 * http://www-cep.ensmp.fr/english/
 * @version
 */


// $Id$

#ifndef _OpenModelica_H
#define _OpenModelica_H

/*!
 * \file OpenModelica.h
 * \brief File containing OpenModelica class description.
 * \author Hubert Thieriot (CEP-Armines)
 * \version 0.1
 */

#include <iostream>
#include <vector>
#include "Variable.h"
#include "MOVector.h"
#include "MOomc.h"
#include "vqtconvert.h"

#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <iostream>
#include "MOParameter.h"
#include <QtCore/QProcess>
#include "VariableType.h"

class OpenModelica
{
    /*! \class OpenModelica
   * \brief Class containing handling functions for OpenModelica.
   *
   *  All functions defined in this class are static. There are used to :
   *    -read/write input or output files
   *    -compile model
   *
   */

public:
    OpenModelica();
    ~OpenModelica(void);

    static bool compile(MOomc *_omc,QFileInfo moFile,QString modelToConsider,QDir storeFolder,
                        const QFileInfoList & moDeps, QFileInfoList neededFiles, QFileInfoList neededFolders);
    static void getInputVariablesFromTxtFile(MOomc *_omc,QString filePath, MOVector<Variable> *,QString _modelName);
    static void getInputVariablesFromTxtFile(MOomc *_omc,QTextStream *, MOVector<Variable> *,QString _modelName);
    static bool getInputVariablesFromXmlFile(MOomc *_omc,QString,QString modModelName, MOVector<Variable> *);
    static bool getInputVariablesFromXmlFile(MOomc *_omc,const QDomDocument &,QString modModelName, MOVector<Variable> *);

    static bool getFinalVariablesFromFile(QString, MOVector<Variable> *,QString _modelName);
    static bool getFinalVariablesFromFile(QTextStream *, MOVector<Variable> *,QString _modelName);

    static bool getFinalVariablesFromMatFile(QString, MOVector<Variable> *,QString _modelName);

    static VariableCausality varCausality(const QDomElement & el);


    static void setInputVariablesTxt(QString, MOVector<Variable> *,QString modModelName,MOParameters *parameters=NULL);
    static bool setInputXml(QString, MOVector<Variable> *, QString modelName,MOParameters *parameters=NULL);
    static bool start(QString exeFile,QString &errMsg, int maxnsec);
    static QString sciNumRx();
    static QString home();


    static QString getLibraryPath(QString version =QString());

private :
    static Variable* variableFromFmi(const QDomElement & ,QString modModelName, bool & ok);
    static void setInputVariablesXml(QDomDocument &, QString modModelName,MOVector<Variable> *);
    static void setInputParametersXml(QDomDocument &, MOParameters *);

    // Parameters


};

#endif
