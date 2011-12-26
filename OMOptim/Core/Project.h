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

  @file Project.h
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version

  */
#if !defined(_PROJECT_H)
#define _PROJECT_H

#include <stdio.h>
#include <QtCore/QThread>
#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>
#include <QtXml/QDomDocument>
#include <QtCore/QSettings>

#include "ModModelPlus.h"
#include "MOItem.h"
#include "OMCases.h"
#include "Problems.h"
#include "Results.h"
#include "MOomc.h"
#include "ModPlusCtrl.h"
#include "ModReader.h"
#include "InfoSender.h"
#include "MOVector.h"
#include "SleeperThread.h"
#include "Info.h"
#include "OneSimResult.h"
#include "OneSimulation.h"
#include "Optimization.h"
#include "LowTools.h"
#include "HighTools.h"
#include "Save.h"
#include "Load.h"
#include "MOThreads.h"
#include "EA/EABase.h"
#include "ModClassTree.h"
#include "ProblemInterface.h"

#ifdef USEEI
#include "EITarget.h"
#include "EIReader.h"
#include "EIHEN1Problem.h"
#endif


/*
 * \brief Main class managing problems, results, models,
 * paths information, save/load main functions, threads.
 */
class Project: public MOItem
{
    Q_OBJECT

    //****************************
    // Attributes
    //****************************
public : 

    //Threads management
    QMutex _componentMutex;
    QMutex _connectionMutex;
    QMutex _problemLaunchMutex;
    QMap<Problem*,MOThreads::ProblemThread *> _launchedThreads;

private:
    //Misc
    QString _filePath;
    bool _isdefined;
    ModClass* _curModClass;
    MOomc *_moomc;

    // Models
    QStringList _moFiles;
    QStringList _mmoFiles;

    // Plugins
    QMap<QString,QString> _pluginsLoaded; //loaded plugins <name,filePath>

    Problems* _problems;
    Results* _results;

    ModReader* _modReader;
    ModClassTree* _modClassTree;
    QMap<ModModel*,ModModelPlus*> _mapModelPlus;

    // Problems interfaces
    ProblemInterfaces _problemsInterfaces;


public:
    Project();
    ~Project();
    QString getClassName(){return "Project";};

    void clear();
    QString getFieldName(int iField,int role);
    unsigned getNbFields();

    //****************************
    //Model managment
    //****************************
    void loadMoFile(QString filePath, bool storePath=true, bool forceLoad = true);
    void loadMoFiles(QStringList filePaths, bool storePath=true, bool forceLoad = true);
    bool loadModelicaLibrary(bool storePath=true, bool forceLoad = true);
    void loadModModelPlus(QString mmoFilePath);
    ModModelPlus* modModelPlus(ModModel* model);
    ModModelPlus* newModModelPlus(ModModel* model);
    ModModel* curModModel();
    ModModelPlus* curModModelPlus();
    void setCurModClass(ModClass*);
    ModModel* findModModel(QString name);
    QList<ModModelPlus*> allModModelPlus();
    void addModModelPlus(ModModelPlus*);
    //    bool compileModModel(ModModel*);
    //    bool compileModModelPlus(ModModelPlus*);
    void storeMmoFilePath(QString mmoFilePath);
    void refreshAllMod();
    void reloadModModel(ModModel*);

    //****************************
    //Problem managment
    //****************************
    ProblemInterfaces problemsInterfaces(){return _problemsInterfaces;}
    void addProblemInterface(ProblemInterface* problemInterface);
    void addNewProblem(ProblemInterface* interface, QList<ModModelPlus*> modModelPlusList,QString problemType);
    void addResult(Result *);
    void addProblem(Problem *);
    void addOMCase(QString filePath);
    void launchProblem(Problem*);
    void removeResult(Result*);
    void removeProblem(Problem*);



    //****************************
    // Get/Set functions
    //****************************
    bool isDefined(){return _isdefined;};
    QString modModelPlusFolder();
    QString problemsFolder();
    QString resultsFolder();
    QString filePath();
    QString folder();
    QString tempPath();
    void setName(QString);
    void setFilePath(QString);
    void setSoftware(int);
    void setIsDefined(bool);
    Problem* curLaunchedProblem();
    QStringList moFiles();
    QStringList mmoFiles();
    QMap<QString,QString> pluginsLoaded();

    MOomc* moomc(){return _moomc;};
    Problems* problems(){return _problems;};
    Results* results(){return _results;};
    ModReader* modReader(){return _modReader;};
    ModClassTree* modClassTree(){return _modClassTree;};
    ModClass* rootModClass(){return _modClassTree->rootElement();};
    QMap<ModModel*,ModModelPlus*> mapModelPlus(){return _mapModelPlus;};



    //****************************
    // Save/ Load functions
    //****************************
    void save();
    bool load(QString);

    //****************************
    // Plugins
    //****************************
    bool loadPlugin(QString filePath, bool storePath=true,bool forceLoad=true);

    //****************************
    // Misc
    //****************************
    void terminateOmsThreads();
    void createTempDir();
    bool checkConfiguration();

    //****************************
    // Slots
    //****************************
public slots :


    // problems
    Problem* restoreProblemFromResult(int numSolved);
    Problem* restoreProblemFromResult(Result* result);
    bool renameProblem(Problem*, QString);
    void onProblemFinished(Problem*,Result*);
    void onProblemStopAsked(Problem*);



    // others
    bool renameResult(Result*, QString);
    void onModClassSelectionChanged(QList<ModClass*> &classes);

    //****************************
    // Signals
    //****************************
signals:
    void sendProgress(float);
    void sendProgress(float,int,int);
    void projectAboutToBeReset();
    void projectChanged();

    void addedProblem(Problem*);
    void addedResult(Result*);

    void databasesUpdated();

    void modifiersUpdated();
    void componentsUpdated();
    void connectionsUpdated();
    void beforeRemoveResult(Result*);
    void beforeRemoveProblem(Problem*);


    void problemBegun(Problem*);
    void newProblemProgress(float);
    void newProblemProgress(float,int,int);
    void problemFinished(Problem*,Result*);

    void curModClassChanged(ModClass*);
    void curModModelChanged(ModModel*);

    void modsUpdated();
    void interfacesModified();

};




#endif  //_PROJECT_H
