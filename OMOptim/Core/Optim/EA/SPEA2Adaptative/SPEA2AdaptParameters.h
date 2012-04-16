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

        @file SPEA2AdaptParameters.h
        @brief Comments for file documentation.
        @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
        Company : CEP - ARMINES (France)
        http://www-cep.ensmp.fr/english/
        @version 0.9

  */

#ifndef _SPEA2ADAPTPARAMETERS_H
#define _SPEA2ADAPTPARAMETERS_H

#include "MOParameter.h"


class SPEA2AdaptParameters
{
public :
    enum parameters
    {
        POPULATIONSIZE,
        OFFSPRINGRATE,
        MAXGENERATIONS,
        SAVEFREQUENCY,
        USESTARTFILE,
        STARTFILEPATH,
        REINITSTDDEV
    };

    static void setDefaultParameters(MOParameters *parameters)
    {

        parameters->addItem(new MOParameter(POPULATIONSIZE,"PopulationSize","Population size",50,MOParameter::INT,1,1000));
        parameters->addItem(new MOParameter(OFFSPRINGRATE,"OffspringRate","OffSpring size/PopulationSize",3,MOParameter::INT,1,1000));
        parameters->addItem(new MOParameter(MAXGENERATIONS,"MaxGen","Max Generations",100,MOParameter::INT,0,100000));
        parameters->addItem(new MOParameter(SAVEFREQUENCY,"SaveFrequency","Population saving frequency (# generations, 0 = save only final state)",1,MOParameter::INT,0,10000000));

        // pursue computation
        parameters->addItem(new MOParameter(USESTARTFILE,"UseStartFile","Use start file (restart from previous result)",false,MOParameter::BOOL));
        parameters->addItem(new MOParameter(STARTFILEPATH,"StartFilePath","Start file path (only if Use start file is checked)",QString(),MOParameter::FILEPATH));
        parameters->addItem(new MOParameter(REINITSTDDEV,"ReinitStdDev","Reinitialize StdDeviation (for pursuing optimization only)",0,MOParameter::BOOL));

        // make parameters enabled only when needed
        QList<int> grIndexes;
        grIndexes << USESTARTFILE << STARTFILEPATH << REINITSTDDEV;
        parameters->setGroup("Pursue computation",grIndexes);
        grIndexes.removeAll(USESTARTFILE);
        parameters->addEnablingIndex(grIndexes,USESTARTFILE,true);
    };
};

#endif
