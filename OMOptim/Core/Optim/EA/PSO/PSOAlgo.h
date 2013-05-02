#ifndef PSOALGO_H
#define PSOALGO_H

// $Id: NSGA2Algo.h 9827 2011-09-15 18:05:03Z hubert.thieriot $
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

        @file NSGA2Algo.h
        @brief Comments for file documentation.
        @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
        Company : CEP - ARMINES (France)
        http://www-cep.ensmp.fr/english/
        @version 0.9

  */

#include "PSOParameters.h"
#include "MOParameter.h"
#include "PSOBreed.h"
#include "updatePbest.h"
#include "updateGbest.h"
#include "updateVelocity.h"

#include <moeo>







template < class MOEOT >
class PSOAlgo : public moeoEA < MOEOT >
{
public:

     //  typedef typename MOEOT::ObjectiveVector ObjectiveVector;

        PSOAlgo(eoContinue < MOEOT > & _continuator, eoEvalFunc < MOEOT > & _eval, eoQuadOp < MOEOT > & _crossover,eoMonOp < MOEOT > & _mutation):
            defaultGenContinuator(0), continuator(_continuator), eval(_eval), defaultPopEval(_eval), popEval(defaultPopEval), select (2), selectMany(select,0.0),
            selectTransform(defaultSelect, defaultTransform),  defaultSGAGenOp(_crossover, 1, _mutation, 1), genBreed (select, defaultSGAGenOp), breed (genBreed),
            replace (fitnessAssignment, diversityAssignment)
    {}


void operator () (eoPop < MOEOT > &_pop, MOParameters *_parameters, moeoUnboundedArchive<MOEOT>& arch, const int & nbDouble,const int & nbInt, const int & nbBool,
                          const std::vector<eoRealInterval> & doubleBounds, const std::vector<eoIntInterval> & intBounds)
    {
      //  eoPop < MOEOT > offspring, empty_pop;

    double inertia = _parameters->value(PSOParameters::str(PSOParameters::INERTIA),1.0).toDouble();
        double alpha   = _parameters->value(PSOParameters::str(PSOParameters::LEARNINGFACTORALPHA),2.0).toDouble();
        double betta   = _parameters->value(PSOParameters::str(PSOParameters::LEARNINGFACTORBETTA),2.0).toDouble();


        eoPop <MOEOT> pbest; // best particle points
        MOEOT gbest; // global best point
        std::vector <MOEOT> velocity;

        popEval (_pop, _pop);  // a first eval of _pop

        // evaluate fitness and diversity
         fitnessAssignment(_pop);
         diversityAssignment(_pop);

       // initialisation of particle best point
        for(int i=0; i< _pop.size(); i++)
        {
            pbest.push_back(_pop[i]);
//            pbest[i].doubleVars = _pop[i].doubleVars;
//            pbest[i].intVars    = _pop[i].intVars;
//            pbest[i].boolVars   = _pop[i].boolVars;
//            pbest[i].objectiveVector(_pop[i].objectiveVector());
        }


        // Archive initialisation
        arch(_pop);

        // initialisation of the global best point
        gbest.doubleVars = arch[0].doubleVars;
        gbest.intVars    = arch[0].intVars;
        gbest.boolVars   = arch[0].boolVars;
        gbest.objectiveVector(arch[0].objectiveVector());


        // velocity initialisation to zero

        velocity.resize(_pop.size());

        for(int i=0; i< _pop.size(); i++ )
        {
            for(int j=0; j<nbDouble; j++)
                (velocity[i].doubleVars).push_back(0);
            for(int j=0; j<nbInt; j++)
                (velocity[i].intVars).push_back(0);
            for(int j=0; j<nbBool; j++)
                (velocity[i].boolVars).push_back(0);
        }


        do
        {
            // generate offspring, worths are recalculated if necessary
           // psobreed (_pop, _pop);
            psobreed (_pop, pbest, gbest, velocity, nbDouble, nbInt, nbBool, doubleBounds, intBounds);
            // eval of offspring

            popEval (pbest, _pop);
            fitnessAssignment (_pop);
            diversityAssignment (_pop);
         //   arch(_pop);
            updatePbest(pbest, _pop, paretoComp);
            updateGbest(gbest, arch);
            updateVelocity(velocity, _pop, pbest, gbest, inertia, alpha, betta);
            // after replace, the new pop is in _pop. Worths are recalculated if necessary
      //      replace (_pop, offspring);


//        popEval (empty_pop, _pop);  // a first eval of _pop
//        // evaluate fitness and diversity
//        fitnessAssignment(_pop);
//        diversityAssignment(_pop);
//        do
//        {
//            // generate offspring, worths are recalculated if necessary
//            breed (_pop, offspring);
//            // eval of offspring
//            popEval (_pop, offspring);
//            // after replace, the new pop is in _pop. Worths are recalculated if necessary
//            replace (_pop, offspring);
        }
        while (continuator (_pop));
    }


virtual void operator () (eoPop<MOEOT> &_pop)
{}

protected:

    /** a continuator based on the number of generations (used as default) */
    eoGenContinue < MOEOT > defaultGenContinuator;
    /** stopping criteria */
    eoContinue < MOEOT > & continuator;
    /**Pareto comparator */
    moeoParetoObjectiveVectorComparator<typename MOEOT::ObjectiveVector> paretoComp;
    UpdatePbest < MOEOT > updatePbest;
    UpdateGbest < MOEOT > updateGbest;
    UpdateVelocity < MOEOT > updateVelocity;
    /** default eval */
    class DummyEval : public eoEvalFunc < MOEOT >
    {
    public:
        void operator()(MOEOT &) {}
    }
    defaultEval;
    /** evaluation function */
    eoEvalFunc < MOEOT > & eval;
    /** default popEval */
    eoPopLoopEval < MOEOT > defaultPopEval;
    /** evaluation function used to evaluate the whole population */
    eoPopEvalFunc < MOEOT > & popEval;
    /** default select */
    class DummySelect : public eoSelect < MOEOT >
    {
    public :
        void operator()(const eoPop<MOEOT>&, eoPop<MOEOT>&) {}
    }
    defaultSelect;
    /** binary tournament selection */
    moeoDetTournamentSelect < MOEOT > select;
    /** default select many */
    eoSelectMany < MOEOT >  selectMany;
    /** select transform */
    eoSelectTransform < MOEOT > selectTransform;
    /** a default crossover */
    eoQuadCloneOp < MOEOT > defaultQuadOp;
    /** a default mutation */
    eoMonCloneOp < MOEOT > defaultMonOp;
    /** an object for genetic operators (used as default) */
    eoSGAGenOp < MOEOT > defaultSGAGenOp;
    /** default transform */
    class DummyTransform : public eoTransform < MOEOT >
    {
    public :
        void operator()(eoPop<MOEOT>&) {}
    }
    defaultTransform;
    /** general breeder */
    eoGeneralBreeder < MOEOT > genBreed;
    /** breeder */
    eoBreed  < MOEOT > & breed;
    PSOBreed < MOEOT >  psobreed;
    /** fitness assignment used in NSGA */
    moeoDominanceDepthFitnessAssignment < MOEOT > fitnessAssignment;
    /** diversity assignment used in NSGA-II */
    moeoFrontByFrontCrowdingDiversityAssignment  < MOEOT > diversityAssignment;
    /** elitist replacement */
    moeoElitistReplacement < MOEOT > replace;


};






#endif // PSOALGO_H
