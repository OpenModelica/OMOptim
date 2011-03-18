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
#include "MyEAEvalFuncCounter.h"
#include "MyEAProgress.h"
#include "MyEAEvalContinue.h"
#include "EAStdCheckPoint.h"
#include "MyEoGnuplot1DMonitor.h"
#include "EAStdOptimizationEval.h"
#include "EAStdResult.h"


SPEA2Adapt::SPEA2Adapt():EABase()
{
	setDefaultParameters();
}

SPEA2Adapt::SPEA2Adapt(Project* project,Problem* problem,ModReader* modReader,ModPlusCtrl* modPlusReader,ModClass* rootClass)
:EABase(project,problem,modReader,modPlusReader,rootClass)
{
	setDefaultParameters();
};


SPEA2Adapt::SPEA2Adapt(Project* project,Problem* problem,ModReader* modReader,ModPlusCtrl* modPlusReader,ModClass* rootClass,EAConfig* eaConfig)
:EABase(project,problem,modReader,modPlusReader,rootClass)
{
	_config = eaConfig;	
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
	_config->parameters->addItem(new AlgoParameter("PopulationSize","Population size",50,"int",1,1000));
	_config->parameters->addItem(new AlgoParameter("OffspringRate","OffSpring size/PopulationSize",3,"double",1,1000));
	_config->parameters->addItem(new AlgoParameter("MaxGen","Max Generations",100,"int",1,10000));
	_config->parameters->addItem(new AlgoParameter("SaveFrequency","Population saving frequency (# generations, 0 = save only final state)",1,"int",0,10000000));
	_config->parameters->addItem(new AlgoParameter("ReinitStdDev","Reinitialize StdDeviation (for pursuing optimization only)",0,"int",0,1));
}

QList<int> SPEA2Adapt::compatibleProblems()
{
	QList<int> problems;
	problems.push_back(Problem::OPTIMIZATION);
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
	case Problem::OPTIMIZATION :
		EAStdBounds::setBounds((Optimization*)_problem,_subModels,doubleBounds,intBounds,nbDouble,nbInt,nbBool);
		break;
	}

	/************************************
	PROGRESS AND CONTINUATOR
	************************************/
	unsigned initPopSize = _config->getParameterValue("PopulationSize",20);
	unsigned offSpringRate = _config->getParameterValue("OffspringRate",0);
	unsigned nTotalGen = _config->getParameterValue("MaxGen",0);
	unsigned nTotalEvals = initPopSize + initPopSize*nTotalGen*offSpringRate;

	MyEAProgress myEAProgress;
	connect(&myEAProgress,SIGNAL(newProgress(float)),_problem,SIGNAL(newProgress(float)));
	connect(&myEAProgress,SIGNAL(newProgress(float,int,int)),_problem,SIGNAL(newProgress(float,int,int)));
			
	eoGenContinue < EOAdapt > genContinuator(nTotalGen);

	/************************************
	FITNESS EVALUATION
	************************************/
	moeoEvalFunc < EOAdapt > *plainEval;
	switch(_problem->type())
	{
	case Problem::OPTIMIZATION :
		plainEval = new EAStdOptimizationEval<EOAdapt>(_project,(Optimization*)_problem,_subModels,tempDir,
			_modReader,_modPlusReader,_rootModClass);
		break;
	}
	MyEAEvalFuncCounter<EOAdapt>* eval = new MyEAEvalFuncCounter<EOAdapt> (* plainEval,&myEAProgress,nTotalEvals);


	

	//************************************
	//INITIAL POPULATION
	//************************************/
	SPEA2AdaptInitBounded<EOAdapt> *init = new SPEA2AdaptInitBounded<EOAdapt>(doubleBounds,intBounds,nbBool,initPopSize);
	//state.storeFunctor(init);

	   
	///************************************
	//CROSSOVER AND MUTATION
	//************************************/
	SBCrossover<EOAdapt> *xover = new SBCrossover <EOAdapt>((EAConfig*)_config);
	//state.storeFunctor(xover);

	EAAdapt1Mutation<EOAdapt> *mutation = new EAAdapt1Mutation<EOAdapt>(doubleBounds,intBounds,(EAConfig*)_config);
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
	if(((EAConfig*)_config)->getUseSartFile() && ((EAConfig*)_config)->getReloadFilePath()!="" && QFileInfo(((EAConfig*)_config)->getReloadFilePath()).exists())
	{
		// create another state for reading
		eoState inState;		// a state for loading - WITHOUT the parser
		// register the rng and the pop in the state, so they can be loaded,
		// and the present run will be the exact continuation of the saved run
		// eventually with different parameters
		inState.registerObject(pop);
		inState.registerObject(rng);
		
		QString path = ((EAConfig*)_config)->getReloadFilePath();
		std::string str = path.toLatin1().data();
		try{
			inState.load(str);
		}
		catch(std::exception &e)
		{
                    infoSender.send(Info("Loading start file failed :"+QString(e.what()),ListInfo::WARNING2));
			loadFailed = true;
		}
		unsigned resetStdDev = _config->getParameterValue("ReinitStdDev",0);
		if(resetStdDev)
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
	eoCheckPoint<EOAdapt>& checkpoint = createEAStdCheckPoint(parser, state, *eval, *evalCont, pop, arch,_project,(EAConfig*)_config,tempDir);

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
	double rate = _config->getParameterValue("OffspringRate",3);
	SPEA2Algo<EOAdapt> spea2(checkpoint,*eval,*xover,1,*mutation,1,arch,initPopSize,rate,true);

	///************************************
	//RUN THE ALGO
	//************************************/
	QTime _time;
	QDate beginDate = QDate::currentDate();
	QTime beginTime = QTime::currentTime();

	_time.start();
	spea2(pop);

	QTime compTime(0,0,0,0);
	int nms = _time.elapsed();
	compTime = compTime.addSecs(nms/1000);


	///************************************
	//GETTING RESULT FROM FINAL ARCHIVE
	//************************************/
	Result* result = buildResult(arch,(EAConfig*)_config);

	result->_hour = beginTime;
	result->_computationTime = compTime;
	result->_date = beginDate;

	return result;
}

