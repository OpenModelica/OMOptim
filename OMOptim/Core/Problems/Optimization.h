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

    @file Optimization.h
    @brief Comments for file documentation.
    @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
    Company : CEP - ARMINES (France)
    http://www-cep.ensmp.fr/english/
    @version

  */
#if !defined(_Optimization_H)
#define _Optimization_H


#include "Problem.h"
#include "Result.h"

#include "MOVector.h"
#include "OptObjective.h"
#include "OptimResult.h"
#include "ProblemConfig.h"

#include "BlockSubs/BlockSubstitution.h"
#include "BlockSubs/BlockSubstitutions.h"
#include "OptObjectives.h"


class EABase;

class Optimization : public Problem
{
    Q_OBJECT



protected :

    Project* _omProject;
    //Model
    QList<QString> _models;
    QMap<QString,ModPlusCtrls*> _ctrls;


    Variables *_overwritedVariables;
    ScannedVariables *_scannedVariables;
    OptVariables *_optimizedVariables;
    OptObjectives *_objectives;
    BlockSubstitutions *_blockSubstitutions;

    bool _useScan;

    // Algorithm information
    QList<OptimAlgo*> _algos;
    int _iCurAlgo;

public:
    //Optimization(void);
    Optimization(Project*,QStringList models);
    Optimization(const Optimization &);
    Optimization(QDomElement domProblem,Project* project,bool &ok);
    virtual Problem* clone() const;
    virtual ~Optimization(void);

    static QString className(){return "Optimization";}
    virtual QString getClassName(){return Optimization::className();}
    virtual bool hasQuickEndOption(){return true;}

    //Get functions
    Variables* overwritedVariables()const{return _overwritedVariables;}
    ScannedVariables* scannedVariables()const{return _scannedVariables;}
    OptVariables *optimizedVariables()const{return _optimizedVariables;}
    OptObjectives *objectives()const{return _objectives;}
    BlockSubstitutions *blockSubstitutions()const{return _blockSubstitutions;}

    // evaluate function
    virtual MOOptVector* evaluate(QList<ModModelPlus*> models, Variables *overwritedVariables, ScannedVariables*, bool &ok);

    //Models functions
    QStringList models() const;
    virtual bool addModel(QString model,ModPlusCtrls* ctrls=NULL);
    virtual bool addModels(QStringList models);
    virtual bool removeModel(QString model);


    //overwrited functions (also virtual since other problems might inherit Optimization)
    virtual bool checkBeforeComp(QString & error);
    virtual Result* launch(ProblemConfig config);
    virtual void store(QString destFolder, QString tempDir);
    virtual QDomElement toXmlData(QDomDocument & doc);

    //specific functions
    void createSubExecs(QList<QList<ModModelPlus*> > & subModels, QList<BlockSubstitutions*> & subBlocks);

    //algo functions
    int getiCurAlgo();
    OptimAlgo* getCurAlgo() const;
    QStringList getAlgoNames();
    void setiCurAlgo(int);

    // controlers
    ModPlusCtrl* ctrl(QString model) const;
    ModPlusCtrls* ctrls(QString model) const;
    QMap<QString, ModPlusCtrls *> ctrls() const;
    ModPlusCtrl::Type  ctrlType(QString model) const;
    void setCtrlType(QString model,ModPlusCtrl::Type);

    int nbScans();

    // recompute points
    virtual void recomputePoints(OptimResult*, QList<int>,bool forceRecompute = false);


public slots :
    void onQuickEndAsked();

signals :
    void addedModel(QString);
    void removedModel(QString);

};


#endif
