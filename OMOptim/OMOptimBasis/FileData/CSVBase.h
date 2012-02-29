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

    @file CSVBase.h
    @brief Comments for file documentation.
    @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
    Company : CEP - ARMINES (France)
    http://www-cep.ensmp.fr/english/
    @version

  */
#ifndef CSVBASE_H
#define CSVBASE_H

#include <stdio.h>
#include <QtCore/QObject>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>
#include <QtXml/QDomDocument>



#include "MOVector.h"

class OptObjective;
class OptObjectiveResult;
class Variable;
class MOOptVector;
class VariableResult;
class ScannedVariable;
class ModModelPlus;


class CSVBase
{
public:
    static QString space();

    // var to text
    static QString variablesToLines(MOVector<Variable> *);
    static QString oneVariableResultToValueLines(VariableResult *);
    static QString variableResultToValueLines(MOOptVector *, int iPoint);
    static QString scannedVariablesToLines(MOVector<ScannedVariable> *);
    static QString optObjectivesToLines(MOVector<OptObjective> *);

    //text to var
    static void linesToVariableResult(MOOptVector *, int iPoint, QString lines, bool addIfNotFound);
    static void LinesToScannedVariables(MOVector<ScannedVariable>*, QString);
    static void LinesToOptObjectives(MOVector<OptObjective>*, QString);
    static void LinesToOptObjectives(MOVector<OptObjectiveResult>*, QString);
    static void LinesToVariables(MOVector<Variable>*, QString);
    static void linesToOneVariableResultValues(VariableResult *,QString);

    //Files to var
    static void FileToVariables(MOVector<Variable>*, QString);
    static void FileToVariableResult(MOOptVector *variables, int iPoint,QString fileName, bool addIfNotFound=true);


};


#endif
