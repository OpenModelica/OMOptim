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

 	@file EAStdCheckPoint.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
/*
* <make_checkpoint_moeo.h>
* Copyright (C) DOLPHIN Project-Team, INRIA Futurs, 2006-2007
* (C) OPAC Team, LIFL, 2002-2007
*
* Arnaud Liefooghe
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

#ifndef EAStdCheckPoint_H
#define EAStdCheckPoint_H

#include <limits.h>
#include <stdlib.h>
#include <sstream>
#include <eoContinue.h>
#include <eoEvalFuncCounter.h>
#include <utils/checkpointing>
#include <utils/selectors.h>
#include <utils/eoParser.h>
#include <utils/eoState.h>
#include <metric/moeoContributionMetric.h>
#include <metric/moeoEntropyMetric.h>
#include <utils/moeoArchiveUpdater.h>
#include <utils/moeoArchiveObjectiveVectorSavingUpdater.h>
#include <utils/moeoBinaryMetricSavingUpdater.h>

#include "EAConfig.h"
#include "Project.h"
#include <QtCore/QObject>
#include "EAUpdaterDispObjGUI.h"


bool testDirRes(std::string _dirName, bool _erase);

/**
 * This functions allows to build an eoCheckPoint for multi-objective optimization from the parser (partly taken from make_checkpoint_pareto.h)
 * @param _parser the parser
 * @param _state to store allocated objects
 * @param _eval the funtions evaluator
 * @param _continue the stopping crietria
 * @param _pop the population
 * @param _archive the archive of non-dominated solutions
 */
template < class MOEOT >
eoCheckPoint < MOEOT > & createEAStdCheckPoint(eoParser & _parser, eoState & _state, eoEvalFuncCounter < MOEOT > & _eval,
											 eoContinue < MOEOT > & _continue, eoPop < MOEOT > & _pop, moeoArchive < MOEOT > & _archive,
											 Project* _project, EAConfig *_config,QString tempDir)
{
  eoCheckPoint < MOEOT > & checkpoint = _state.storeFunctor(new eoCheckPoint < MOEOT > (_continue));
  /* the objective vector type */
  typedef typename MOEOT::ObjectiveVector ObjectiveVector;


  QString saveFolder = tempDir;
  QString genSaveFilePath = tempDir+QDir::separator() + "iteration";
  QString archSaveFilePath = tempDir+QDir::separator() + "archive";
  QString contribSaveFilePath = tempDir+QDir::separator() + "contribution";
  QString secondStatsSaveFilePath = tempDir+QDir::separator() + "secondStats";
  QString bestStatsSaveFilePath = tempDir+QDir::separator() + "bestStats";
  
  
  unsigned int saveFreq = _config->getParameterValue("SaveFrequency",50);
   unsigned int maxIter = _config->getParameterValue("MaxIterations",50);


 
  eoValueParam<unsigned int> *evalCounter = new eoValueParam<unsigned int>(0, "Gen.");
  // Create an incrementor (sub-class of eoUpdater).
  eoIncrementor<unsigned int> & increment = _state.storeFunctor( new eoIncrementor<unsigned int>(evalCounter->value()) );
  // Add it to the checkpoint
  checkpoint.add(increment);
  

  //////////////////////////////////
  // State saver
  //////////////////////////////
  eoCountedStateSaver *stateSaver1;
  if(saveFreq==0)
  {
	  //just save last state (saveFreq is replaced by maximum number of iterations in order to avoid intermediate saving)
	  stateSaver1 = new eoCountedStateSaver(maxIter, _state, genSaveFilePath.toStdString(),true);
  }
  else
  {
	  // save every "savefreq" generations
	  stateSaver1 = new eoCountedStateSaver(saveFreq, _state, genSaveFilePath.toStdString(),true);
  }
  _state.storeFunctor(stateSaver1);
  checkpoint.add(*stateSaver1);
 

  ///////////////////
  // Archive
  //////////////////
  // update the archive every generation
  moeoArchiveUpdater < MOEOT > * updater = new moeoArchiveUpdater < MOEOT > (_archive, _pop);
  _state.storeFunctor(updater);
  checkpoint.add(*updater);
  
  // display obj vector in GUI
  EAUpdaterDispObjGUI < MOEOT > * disp_updater = new EAUpdaterDispObjGUI < MOEOT > (_archive);
  _state.storeFunctor(disp_updater);
  checkpoint.add(*disp_updater);
 
  // store the objective vectors contained in the archive every generation
   moeoArchiveObjectiveVectorSavingUpdater < MOEOT > * save_updater = new moeoArchiveObjectiveVectorSavingUpdater < MOEOT > (_archive, archSaveFilePath.toStdString(),true);
  _state.storeFunctor(save_updater);
  checkpoint.add(*save_updater);


  // store the contribution of the non-dominated solutions
  moeoContributionMetric < ObjectiveVector > * contribution = new moeoContributionMetric < ObjectiveVector >;
  moeoBinaryMetricSavingUpdater < MOEOT > * contribution_updater = new moeoBinaryMetricSavingUpdater < MOEOT > (*contribution, _archive, contribSaveFilePath.toStdString());
  _state.storeFunctor(contribution_updater);
  checkpoint.add(*contribution_updater);

  // Stats
  eoFileMonitor *bestStatsFileMonitor = new eoFileMonitor(bestStatsSaveFilePath.toStdString());
  checkpoint.add(*bestStatsFileMonitor);
  _state.storeFunctor(bestStatsFileMonitor);

  eoFileMonitor *secondStatsFileMonitor = new eoFileMonitor(secondStatsSaveFilePath.toStdString());
  checkpoint.add(*secondStatsFileMonitor);
  _state.storeFunctor(secondStatsFileMonitor);

  eoBestFitnessStat<MOEOT> *bestStat = new   eoBestFitnessStat<MOEOT>;
  checkpoint.add(*bestStat);
  _state.storeFunctor(bestStat);
  bestStatsFileMonitor->add(*bestStat);
  // Second moment stats: average and stdev
  eoSecondMomentStats<MOEOT> *secondStat = new eoSecondMomentStats<MOEOT>;
  checkpoint.add(*secondStat);
  _state.storeFunctor(secondStat);
  secondStatsFileMonitor->add(*secondStat);

  
  

  // and that's it for the (control and) output
  return checkpoint;
}

#endif
