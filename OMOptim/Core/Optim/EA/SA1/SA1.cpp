// $Id: SA1.cpp 9677 2011-08-24 13:09:00Z hubert.thieriot $
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
 * Main contributor 2011, Hubert Thierot, CEP - ARMINES (France)

 	@file SA1.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 

  */
/*
* Base on <Sch1.cpp>
* Copyright (C) DOLPHIN Project-Team, INRIA Futurs, 2006-2007
* (C) OPAC Team, LIFL, 2002-2007
*
* Abdelhakim Deneche, Arnaud Liefooghe
*
* This software is governed by the CeCILL license under French law and
* abiding by the rules of distribution of free software.  You can  use,
* modify and/ or redistribute the software under the terms of the CeCILL
* license as circulated by CEA, CNRS and INRIA at the following URL
* "http://www.cecill.info".
*
* As a counterpart to the access to the source code and  rights to copy,
* modify and redistribute granted by the license, users are provided only
* with a limited warranty  and the software's author,  the holder of the
* economic rights,  and the successive licensors  have only  limited liability.
*
* In this respect, the user's attention is drawn to the risks associated
* with loading,  using,  modifying and/or developing or reproducing the
* software by the user in light of its specific status of free software,
* that may mean  that it is complicated to manipulate,  and  that  also
* therefore means  that it is reserved for developers  and  experienced
* professionals having in-depth computer knowledge. Users are therefore
* encouraged to load and test the software's suitability as regards their
* requirements in conditions enabling the security of their systems and/or
* data to be ensured and,  more generally, to use and operate it in the
* same conditions as regards security.
* The fact that you are presently reading this means that you have had
* knowledge of the CeCILL license and that you accept its terms.
*
* ParadisEO WebSite : http://paradiseo.gforge.inria.fr
* Contact: paradiseo-help@lists.gforge.inria.fr
*
*/
//-----------------------------------------------------------------------------

#include "SA1.h"
#include "SA1mo.h"
#include "Chromosome/EOStd.h"
#include "EAStdInitBounded.h"
#include "EAStdMutation.h"
#include "SBCrossover.h"
#include "EAStdCheckPoint.h"
#include "EAStdBounds.h"
#include "OMEAEvalFuncCounter.h"
#include "OMEAProgress.h"
#include "OMEAEvalContinue.h"
#include "MyEoGnuplot1DMonitor.h"
#include "EAStdOptimizationEval.h"
#include "EAStdResult.h"
#include "SA1Parameters.h"

#include <stdexcept>  // runtime_error
#include <iostream>   // cout
#include <sstream>  // ostrstream, istrstream
#include <fstream>
#include <string.h>

// the general include for eo
#include <eo>
#include <ga.h>


#include "continuator/moCheckpoint.h"

using namespace std;

//-----------------------------------------------------------------------------
//Representation and initializer
#include <eoInt.h>
#include <eoInit.h>
#include <eoScalarFitness.h>

// fitness function
#include <eval/moFullEvalByModif.h>
#include <SA1moFullEvalByCopy.h>

//Neighbors and Neighborhoods
#include <SA1moShiftNeighbor.h>
#include <SA1moRndWithReplNeighborhood.h>
//Algorithm and its components
#include <coolingSchedule/moCoolingSchedule.h>
#include <algo/moSA.h>

//comparator
#include <comparator/moSolNeighborComparator.h>

//continuators
#include <continuator/moTrueContinuator.h>
#include <continuator/moCheckpoint.h>
#include <continuator/moFitnessStat.h>
#include <utils/eoFileMonitor.h>
#include <continuator/moCounterMonitorSaver.h>
#include "SA1/SA1Archive.h">"

//-----------------------------------------------------------------------------
// Define types of the representation solution, different neighbors and neighborhoods
//-----------------------------------------------------------------------------
//typedef eoInt<eoMinimizingFitness> Queen; //Permutation (Queen's problem representation)

