// $Id: NSGA2Parameters.h 9677 2011-08-24 13:09:00Z hubert.thieriot $
/**
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linkpings universitet, Department of Computer and Information Science,
 * SE-58183 Linkping, Sweden.
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
        @version

  */

#ifndef _SA1PARAMETERS_H
#define _SA1PARAMETERS_H

#include "MOParameter.h"


class SA1Parameters
{
public :
    enum parameters
    {
        MAXITERATIONS,
        INITTEMPERATURE,
        FINALTEMPERATURE,
        DECREASINGFACTOR,
        SPAN,
        SAVEFREQUENCY,
        USESTARTFILE,
        STARTFILEPATH
    };

    static void setDefaultParameters(MOParameters *parameters)
    {
        parameters->addItem(new MOParameter(MAXITERATIONS,"MaxIterations","Max Iterations",10000,MOParameter::INT,1,1000000000));
        parameters->addItem(new MOParameter(INITTEMPERATURE,"InitialTemperature","Initial Temperature",1,MOParameter::DOUBLE,0.001,10000));
        parameters->addItem(new MOParameter(FINALTEMPERATURE,"FinalTemperature","Final Temperature",0.01,MOParameter::DOUBLE,0.00001,100));
        parameters->addItem(new MOParameter(DECREASINGFACTOR,"DecreasingFactor","Decreasing Factor",0.9,MOParameter::DOUBLE,0.1,0.99));
        parameters->addItem(new MOParameter(SPAN,"Span","Number of iterations with the same temperature",100,MOParameter::INT,1,10000));
        parameters->addItem(new MOParameter(SAVEFREQUENCY,"SaveFrequency","Population saving frequency (# generations, 0 = save only final state)",1,MOParameter::INT,0,10000000));
        parameters->addItem(new MOParameter(USESTARTFILE,"UseStartFile","Use start file (restart from previous result)",false,MOParameter::BOOL));
        parameters->addItem(new MOParameter(STARTFILEPATH,"StartFilePath","Start file path (only if Use start file is checked)",QString(),MOParameter::FILEPATH));

        // make parameters enabled only when needed
        QList<int> grIndexes;
        grIndexes << USESTARTFILE << STARTFILEPATH;
        parameters->setGroup("Pursue computation",grIndexes);
        grIndexes.removeAll(USESTARTFILE);
        parameters->addEnablingIndex(grIndexes,USESTARTFILE,true);
    };
};

#endif
