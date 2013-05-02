#ifndef PSOPARAMETERS_H
#define PSOPARAMETERS_H
// $Id: NSGA2Parameters.h 10617 2011-11-30 16:17:55Z hubert.thieriot $
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

#include "MOParameter.h"


class PSOParameters
{
public :
    enum parameters
    {
  POPULATIONSIZE,
  MAXITERATIONS,
  INERTIA,
  LEARNINGFACTORALPHA,
  LEARNINGFACTORBETTA,
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
  case INERTIA :return "Inertia";
  case LEARNINGFACTORALPHA :return "LearningFactorAlpha";
  case LEARNINGFACTORBETTA :return "LearningFactorBetta";
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
  parameters->addItem(new MOParameter(str(INERTIA),"Inertia Factor",1,MOParameter::DOUBLE,0,1));
  parameters->addItem(new MOParameter(str(LEARNINGFACTORALPHA),"Learning Factor Alpha",2,MOParameter::DOUBLE,0,2));
  parameters->addItem(new MOParameter(str(LEARNINGFACTORBETTA),"Learning Factor Betta",2,MOParameter::DOUBLE,0,2));
  parameters->addItem(new MOParameter(str(SAVEFREQUENCY),"Population saving frequency (# generations, 0 = save only final state)",1,MOParameter::INT,0,10000000));
  parameters->addItem(new MOParameter(str(USESTARTFILE),"Use start file (restart from previous result)",false,MOParameter::BOOL));
  parameters->addItem(new MOParameter(str(STARTFILEPATH),"Start file path (only if Use start file is checked)",QString(),MOParameter::FILEPATH));
    }
    };





#endif // PSOPARAMETERS_H
