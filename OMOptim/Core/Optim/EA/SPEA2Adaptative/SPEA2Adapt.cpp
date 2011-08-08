// $Id$
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

 	@file SPEA2Adapt.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

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

// Specific Adaptive headers
#include "SPEA2Adapt.h"
#include "EOAdapt.h"
#include "SPEA2AdaptInitBounded.h"
#include "SPEA2AdaptMutation.h"
#include "SPEA2Algo.h"

// General EA headers
#include "SBCrossover.h"
#include "EAStdBounds.h"
#include "OMEAEvalFuncCounter.h"
#include "OMEAProgress.h"
#include "OMEAEvalContinue.h"
#include "EAStdCheckPoint.h"
#include "MyEoGnuplot1DMonitor.h"
#include "EAStdOptimizationEval.h"
#include "EAStdResult.h"


SPEA2Adapt::SPEA2Adapt():EABase()
{
	setDefaultParameters();
}

SPEA2Adapt::SPEA2Adapt(Project* project,Problem* problem,ModClassTree* modClassTree,ModPlusCtrl* modPlusCtrl)
:EABase(project,problem,modClassTree,modPlusCtrl)
{
	setDefaultParameters();
};


SPEA2Adapt::SPEA2Adapt(Project* project,Problem* problem,ModClassTree* modClassTree,ModPlusCtrl* modPlusCtrl,MOParameters* parameters)
:EABase(project,problem,modClassTree,modPlusCtrl)
{
    delete _parameters;
    _parameters = new MOParameters(*parameters);
};

SPEA2Adapt::SPEA2Adapt(const SPEA2Adapt & ea):EABase(ea)
{
}

SPEA2Adapt* SPEA2Adapt::clone()
{
	SPEA2Adapt* newEA = new SPEA2Adapt(*this);
	return newEA ;
}

QString SPEA2Adapt::name()
{
	return("SPEA2Adapt");
}

void SPEA2Adapt::setDefaultParameters()
{
    SPEA2AdaptParameters::setDefaultParameters(_parameters);
}

QList<int> SPEA2Adapt::compatibleOMCases()
{
	QList<int> problems;
	problems.push_back(Problem::OPTIMIZATIONTYPE);
	return problems;
}


