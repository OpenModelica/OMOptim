// $Id: OpenModelicaParameters.h 11160 2012-02-17 18:28:06Z hubert.thieriot $
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

  @file OpenModelicaParameters.h
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
        @version

  */

#ifndef _OpenModelicaPARAMETERS_H
#define _OpenModelicaPARAMETERS_H

#include "MOParameter.h"

class OpenModelicaParameters
{
public :
    // Parameters
    enum Parameters{STOPTIME,MAXSIMTIME,SOLVER,TOLERANCE,NBINTERVALS,STARTTIME,OUTPUT};
    enum Solvers{DASSL,EULER};
    enum Output{CSV,MAT};


    static QString str(Parameters index)
    {
        switch(index)
        {
        case STOPTIME: return "stopTime";
        case MAXSIMTIME: return "MaxSimTime";
        case SOLVER: return "solver";
        case TOLERANCE: return "tolerance";
        case NBINTERVALS: return "NbIntervals";
        case STARTTIME: return "startTime";
        case OUTPUT: return "outputFormat";
        }
    }


    static void setDefaultParameters(MOParameters *parameters)
    {
        parameters->addItem(new MOParameter(str(STOPTIME),"Stop time",1,MOParameter::DOUBLE,0,std::numeric_limits<int>::max()));

        QMap<int,QString> mapSolvers;
        mapSolvers.insert(DASSL,"dassl");
        mapSolvers.insert(EULER,"euler");

        parameters->addItem( new MOParameterListed(str(SOLVER),"Solver",DASSL,mapSolvers));

        parameters->addItem(new MOParameter(str(TOLERANCE),"Tolerance",1e-6,MOParameter::DOUBLE,0,std::numeric_limits<int>::max()));
        parameters->addItem(new MOParameter(str(NBINTERVALS),"Number of output intervals",2,MOParameter::INT,2,std::numeric_limits<int>::max()));
        parameters->addItem(new MOParameter(str(STARTTIME),"Start time",0,MOParameter::DOUBLE,0,std::numeric_limits<int>::max()));

        parameters->addItem(new MOParameter(str(MAXSIMTIME),"Maximum time allowed for simulation [sec] (-1 : no limit)",-1,MOParameter::INT,-1,std::numeric_limits<int>::max()));

        QMap<int,QString> mapOutput;
        mapOutput.insert(MAT,"mat");
        mapOutput.insert(CSV,"csv");

        parameters->addItem( new MOParameterListed(str(OUTPUT),"Output",MAT,mapOutput));
    }

};

#endif
