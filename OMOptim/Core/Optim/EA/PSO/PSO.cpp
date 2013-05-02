// $Id: PSO.cpp 10617 2011-11-30 16:17:55Z hubert.thieriot $
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

        @file PSO.cpp
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

#include "PSO.h"
#include "EOStd.h"
#include "EAStdInitBounded.h"
#include "EAStdMutation.h"
#include "SBCrossover.h"
#include "EAStdCheckPoint.h"
#include "PSOAlgo.h"
#include "EAStdBounds.h"
#include "OMEAEvalFuncCounter.h"
#include "OMEAProgress.h"
#include "OMEAEvalContinue.h"
#include "MyEoGnuplot1DMonitor.h"
#include "EAStdOptimizationEval.h"
#include "EAStdResult.h"

PSO::PSO():EABase()
{
        setDefaultParameters();
}

PSO::PSO(Project* _project,Problem* _problem)
:EABase(_project,_problem)
{
        setDefaultParameters();
}


PSO::PSO(Project* _project,Problem* _problem,MOParameters* parameters)
:EABase(_project,_problem)
{
    delete _parameters;
    _parameters = new MOParameters(*parameters);
}

PSO::PSO(const PSO & ea):EABase(ea)
{
}

EABase* PSO::clone() const
{
        PSO* newEA = new PSO(*this);
        return newEA ;
}

QString PSO::name()
{
        return("PSO");
}

void PSO::setDefaultParameters()
{
    PSOParameters::setDefaultParameters(_parameters);
}


bool PSO::acceptMultiObjectives()
{
    return true;
}


// main
Result* PSO::launch(QString tempDir)
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
        int nbObj = ((Optimization*)_problem)->objectives()->size();

        EAStdBounds::setBounds((Optimization*)_problem/*,_subModels*/,doubleBounds,intBounds,nbDouble,nbInt,nbBool);


        /************************************
        PROGRESS
        ************************************/
        OMEAProgress* omEAProgress = new OMEAProgress();
        connect(omEAProgress,SIGNAL(newProgress(float)),_problem,SIGNAL(newProgress(float)));
        connect(omEAProgress,SIGNAL(newProgress(float,int,int)),_problem,SIGNAL(newProgress(float,int,int)));
        int totalEval = _parameters->value(PSOParameters::str(PSOParameters::MAXITERATIONS),50).toInt();

        /************************************
        FITNESS EVALUATION
        ************************************/
        moeoEvalFunc < EOStd > *plainEval;
        plainEval = new EAStdOptimizationEval<EOStd>(_project,(Optimization*)_problem,/*_subModels,*/tempDir,_modItemsTree,&_quickEnd);

        OMEAEvalFuncCounter<EOStd>* eval = new OMEAEvalFuncCounter<EOStd> (* plainEval,omEAProgress,totalEval);
        state.storeFunctor(eval);



        //************************************
        //INITIAL POPULATION
        //************************************/
        EAStdInitBounded<EOStd> *init = new EAStdInitBounded<EOStd>(doubleBounds,intBounds,nbBool);
        state.storeFunctor(init);


        /************************************
        CROSSOVER AND MUTATION
        ************************************/
        SBCrossover<EOStd> *xover = new SBCrossover<EOStd>(_parameters);
        state.storeFunctor(xover);

        EAStdMutation<EOStd> *mutation = new EAStdMutation<EOStd>(doubleBounds,intBounds,_parameters);
        state.storeFunctor(mutation);

        eoSequentialOp<EOStd> *op = new eoSequentialOp<EOStd>;
        state.storeFunctor(op);
        op -> add(*xover, 1.0);   // always do crossover (probabilities are taken into account inside)
        op -> add(*mutation, 1.0); // and mutation




        /************************************
        POPULATION
        ************************************/
        eoPop<EOStd> pop;
        bool loadFailed=false;
        bool useStartFile = _parameters->value(PSOParameters::str(PSOParameters::USESTARTFILE),false).toBool();
        QString reloadFilePath = _parameters->value(PSOParameters::str(PSOParameters::STARTFILEPATH)).toString();

        if(useStartFile && (reloadFilePath!="") && QFileInfo(reloadFilePath).exists())
        {
                // create another state for reading
                eoState inState;    // a state for loading - WITHOUT the parser
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
                        InfoSender::instance()->debug("loading start file failed :"+QString(e.what()));
                        loadFailed = true;
                }
        }


        if(loadFailed)
        {
                pop.clear();
                pop = state.takeOwnership(eoPop<EOStd>());
        }

        int populationSize = _parameters->value(PSOParameters::str(PSOParameters::POPULATIONSIZE),20).toInt();
        if(pop.size() < populationSize)
        {
                pop.append(populationSize,*init);
        }

        // for future stateSave, register the algorithm into the state
        state.registerObject(parser);
        state.registerObject(pop);
        state.registerObject(rng);


        /************************************
        ARCHIVE
        ************************************/
        moeoUnboundedArchive<EOStd> arch;


        /************************************
        STOPPING CRITERIA
        ************************************/
        MyEAEvalContinue<EOStd> *evalCont = new MyEAEvalContinue<EOStd>(*eval,totalEval,&_quickEnd);
        state.storeFunctor(evalCont);


        /************************************
        OUTPUT
        ************************************/
        eoCheckPoint < EOStd > & checkpoint = createEAStdCheckPoint(parser, state, *eval, *evalCont, pop, arch,_project,_parameters, tempDir, nbObj);


        ///************************************
        //BUILD NSGA-II
        //************************************/


        PSOAlgo < EOStd > psoa (checkpoint,*eval, *xover, *mutation);  // A changer, plus de mutation, plus de crossover

        ///************************************
        //RUN THE ALGO
        //************************************/

        psoa (pop, _parameters, arch, nbDouble,nbInt,nbBool,doubleBounds,intBounds);





        ///************************************
        //GETTING RESULT FROM FINAL ARCHIVE
        //************************************/
        if(!_keepResults) // if stop has been called
            return NULL;

        Result* result = buildResult(arch);

        return result;

}


