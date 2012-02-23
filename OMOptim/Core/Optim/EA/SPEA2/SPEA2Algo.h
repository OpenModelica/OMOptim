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

 	@file SPEA2Algo.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#ifndef _SPEA2Algo_H
#define _SPEA2Algo_H


#include <moeo>

/*
* <SPEA2Algo.h>
* This file is a slight modification from moeoSPEA2.h
* (see this file for copyright information)
* It's introducing constructor option to specify offpring size (by absolute value or by factor of pop size)
*/

#include <eoBreed.h>
#include <eoCloneOps.h>
#include <eoContinue.h>
#include <eoEvalFunc.h>
#include <eoGenContinue.h>
#include <eoGeneralBreeder.h>
#include <eoGenOp.h>
#include <eoPopEvalFunc.h>
#include <eoSGAGenOp.h>
#include <algo/moeoEA.h>
#include <diversity/moeoNearestNeighborDiversityAssignment.h>
#include <fitness/moeoDominanceCountFitnessAssignment.h>
#include <fitness/moeoDominanceCountRankingFitnessAssignment.h>
#include <replacement/moeoGenerationalReplacement.h>
#include <selection/moeoDetTournamentSelect.h>
#include <archive/moeoFixedSizeArchive.h>
#include <distance/moeoEuclideanDistance.h>
#include <selection/moeoSelectFromPopAndArch.h>

/**
 * SPEA2 algorithm.
 * E. Zitzler, M. Laumanns, and L. Thiele. SPEA2: Improving the Strength Pareto Evolutionary Algorithm. Technical Report 103,
 * Computer Engineering and Networks Laboratory (TIK), ETH Zurich, Zurich, Switzerland, 2001.
 */

template < class MOEOT >
class SPEA2Algo: public moeoEA < MOEOT >
{
public:

   /**
        * Ctor with a crossover, a mutation and their corresponding rates.
        * @param _continuator stopping criteria
        * @param _eval evaluation function
        * @param _crossover crossover
        * @param _pCross crossover probability
        * @param _mutation mutation
        * @param _pMut mutation probability
        * @param _archive archive
        * @param _k the k-ieme distance used to fixe diversity
        * @param _nocopy boolean allow to consider copies and doublons as bad elements whose were dominated by all other MOEOT in fitness assignment.
        */
    SPEA2Algo(eoContinue < MOEOT >& _continuator, eoEvalFunc < MOEOT > & _eval, eoQuadOp < MOEOT > & _crossover, double _pCross, eoMonOp < MOEOT > & _mutation, double _pMut, moeoArchive < MOEOT >& _archive, int _popSize, double  _rate=1.0, bool _interpret_as_rate = true, int _k=1, bool _nocopy=false) :
            defaultGenContinuator(0), continuator(_continuator), eval(_eval), loopEval(_eval), popEval(loopEval), archive(_archive),defaultSelect(2),select(defaultSelect, defaultSelect, _archive, 0.0),
            defaultSGAGenOp(_crossover, _pCross, _mutation, _pMut), fitnessAssignment(_archive, _nocopy),
            genBreed(defaultSelect, defaultSGAGenOp,_rate,_interpret_as_rate),selectMany(defaultSelect,_popSize), selectTransform(selectMany, dummyTransform), breed(genBreed), diversityAssignment(dist,_archive, _k),
			replace(fitnessAssignment,diversityAssignment)
    {
		popSize = _popSize;
	}



    /**
     * Apply a few generation of evolution to the population _pop until the stopping criteria is verified.
     * @param _pop the population
     */
    virtual void operator () (eoPop < MOEOT > &_pop)
    {
        eoPop < MOEOT >empty_pop, offspring;
        popEval (empty_pop, _pop);// a first eval of _pop
		fitnessAssignment(_pop); //a first fitness assignment of _pop
        diversityAssignment(_pop);//a first diversity assignment of _pop
		archive(_pop);//a first filling of archive

		// resize if _pop size > pop asked size (e.g. when load a saved file)
		if(_pop.size()>popSize)
		{
			// sorts the whole population according to the comparator
			moeoFitnessThenDiversityComparator < MOEOT > comparator;
			std::sort(_pop.begin(), _pop.end(), comparator);
			// finally, resize this global population
			_pop.resize(popSize);
		}

      
        while (continuator (_pop))
        {
            // generate offspring, worths are recalculated if necessary
            breed (_pop, offspring);
            popEval (_pop, offspring); // eval of offspring
            // after replace, the new pop is in _pop. Worths are recalculated if necessary
            replace (_pop, offspring); // include fitness and diversity assignments
 			
			//fitnessAssignment(_pop); //fitness assignment of _pop
            //diversityAssignment(_pop); //diversity assignment of _pop
            archive(_pop); //control of archive

        }
    }


protected:

	/** dummy evaluation */
	class eoDummyEval : public eoEvalFunc< MOEOT >
    {
    public:
        void operator()(MOEOT &) {}
    }
    dummyEval;

    /** dummy transform */
    class eoDummyTransform : public eoTransform<MOEOT>
    {
    public :
        void operator()(eoPop<MOEOT>&) {}
    }
    dummyTransform;

	int popSize;
    /** a continuator based on the number of generations (used as default) */
    eoGenContinue < MOEOT > defaultGenContinuator;
    /** stopping criteria */
    eoContinue < MOEOT > & continuator;
    /** evaluation function */
    eoEvalFunc < MOEOT > & eval;
    /** loop eval */
    eoPopLoopEval < MOEOT > loopEval;
    /** evaluation function used to evaluate the whole population */
    eoPopEvalFunc < MOEOT > & popEval;
    /**archive*/
    moeoArchive < MOEOT >& archive;
    /**SelectOne*/
    moeoDetTournamentSelect < MOEOT > defaultSelect;
    /** binary tournament selection */
    moeoSelectFromPopAndArch < MOEOT > select;
    /** a default mutation */
    eoMonCloneOp < MOEOT > defaultMonOp;
    /** a default crossover */
    eoQuadCloneOp < MOEOT > defaultQuadOp;
    /** an object for genetic operators (used as default) */
    eoSGAGenOp < MOEOT > defaultSGAGenOp;
    /** fitness assignment used in NSGA-II */
    moeoDominanceCountRankingFitnessAssignment < MOEOT > fitnessAssignment;
    /** general breeder */
    eoGeneralBreeder < MOEOT > genBreed;
    /** selectMany */
    eoSelectMany <MOEOT>  selectMany;
    /** select Transform*/
    eoSelectTransform <MOEOT> selectTransform;
    /** breeder */
    eoBreed < MOEOT > & breed;
    /** diversity assignment used in NSGA-II */
    moeoNearestNeighborDiversityAssignment  < MOEOT > diversityAssignment;
    /** elitist replacement */
	moeoElitistReplacement < MOEOT > replace;
   /**distance*/
    moeoEuclideanDistance < MOEOT > dist;


};

#endif /*MOSPEA2_H_*/


