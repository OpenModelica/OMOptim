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

#ifndef VARIABLES_H
#define VARIABLES_H

#include "MOVector.h"
#include "Variable.h"


/**
  * Variables is a container for several variables. It contains a
  * modelname concerned in order to improve display. Except this function,
  * it acts as MOVector<Variable>
  */
class Variables : public MOVector<Variable>
{
public:
    Variables(bool owner,QObject* parent);
    ~Variables();

    QVariant data(const QModelIndex &index, int role) const;
    Variables* clone() const;

 //   QStringList mimeTypes() const;
 //   QMimeData* mimeData(const QModelIndexList &indexes) const;
    Qt::DropActions supportedDropActions() const;

    Variable* findVariable(QString model,QString shortVarName);
    int findVariable(QString model, QRegularExpression &shortVarName, int from=0);

private :
};

/**
  * OptVariables is a container for several variables. It contains a
  * modelname concerned in order to improve display. Except this function,
  * it acts as MOVector<OptVariable>
  */
class OptVariables : public MOVector<OptVariable>
{
public:
    OptVariables(bool owner,QObject* parent);
    ~OptVariables();
    QVariant data(const QModelIndex &index, int role) const;
    OptVariables* clone() const;

private :
};


/**
  * ScannedVariables is a container for several variables. It contains a
  * modelname concerned in order to improve display. Except this function,
  * it acts as MOVector<ScannedVariable>
  */
class ScannedVariables : public MOVector<ScannedVariable>
{
public:
    ScannedVariables(bool owner,QObject* parent);
    ~ScannedVariables();
    QVariant data(const QModelIndex &index, int role) const;
    ScannedVariables* clone() const;

private :
};




#endif // VARIABLES_H
