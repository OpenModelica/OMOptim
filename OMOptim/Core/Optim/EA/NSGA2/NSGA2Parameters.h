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
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)

  @file NSGA2Parameters.h
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version 0.9

  */

#ifndef _NSGA2PARAMETERS_H
#define _NSGA2PARAMETERS_H

#include "MOParameter.h"


class NSGA2Parameters
{
public :
    enum parameters
    {
  POPULATIONSIZE,
  MAXITERATIONS,
  DOUBLEMUTEPSILON,
  DOUBLEPMUT,
  DOUBLEPCROSS,
  INTPMUT,
  INTPCROSS,
  BOOLPMUT,
  BOOLPCROSS,
  SAVEFREQUENCY,
  USESTARTFILE,
  STARTFILEPATH
    };

    static QString str(parameters index)
    {
  switch(index)
  {
  case POPULATIONSIZE :return "PopulationSize";
  case MAXITERATIONS :return "MaxIterations";
  case DOUBLEMUTEPSILON :return "DoubleMutEpsilon";
  case DOUBLEPMUT :return "DoublePMut";
  case DOUBLEPCROSS :return "DoublePCross";
  case INTPMUT :return "IntPMut";
  case INTPCROSS :return "IntPCross";
  case BOOLPMUT :return "BoolPMut";
  case BOOLPCROSS :return "BoolPCross";
  case SAVEFREQUENCY :return "SaveFrequency";
  case USESTARTFILE :return "UseStartFile";
  case STARTFILEPATH :return "StartFilePath";
  }
  return QString();
    }

    static void setDefaultParameters(MOParameters *parameters)
    {
  parameters->addItem(new MOParameter(str(POPULATIONSIZE),"Population size",50,MOParameter::INT,1,1000));
  parameters->addItem(new MOParameter(str(MAXITERATIONS),"Max Iterations",10000,MOParameter::INT,1,1000000000));
  parameters->addItem(new MOParameter(str(DOUBLEMUTEPSILON),"Epsilon for real variables mutation",0.01,MOParameter::DOUBLE,0,1));
  parameters->addItem(new MOParameter(str(DOUBLEPMUT),"Mutation probability for real variables",0.35,MOParameter::DOUBLE,0,1));
  parameters->addItem(new MOParameter(str(DOUBLEPCROSS),"Crossover probability for real variables",0.25,MOParameter::DOUBLE,0,1));
  parameters->addItem(new MOParameter(str(INTPMUT),"Mutation probability for integer variables",0.35,MOParameter::DOUBLE,0,1));
  parameters->addItem(new MOParameter(str(INTPCROSS),"Crossover probability for integer variables",0.25,MOParameter::DOUBLE,0,1));
  parameters->addItem(new MOParameter(str(BOOLPMUT),"Mutation probability for boolean variables",0.35,MOParameter::DOUBLE,0,1));
  parameters->addItem(new MOParameter(str(BOOLPCROSS),"Crossover probability for boolean variables",0.25,MOParameter::DOUBLE,0,1));
  parameters->addItem(new MOParameter(str(SAVEFREQUENCY),"Population saving frequency (# generations, 0 = save only final state)",1,MOParameter::INT,0,10000000));
  parameters->addItem(new MOParameter(str(USESTARTFILE),"Use start file (restart from previous result)",false,MOParameter::BOOL));
  parameters->addItem(new MOParameter(str(STARTFILEPATH),"Start file path (only if Use start file is checked)",QString(),MOParameter::FILEPATH));

  // make parameters enabled only when needed
  QStringList grIndexes;
  grIndexes << str(USESTARTFILE) << str(STARTFILEPATH);
  parameters->setGroup("Pursue computation",grIndexes);
  grIndexes.removeAll(str(USESTARTFILE));
  parameters->addEnablingIndex(grIndexes,str(USESTARTFILE),true);
    }
};

#endif
