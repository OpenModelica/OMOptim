// $Id: NSGA2Algo.h 9677 2011-08-24 13:09:00Z hubert.thieriot $
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
     @version

  */


#ifndef _MOSA1_H
#define _MOSA1_H

#include <moeo>


//template < class MOEOT >
//class NSGA2Algo : public moeoEA < MOEOT >
//{
//public:


//    NSGA2Algo(eoContinue < MOEOT > & _continuator, eoEvalFunc < MOEOT > & _eval, eoQuadOp < MOEOT > & _crossover,eoMonOp < MOEOT > & _mutation) :
//            defaultGenContinuator(0), continuator(_continuator), eval(_eval), defaultPopEval(_eval), popEval(defaultPopEval), select (2), selectMany(select,0.0), selectTransform(defaultSelect, defaultTransform), defaultSGAGenOp(_crossover, 1, _mutation, 1), genBreed (select, defaultSGAGenOp), breed (genBreed), replace (fitnessAssignment, diversityAssignment)
//    {}


//virtual void operator () (eoPop < MOEOT > &_pop)
//    {
//        eoPop < MOEOT > offspring, empty_pop;
//        popEval (empty_pop, _pop);    // a first eval of _pop
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
//        }
//        while (continuator (_pop));
//    }


//protected:

//    /** a continuator based on the number of generations (used as default) */
//    eoGenContinue < MOEOT > defaultGenContinuator;
//    /** stopping criteria */
//    eoContinue < MOEOT > & continuator;
//    /** default eval */
//    class DummyEval : public eoEvalFunc < MOEOT >
//    {
//    public:
//        void operator()(MOEOT &) {}
//    }
//    defaultEval;
//    /** evaluation function */
//    eoEvalFunc < MOEOT > & eval;
//    /** default popEval */
//    eoPopLoopEval < MOEOT > defaultPopEval;
//    /** evaluation function used to evaluate the whole population */
//    eoPopEvalFunc < MOEOT > & popEval;
//    /** default select */
//    class DummySelect : public eoSelect < MOEOT >
//    {
//    public :
//        void operator()(const eoPop<MOEOT>&, eoPop<MOEOT>&) {}
//    }
//    defaultSelect;
//    /** binary tournament selection */
//    moeoDetTournamentSelect < MOEOT > select;
//    /** default select many */
//    eoSelectMany < MOEOT >  selectMany;
//    /** select transform */
//    eoSelectTransform < MOEOT > selectTransform;
//    /** a default crossover */
//    eoQuadCloneOp < MOEOT > defaultQuadOp;
//    /** a default mutation */
//    eoMonCloneOp < MOEOT > defaultMonOp;
//    /** an object for genetic operators (used as default) */
//    eoSGAGenOp < MOEOT > defaultSGAGenOp;
//    /** default transform */
//    class DummyTransform : public eoTransform < MOEOT >
//    {
//    public :
//        void operator()(eoPop<MOEOT>&) {}
//    }
//    defaultTransform;
//    /** general breeder */
//    eoGeneralBreeder < MOEOT > genBreed;
//    /** breeder */
//    eoBreed < MOEOT > & breed;
//    /** fitness assignment used in NSGA */
//    moeoDominanceDepthFitnessAssignment < MOEOT > fitnessAssignment;
//    /** diversity assignment used in NSGA-II */
//    moeoFrontByFrontCrowdingDiversityAssignment  < MOEOT > diversityAssignment;
//    /** elitist replacement */
//    moeoElitistReplacement < MOEOT > replace;


//};



#endif
