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

     @file Save.h
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version

  */
#ifndef SAVEOMOPTIM_H
#define SAVEOMOPTIM_H

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

#include "Result.h"
#include "OneSimResult.h"
#include "OptimResult.h"
#include "Project.h"
#include "Problem.h"
#include "OneSimulation.h"
#include "Optimization.h"
#include "OptimResult.h"
#include "MOVector.h"

class Optimization;

class SaveOMOptim
{
public:
    SaveOMOptim(void);
    ~SaveOMOptim(void);

    static QDomDocument ProjectToXml(Project*);

    static void saveProject(Project*,bool saveAllCases=true);
    static void saveModelPlus(Project *project, ModelPlus *modelPlus);
    static bool setModelsPath(QString projectFilePath,QStringList modelsPaths);

  //  static void saveExternalProject(Project *project, QDir exportFolder);
};


#endif
