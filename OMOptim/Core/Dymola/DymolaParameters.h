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
 * @file DymolaParameters.h
 * @brief Comments for file documentation.
 * @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 * Company : CEP - ARMINES (France)
 * http://www-cep.ensmp.fr/english/
 * @version
 */


// $Id: DymolaParameters.h 11160 2012-02-17 18:28:06Z hubert.thieriot $


#ifndef _DymolaPARAMETERS_H
#define _DymolaPARAMETERS_H

#include "MOParameter.h"

class DymolaParameters
{
public :
    // Parameters
    enum parameters{STOPTIME,TOLERANCE,NINTERVAL,SOLVER,MAXSIMTIME,FINALFILE};

    enum OutputReadMode
    {
        DSFINAL,
        DSRES
    };
    enum DymolaSolvers{DASSL=8,EULER=11};

    static QString str(parameters index)
    {
        switch(index)
        {
        case STOPTIME: return "stop time";
        case TOLERANCE: return "tolerance";
        case NINTERVAL: return "nInterval";
        case SOLVER: return "Solver";
        case MAXSIMTIME: return "MaxSimTime";
        case FINALFILE: return "Final file considered";
        }
    }


    static void setDefaultParameters(MOParameters *parameters)
    {
        parameters->addItem(new MOParameter(str(STOPTIME),"Stop time",1,MOParameter::DOUBLE,0,std::numeric_limits<int>::max()));
        parameters->addItem(new MOParameter(str(TOLERANCE),"Tolerance",1E-4,MOParameter::DOUBLE,0,std::numeric_limits<int>::max()));
        parameters->addItem(new MOParameter(str(NINTERVAL),"nInterval",500,MOParameter::INT,0,std::numeric_limits<int>::max()));


        QMap<int,QString> mapSolvers;
        mapSolvers.insert(DASSL,"Dassl");
        mapSolvers.insert(EULER,"Euler");

        parameters->addItem( new MOParameterListed(str(SOLVER),"Solver",DASSL,mapSolvers));
        parameters->addItem(new MOParameter(str(MAXSIMTIME),"Maximum time allowed for simulation [sec] (-1 : no limit)",-1,MOParameter::INT,-1,std::numeric_limits<int>::max()));

        QMap<int,QString> finalFiles;
        finalFiles.insert(DSFINAL,"dsfinal");
        finalFiles.insert(DSRES,"dsres");
        parameters->addItem( new MOParameterListed(str(FINALFILE),"File where variables are read (longer if dsres but all variables are within)",DSFINAL,finalFiles));

    }

};

#endif
