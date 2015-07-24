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

#ifndef EAStdCheckPoint_H
#define EAStdCheckPoint_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtCore/QObject>
#endif

#include <limits.h>
#include <stdlib.h>
#include <sstream>
#include <eoContinue.h>
#include <eoEvalFuncCounter.h>
#include <utils/checkpointing>
#include <utils/selectors.h>
#include <utils/eoParser.h>
#include <utils/eoState.h>

//#include "continuator/moCheckpoint.h"

#include <metric/moeoContributionMetric.h>
#include <metric/moeoEntropyMetric.h>
#include <utils/moeoArchiveUpdater.h>
#include <utils/moeoArchiveObjectiveVectorSavingUpdater.h>
#include <utils/moeoBinaryMetricSavingUpdater.h>


#include "Project.h"
#include "EAUpdaterDispObjGUI.h"
#include "MOParameter.h"
#include "MyEoGnuplot1DMonitor.h"
//#include "myEOArchiveUpdater.h"

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
                                               Project* _project, MOParameters *_parameters,QString tempDir, int nbObj)
{
    eoCheckPoint < MOEOT > & checkpoint = _state.storeFunctor(new eoCheckPoint < MOEOT > (_continue));
    /* the objective vector type */
    typedef typename MOEOT::ObjectiveVector ObjectiveVector;

    QString genSaveFilePath = tempDir+QDir::separator() + "iteration";
    QString archSaveFilePath = tempDir+QDir::separator() + "archive";
    QString contribSaveFilePath = tempDir+QDir::separator() + "contribution";
    QString secondStatsSaveFilePath = tempDir+QDir::separator() + "secondStats";
    QString bestStatsSaveFilePath = tempDir+QDir::separator() + "bestStats";
    QString plotFile = tempDir+QDir::separator() + "plotData";


    unsigned int saveFreq = _parameters->value("SaveFrequency",50).toInt();
    unsigned int maxIter = _parameters->value("MaxIterations",50).toInt();



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
    moeoArchiveUpdater< MOEOT > * updater = new moeoArchiveUpdater < MOEOT > (_archive, _pop);
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
    moeoContributionMetric < ObjectiveVector >  *contribution = new moeoContributionMetric < ObjectiveVector >;
    moeoBinaryMetricSavingUpdater < MOEOT > * contribution_updater = new moeoBinaryMetricSavingUpdater < MOEOT > (*contribution, _archive, contribSaveFilePath.toStdString());
    _state.storeFunctor(contribution_updater);
    checkpoint.add(*contribution_updater);

    // Stats
    eoBestFitnessStat<MOEOT> *bestStat = new   eoBestFitnessStat<MOEOT>;
    checkpoint.add(*bestStat);
    _state.storeFunctor(bestStat);

    eoSecondMomentStats<MOEOT> *secondStat = new eoSecondMomentStats<MOEOT>;
    checkpoint.add(*secondStat);
    _state.storeFunctor(secondStat);

    // Monitors
    bool useGnuplot = MOSettings::value("useGnuplot",false).toBool();
    if(useGnuplot /*&& nbObj>1*/)
    {
        MyEoGnuplot1DMonitor *gnuplotMonitor = new MyEoGnuplot1DMonitor(QDir(tempDir),nbObj,false);
        checkpoint.add(*gnuplotMonitor);
        _state.storeFunctor(gnuplotMonitor);
        gnuplotMonitor->add(*evalCounter);
        gnuplotMonitor->add(*bestStat);
    }

    eoFileMonitor *bestStatsFileMonitor = new eoFileMonitor(bestStatsSaveFilePath.toStdString());
    checkpoint.add(*bestStatsFileMonitor);
    _state.storeFunctor(bestStatsFileMonitor);
    bestStatsFileMonitor->add(*bestStat);

    eoFileMonitor *secondStatsFileMonitor = new eoFileMonitor(secondStatsSaveFilePath.toStdString());
    checkpoint.add(*secondStatsFileMonitor);
    _state.storeFunctor(secondStatsFileMonitor);
    secondStatsFileMonitor->add(*secondStat);



    // and that's it for the (control and) output
    return checkpoint;
}


///**
// * This functions allows to build an moCheckPoint for multi-objective optimization from the parser (partly taken from make_checkpoint_pareto.h)
// * @param _parser the parser
// * @param _state to store allocated objects
// * @param _eval the funtions evaluator
// * @param _continue the stopping crietria
// * @param _sol the solution
// * @param _archive the archive of non-dominated solutions
// */
//template < class MOEOT >
//moCheckpoint < MOEOT > & createEAStdCheckPoint(eoParser & _parser, eoState & _state, eoEvalFuncCounter < MOEOT > & _eval,
//                                                                                         eoContinue < MOEOT > & _continue, MOEOT & _sol, moeoArchive < MOEOT > & _archive,
//                                                       Project* _project, MOParameters *_parameters,QString tempDir)
//{
//  moCheckpoint < MOEOT > & checkpoint = _state.storeFunctor(new moCheckpoint < MOEOT > (_continue));
//  /* the objective vector type */
//  typedef typename MOEOT::ObjectiveVector ObjectiveVector;