typedef SA1moShiftNeighbor<EOStd> shiftNeighbor; //shift Neighbor
typedef SA1moRndWithReplNeighborhood<shiftNeighbor> SA1rndShiftNeighborhood; //rnd shift Neighborhood (Indexed)



SA1::SA1():EABase()
{
	setDefaultParameters();
}

SA1::SA1(Project* _project,Problem* _problem,ModClassTree* _modClassTree)
:EABase(_project,_problem,_modClassTree)
{
	setDefaultParameters();
};


SA1::SA1(Project* _project,Problem* _problem,ModClassTree* _modClassTree,MOParameters* parameters)
:EABase(_project,_problem,_modClassTree)
{
    delete _parameters;
    _parameters = new MOParameters(*parameters);
};

SA1::SA1(const SA1 & ea):EABase(ea)
{
}

SA1* SA1::clone() const
{
	SA1* newEA = new SA1(*this);
	return newEA ;
}

QString SA1::name()
{
	return("SA1");
}

void SA1::setDefaultParameters()
{
    SA1Parameters::setDefaultParameters(_parameters); //////////////////
}

bool SA1::acceptMultiObjectives()
{
    return false;
}

// main
Result* SA1::launch(QString tempDir)
{
	// init random
	rng.reseed(time(NULL));

	int argc=0;
	char *argv1 = "";
	char **argv = &argv1;

        //eoParser parser(argc, argv);  // for user-parameter reading
	eoState state;




        /* =========================================================
            *
            * Parameters
            *
            * ========================================================= */

            // First define a parser from the command-line arguments
            eoParser parser(argc, argv);

            // For each parameter, define Parameter, read it through the parser,
            // and assign the value to the variable
            eoValueParam<uint32_t> seedParam(time(0), "seed", "Random number seed", 'S');
            parser.processParam( seedParam );
            unsigned seed = seedParam.value();

            // description of genotype
            eoValueParam<unsigned int> vecSizeParam(8, "vecSize", "Genotype size", 'V');
            parser.processParam( vecSizeParam, "Representation" );
            unsigned vecSize = vecSizeParam.value();

            // the name of the "status" file where all actual parameter values will be saved
            string str_status = parser.ProgramName() + ".status"; // default value
            eoValueParam<string> statusParam(str_status.c_str(), "status", "Status file");
            parser.processParam( statusParam, "Persistence" );

            // do the following AFTER ALL PARAMETERS HAVE BEEN PROCESSED
            // i.e. in case you need parameters somewhere else, postpone these
            if (parser.userNeedsHelp()) {
                parser.printHelp(cout);
                exit(1);
            }
            if (statusParam.value() != "") {
                ofstream os(statusParam.value().c_str());
                os << parser;// and you can use that file as parameter file
            }

            /* =========================================================
             *
             * Random seed
             *
             * ========================================================= */

            //reproducible random seed: if you don't change SEED above,
            // you'll always get the same result, NOT a random run
            rng.reseed(seed);


                /************************************
                BOUNDS
                ************************************/
                std::vector<eoRealInterval> doubleBounds;
                std::vector<eoIntInterval> intBounds;
                int nbDouble=0,nbInt=0,nbBool=0;


    EAStdBounds::setBounds((Optimization*)_problem,_subModels,doubleBounds,intBounds,nbDouble,nbInt,nbBool);

                /************************************
                PROGRESS
                ************************************/
                    OMEAProgress* omEAProgress = new OMEAProgress();
                    connect(omEAProgress,SIGNAL(newProgress(float)),_problem,SIGNAL(newProgress(float)));
                    connect(omEAProgress,SIGNAL(newProgress(float,int,int)),_problem,SIGNAL(newProgress(float,int,int)));
                    int totalEval = _parameters->value(SA1Parameters::MAXITERATIONS,100).toInt();

            /* =========================================================
             *
             * Eval fitness function
             *
             * ========================================================= */
                moeoEvalFunc < EOStd > *plainEval;
                plainEval = new EAStdOptimizationEval<EOStd>(_project,(Optimization*)_problem,_subModels,tempDir,_modClassTree);

                OMEAEvalFuncCounter<EOStd>* eval = new OMEAEvalFuncCounter<EOStd> (* plainEval,omEAProgress,totalEval);
                state.storeFunctor(eval);




            /* =========================================================
             *
             * Initilisation of the solution
             *
             * ========================================================= */

            EAStdInitBounded<EOStd> *init = new EAStdInitBounded<EOStd>(doubleBounds,intBounds,nbBool);
            state.storeFunctor(init);
            /* =========================================================
             *
             * evaluation of a neighbor solution
             *
             * ========================================================= */

            SA1moFullEvalByCopy<shiftNeighbor> shiftEval(*plainEval);

            /* =========================================================
             *
             * the neighborhood of a solution
             *
             * ========================================================= */


            SA1rndShiftNeighborhood rndShiftNH(nbDouble+nbInt+nbBool, doubleBounds, intBounds, nbBool);



                /************************************
                STOPPING CRITERIA
                ************************************/

//                MyEAEvalContinue<EOStd> *evalCont = new MyEAEvalContinue<EOStd>(*eval,totalEval,&_stop);
//                state.storeFunctor(evalCont);


            /* =========================================================
             *
             * execute the local search from random solution
             *
             * ========================================================= */

            EOStd  solution2;

//            /* =========================================================
//             *
//             * the cooling schedule of the process
//             *
//             * ========================================================= */

//            // initial temp, factor of decrease, number of steps without decrease, final temp.
            moSimpleCoolingSchedule<EOStd> coolingSchedule(_parameters->value(SA1Parameters::INITTEMPERATURE,100).toDouble(), _parameters->value(SA1Parameters::DECREASINGFACTOR,0.9).toDouble(), _parameters->value(SA1Parameters::SPAN,100).toDouble(), _parameters->value(SA1Parameters::FINALTEMPERATURE,1).toDouble());

//            /* =========================================================
//             *
//             * Comparator of neighbors
//             *
//             * ========================================================= */

//            SA1moSolNeighborComparator<shiftNeighbor> solComparator;

//            /* =========================================================
//             *
//             * Example of Checkpointing
//             *
//             * ========================================================= */

            moTrueContinuator<shiftNeighbor> continuator;//always continue
            moCheckpoint<shiftNeighbor> checkpoint2(continuator);
            moFitnessStat<EOStd> fitStat;
            checkpoint2.add(fitStat);
            eoFileMonitor monitor("fitness.out", "");
            moCounterMonitorSaver countMon(100, monitor);
            checkpoint2.add(countMon);
            monitor.add(fitStat);
            // display obj vector in GUI
            EAUpdaterDispObjGUIOneSol < EOStd > disp_updater(solution2);
            checkpoint2.add(disp_updater);

//            moTrueContinuator<shiftNeighbor> continuator2;//always continue
//            moCheckpoint<shiftNeighbor> checkpoint2(continuator2);
//            moFitnessStat<EOStd> fitStat2;
//            checkpoint1.add(fitStat2);
//            // display obj vector in GUI
//            EAUpdaterDispObjGUIOneSol < EOStd > disp_updater2(solution2);
//            checkpoint2.add(disp_updater2);
            SA1moSolNeighborComparator<shiftNeighbor> solComparator;

            SA1mo<shiftNeighbor> localSearch2(rndShiftNH, *plainEval, shiftEval, coolingSchedule, solComparator, checkpoint2);

            (*init)(solution2);

            (*plainEval)(solution2);

            std::cout << "#########################################" << std::endl;
            std::cout << "initial solution2: " << solution2 << std::endl ;

            localSearch2(solution2);






        ///************************************
        //GETTING RESULT FROM FINAL ARCHIVE
        //************************************/
        Result* result = buildResult(arch);


        return result;

}