// main
Result* SPEA2Adapt::launch(QString tempDir)
{
	// init random
	rng.reseed(time(NULL));

	int argc=0;
	char *argv1 = "";
	char **argv = &argv1;

	eoParser parser(argc, argv);  // for user-parameter reading
	eoState state;

	/************************************
	BOUNDS
	************************************/
	std::vector<eoRealInterval> doubleBounds;
	std::vector<eoIntInterval> intBounds;
	int nbDouble=0,nbInt=0,nbBool=0;
	
	switch(_problem->type())
	{
	case Problem::OPTIMIZATIONTYPE :
		EAStdBounds::setBounds((Optimization*)_problem,_subModels,doubleBounds,intBounds,nbDouble,nbInt,nbBool);
		break;
	}

	/************************************
	PROGRESS AND CONTINUATOR
	************************************/
        unsigned initPopSize = _parameters->value(SPEA2AdaptParameters::POPULATIONSIZE,20).toInt();
        unsigned offSpringRate = _parameters->value(SPEA2AdaptParameters::OFFSPRINGRATE,3).toInt();
        unsigned nTotalGen = _parameters->value(SPEA2AdaptParameters::MAXGENERATIONS,100).toInt();
	unsigned nTotalEvals = initPopSize + initPopSize*nTotalGen*offSpringRate;

	OMEAProgress OMEAProgress;
	connect(&OMEAProgress,SIGNAL(newProgress(float)),_problem,SIGNAL(newProgress(float)));
	connect(&OMEAProgress,SIGNAL(newProgress(float,int,int)),_problem,SIGNAL(newProgress(float,int,int)));
			
	eoGenContinue < EOAdapt > genContinuator(nTotalGen);

	/************************************
	FITNESS EVALUATION
	************************************/
	moeoEvalFunc < EOAdapt > *plainEval;
	switch(_problem->type())
	{
	case Problem::OPTIMIZATIONTYPE :
		plainEval = new EAStdOptimizationEval<EOAdapt>(_project,(Optimization*)_problem,_subModels,tempDir,
                        _modClassTree,_modPlusCtrl);
		break;
	}
	OMEAEvalFuncCounter<EOAdapt>* eval = new OMEAEvalFuncCounter<EOAdapt> (* plainEval,&OMEAProgress,nTotalEvals);


	

	//************************************
	//INITIAL POPULATION
	//************************************/
	SPEA2AdaptInitBounded<EOAdapt> *init = new SPEA2AdaptInitBounded<EOAdapt>(doubleBounds,intBounds,nbBool,initPopSize);
	//state.storeFunctor(init);

	   
	///************************************
	//CROSSOVER AND MUTATION
	//************************************/
        SBCrossover<EOAdapt> *xover = new SBCrossover <EOAdapt>(_parameters);
	//state.storeFunctor(xover);

        EAAdapt1Mutation<EOAdapt> *mutation = new EAAdapt1Mutation<EOAdapt>(doubleBounds,intBounds,_parameters);
	//state.storeFunctor(mutation);

	eoSequentialOp<EOAdapt> *op = new eoSequentialOp<EOAdapt>;
	//state.storeFunctor(op);
	op -> add(*xover, 1.0);	 // always do crossover (probabilities are taken into account inside)
	op -> add(*mutation, 1.0); // and mutation 

	/************************************
	POPULATION
	************************************/
	//eoPop<EOAdapt>& pop = state.takeOwnership(eoPop<EOAdapt>());
	eoPop<EOAdapt> pop;
	bool loadFailed=false;
        bool useStartFile = _parameters->value(SPEA2AdaptParameters::USESTARTFILE,false).toBool();
        QString reloadFilePath = _parameters->value(SPEA2AdaptParameters::STARTFILEPATH).toString();

        if(useStartFile && (reloadFilePath!="") && QFileInfo(reloadFilePath).exists())
	{

		// create another state for reading
		eoState inState;		// a state for loading - WITHOUT the parser
		// register the rng and the pop in the state, so they can be loaded,
		// and the present run will be the exact continuation of the saved run
		// eventually with different parameters
		inState.registerObject(pop);
		inState.registerObject(rng);
		

                std::string str = reloadFilePath.toLatin1().data();
		try{
			inState.load(str);
		}
		catch(std::exception &e)
		{
                    infoSender.send(Info("Loading start file failed :"+QString(e.what()),ListInfo::WARNING2));
			loadFailed = true;
		}
                bool reinitStdDev= _parameters->value(SPEA2AdaptParameters::REINITSTDDEV).toBool();
                if(reinitStdDev)
		{
			EAAdaptReinitStdDev<EOAdapt>::reinitDblStdDev(pop,doubleBounds,initPopSize);
		}
	}

	if(loadFailed)
	{
		pop.clear();
		pop = state.takeOwnership(eoPop<EOAdapt>());
	}

	if(pop.size() < initPopSize)
	{
		pop.append(initPopSize,*init);
	}

	// for future stateSave, register the algorithm into the state
	state.registerObject(parser);
	state.registerObject(pop);
	state.registerObject(rng);	


	/************************************
	ARCHIVE
	************************************/
 	moeoUnboundedArchive<EOAdapt> arch;

	/************************************
	STOPPING CRITERIA
	************************************/
	MyEAEvalContinue<EOAdapt> *evalCont = new MyEAEvalContinue<EOAdapt>(*eval,nTotalEvals,&_stop);
	//state.storeFunctor(evalCont);

	/************************************
	OUTPUT
	************************************/
        eoCheckPoint<EOAdapt>& checkpoint = createEAStdCheckPoint(parser, state, *eval, *evalCont, pop, arch,_project,_parameters,tempDir);

	/************************************
	MONITOR
	************************************/
	 // Create a counter parameter
    eoValueParam<unsigned> generationCounter(0, "Gen.");
    // now some statistics on the population:
    // Best fitness in population
   
    // the Fitness Distance Correlation
    // need first an object to compute the distances
 //   eoQuadDistance<EOAdapt> dist;
	//// Hamming distance
 //   eoFDCStat<EOAdapt> fdcStat(dist);
 //   // Add them to the checkpoint to get them called at the appropriate time
 //   checkpoint.add(averageStat);
 //   checkpoint.add(fdcStat);

	// The Stdout monitor will print parameters to the screen ...
    MyEoGnuplot1DMonitor monitor("export_monitor.xg",false);
    // when called by the checkpoint (i.e. at every generation)
    checkpoint.add(monitor);

    // the monitor will output a series of parameters: add them
    monitor.add(generationCounter);
   // monitor.add(eval);		// because now eval is an eoEvalFuncCounter!
    //monitor.add(bestStat);
    //monitor.add(SecondStat);
    //monitor.add(fdcStat);

	///************************************
	//BUILD SPEAAdapt1
	//************************************/
        double rate = _parameters->value(SPEA2AdaptParameters::OFFSPRINGRATE,3).toDouble();
	SPEA2Algo<EOAdapt> spea2(checkpoint,*eval,*xover,1,*mutation,1,arch,initPopSize,rate,true);

	///************************************
	//RUN THE ALGO
	//************************************/
	spea2(pop);


	///************************************
	//GETTING RESULT FROM FINAL ARCHIVE
	//************************************/
        Result* result = buildResult(arch);


	return result;
}