//  QString saveFolder = tempDir;
//  QString genSaveFilePath = tempDir+QDir::separator() + "iteration";
//  QString archSaveFilePath = tempDir+QDir::separator() + "archive";
//  QString contribSaveFilePath = tempDir+QDir::separator() + "contribution";
//  QString secondStatsSaveFilePath = tempDir+QDir::separator() + "secondStats";
//  QString bestStatsSaveFilePath = tempDir+QDir::separator() + "bestStats";


//    unsigned int saveFreq = _parameters->value("SaveFrequency",50).toInt();
//    unsigned int maxIter = _parameters->value("MaxIterations",50).toInt();



//  eoValueParam<unsigned int> *evalCounter = new eoValueParam<unsigned int>(0, "Gen.");
//  // Create an incrementor (sub-class of eoUpdater).
//  eoIncrementor<unsigned int> & increment = _state.storeFunctor( new eoIncrementor<unsigned int>(evalCounter->value()) );
//  // Add it to the checkpoint
//  checkpoint.add(increment);


//  //////////////////////////////////
//  // State saver
//  //////////////////////////////
//  eoCountedStateSaver *stateSaver1;
//  if(saveFreq==0)
//  {
//          //just save last state (saveFreq is replaced by maximum number of iterations in order to avoid intermediate saving)
//          stateSaver1 = new eoCountedStateSaver(maxIter, _state, genSaveFilePath.toStdString(),true);
//  }
//  else
//  {
//          // save every "savefreq" generations
//          stateSaver1 = new eoCountedStateSaver(saveFreq, _state, genSaveFilePath.toStdString(),true);
//  }
//  _state.storeFunctor(stateSaver1);
//  checkpoint.add(*stateSaver1);


//  ///////////////////
//  // Archive
//  //////////////////
//  // update the archive every generation

//  myEOArchiveUpdater< MOEOT > * updater = new myEOArchiveUpdater < MOEOT > (_archive, _sol);
//  _state.storeFunctor(updater);
//  checkpoint.add(*updater);

//  // display obj vector in GUI
//  EAUpdaterDispObjGUI < MOEOT > * disp_updater = new EAUpdaterDispObjGUI < MOEOT > (_archive);
//  _state.storeFunctor(disp_updater);
//  checkpoint.add(*disp_updater);

//  // store the objective vectors contained in the archive every generation
//   moeoArchiveObjectiveVectorSavingUpdater < MOEOT > * save_updater = new moeoArchiveObjectiveVectorSavingUpdater < MOEOT > (_archive, archSaveFilePath.toStdString(),true);
//  _state.storeFunctor(save_updater);
//  checkpoint.add(*save_updater);


//  // store the contribution of the non-dominated solutions
//  moeoContributionMetric < ObjectiveVector > * contribution = new moeoContributionMetric < ObjectiveVector >;
//  moeoBinaryMetricSavingUpdater < MOEOT > * contribution_updater = new moeoBinaryMetricSavingUpdater < MOEOT > (*contribution, _archive, contribSaveFilePath.toStdString());
//  _state.storeFunctor(contribution_updater);
//  checkpoint.add(*contribution_updater);

//  // Stats
//  eoFileMonitor *bestStatsFileMonitor = new eoFileMonitor(bestStatsSaveFilePath.toStdString());
//  checkpoint.add(*bestStatsFileMonitor);
//  _state.storeFunctor(bestStatsFileMonitor);

//  eoFileMonitor *secondStatsFileMonitor = new eoFileMonitor(secondStatsSaveFilePath.toStdString());
//  checkpoint.add(*secondStatsFileMonitor);
//  _state.storeFunctor(secondStatsFileMonitor);

//  eoBestFitnessStat<MOEOT> *bestStat = new   eoBestFitnessStat<MOEOT>;
//  checkpoint.add(*bestStat);
//  _state.storeFunctor(bestStat);
//  bestStatsFileMonitor->add(*bestStat);
//  // Second moment stats: average and stdev
//  eoSecondMomentStats<MOEOT> *secondStat = new eoSecondMomentStats<MOEOT>;
//  checkpoint.add(*secondStat);
//  _state.storeFunctor(secondStat);
//  secondStatsFileMonitor->add(*secondStat);


//  // and that's it for the (control and) output
//  return checkpoint;
//}

#endif
