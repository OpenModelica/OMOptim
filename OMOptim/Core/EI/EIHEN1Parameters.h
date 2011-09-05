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

  @file HEN1Parameters.h
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
        @version

  */

#ifndef _EIHEN1PARAMETERS_H
#define _EIHEN1PARAMETERS_H

#include "MOParameter.h"
#include "CBCTools.h"

class EIHEN1Parameters
{
public :
    enum Solver
    {
        GLPK,
        CBC
    };

    enum CbcPreprocessor
    {
        OFF,
        ON,
        SAVE,
        EQUAL,
        SOS,
        TRYSOS,
        EQUALALL,
        STRATEGY,
        AGGREGATE,
        FORCESOS
    };

    enum parameters
    {
        SPLITPINCH,//if true, split streams in two zones / pinch
        ALLOWNI,// if true, non-isothermal connections are allowed for every streams
        ALLOWSPLITS,// if true, splits are allowed for all streams
        ALLOWSEVERAL, // if true, more than one heat exchanger is permitted between each couple of streams
        KMAX, //MAximum number of heat exchanger between two streams
        SPLITDT, // should we split temperature intervals (better if yes but computation time increased)
        SPLITDTSTEP, // temperature interval spliting step
        SOLVER,
        LOGSENS, //print sensitivity report (only for glpk)
        CBCPREPROCESS,
        PRIMALTOLERANCE,
        INTEGERTOLERANCE
    };

    /**
      * Provides parameters' predefinitions for HEN solving. Correponsing to different
      * trade-offs speedness - precision.
      */
    enum predefinitions
    {
        SPEEDNESS,
        AVERAGE,
        PRECISION
    };

    static void setDefaultParameters(MOParameters *parameters)
    {
        parameters->addItem(new MOParameter((int)SPLITPINCH,"Split streams in two zones (below,above pinch)","PinchSplit",false,MOParameter::BOOL));
        parameters->addItem(new MOParameter((int)ALLOWNI,"Allow non-isothermal","NI",false,MOParameter::BOOL));
        parameters->addItem(new MOParameter((int)ALLOWSPLITS,"Allow splits","S",true,MOParameter::BOOL));
        parameters->addItem(new MOParameter((int)ALLOWSEVERAL,"Allow several heat exchangers between two streams","B",false,MOParameter::BOOL));
        parameters->addItem(new MOParameter((int)KMAX,"Maximum heat exhangers between two streams","S",5,MOParameter::INT,1,std::numeric_limits<int>::max(),(int)ALLOWSEVERAL));
        parameters->addItem(new MOParameter((int)SPLITDT,"Should we split temperature intervals (better if yes but computation time increased) ?","",false,MOParameter::BOOL));
        parameters->addItem(new MOParameter((int)SPLITDTSTEP,"Maximum temperature interval (if an interval is higher, it will be splitted)","",10,MOParameter::DOUBLE,1E-10,std::numeric_limits<int>::max(),(int)SPLITDT));



        //solver choice
        QMap<int,QString> solverList;
        solverList.insert(GLPK,"Glpk");
        if(CBCTools::isInstalled())
            solverList.insert(CBC,"Cbc");
        parameters->addItem(new MOParameterListed((int)SOLVER,"Solver","",GLPK,solverList));

        parameters->addItem(new MOParameter((int)LOGSENS,"Print sensitivity report (GLPK)","LOGSENS",true,MOParameter::BOOL,0,1,(int)SOLVER,(int)GLPK));

        if(CBCTools::isInstalled())
        {
            //CBC preprocessor
            QMap<int,QString> cbcPreprocessList;
            cbcPreprocessList.insert(OFF,"off");
            cbcPreprocessList.insert(ON,"on");
            cbcPreprocessList.insert(SAVE,"save");
            cbcPreprocessList.insert(EQUAL,"equal");
            cbcPreprocessList.insert(SOS,"sos");
            cbcPreprocessList.insert(TRYSOS,"trysos");
            cbcPreprocessList.insert(EQUALALL,"equalall");
            cbcPreprocessList.insert(STRATEGY,"strategy");
            cbcPreprocessList.insert(AGGREGATE,"aggregate");
            cbcPreprocessList.insert(FORCESOS,"forcesos");
            parameters->addItem(new MOParameterListed((int)CBCPREPROCESS,"Cbc preprocessor","",SOS,cbcPreprocessList,(int)SOLVER,(int)CBC));



            // tolerances
            parameters->addItem(new MOParameter((int)PRIMALTOLERANCE,"Cbc : Primal tolerance ","",1E-7,MOParameter::DOUBLE,
                                                0,std::numeric_limits<int>::max(),(int)SOLVER,(int)CBC));
            parameters->addItem(new MOParameter((int)INTEGERTOLERANCE,"Cbc : Integer tolerance","",1E-6,MOParameter::DOUBLE,
                                                0,std::numeric_limits<int>::max(),(int)SOLVER,(int)CBC));

        }
    };


    static void updateParameters(MOParameters *parameters, predefinitions predef)
    {
        switch(predef)
        {
        case SPEEDNESS :
            parameters->setValue((int)SPLITPINCH,true);
            parameters->setValue((int)ALLOWNI,false);
            parameters->setValue((int)ALLOWSPLITS,true);
            parameters->setValue((int)ALLOWSEVERAL,false);
            parameters->setValue((int)SPLITDT,false);
            break;
        case AVERAGE :
            parameters->setValue((int)SPLITPINCH,false);
            parameters->setValue((int)ALLOWNI,false);
            parameters->setValue((int)ALLOWSPLITS,true);
            parameters->setValue((int)ALLOWSEVERAL,true);
            parameters->setValue((int)KMAX,3);
            parameters->setValue((int)SPLITDT,false);
            break;
        case PRECISION :
            parameters->setValue((int)SPLITPINCH,false);
            parameters->setValue((int)ALLOWNI,false);
            parameters->setValue((int)ALLOWSPLITS,true);
            parameters->setValue((int)ALLOWSEVERAL,true);
            parameters->setValue((int)KMAX,3);
            parameters->setValue((int)SPLITDT,true);
            parameters->setValue((int)SPLITDTSTEP,5);
            break;
        }
    }
};

#endif
