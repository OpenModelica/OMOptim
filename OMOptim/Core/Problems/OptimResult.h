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

    @file OptimResult.h
    @brief Comments for file documentation.
    @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
    Company : CEP - ARMINES (France)
    http://www-cep.ensmp.fr/english/
    @version

  */
#if !defined(_OPTIMRESULT_H)
#define _OPTIMRESULT_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtXml/QDomDocument>
#endif

#include "Result.h"
#include "MOOptVector.h"
#include "Variable.h"
#include "OptObjective.h"
#include "OneSimulation.h"
#include "OneSimResult.h"
//#include "BlockSubs/BlockSubstitutions.h"
#include "CSV.h"
#include "ProblemConfig.h"

#include "EA/OptimAlgo.h"
#include "Project.h"


class Save;
class Optimization;


class OptimResult : public Result
{
    Q_OBJECT
public:

    OptimResult(Project *project);

    OptimResult(Project*,const Optimization &);

    OptimResult(Project*,const QDomElement & domResult,const Optimization & problem,QDir resultDir,bool &ok);
    OptimResult(const OptimResult &_res);
    virtual ~OptimResult(void);
    static QString className(){return "OptimResult";}
    virtual QString getClassName() const {return OptimResult::className();}

    virtual QDomElement toXmlData(QDomDocument &);

//    void updateRecomputedPointsFromFolder();
    void updateOptimValuesFromFrontFile(QString fileName);



    QString buildOptVarsFrontCSV(QString separator="\t");
    QString buildAllVarsFrontCSV(QString separator="\t");
    QString buildAllVarsFrontCSV( QList<int> points,QString separator="\t");

    QString buildVarsFrontCSV(MOOptVector* variables, QList<int> points,QString separator="\t");


    void exportFrontCSV(QString FileName, bool allVars);

    void setCurScan(int);
    int curScan();


    int nbPoints();
    void setCurPoint(int);
    int curPoint();

signals:
    void curPointChanged();
    void curScanChanged(int );
    void finishedRecomputingPoints(QList<int> );


public :
    MOOptVector *optVariablesResults(){return _optVariablesResults;}
    MOOptVector *optObjectivesResults(){return _optObjectivesResults;}
    MOOptVector *recomputedVariables(){return _recomputedVariables;}

    QList<int> recomputedPoints(){return _recomputedPoints;}
    void recomputePoints(QList<int> iPoints, bool forceRecompute);

    // index for each point of subBlocks included (in subBlocks)
  //  QList<int> _iSubModels;
    // different model configurations (using different BlockSubs)
  //  QList<BlockSubstitutions*> _subBlocks;

    //save informations
    QString _optVarsFrontFileName;
    QString _allVarsFrontFileName;

    Project* omProject(){return dynamic_cast<Project*>(_project);}




protected:

    void loadOptimValuesFromFrontFile(int version);
    void loadOptimValuesFromFrontFilev0();
    void loadOptimValuesFromFrontFilev1();

    int _curPoint;
    int _curScan;

    Project* _omProject;
    MOOptVector *_optVariablesResults;
    MOOptVector *_optObjectivesResults;
    MOOptVector *_recomputedVariables;

    QList<int> _recomputedPoints;

    OptimAlgo *_algo;


    //Model

    QStringList _models;

};





#endif
