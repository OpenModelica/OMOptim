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
#include "ModLoader.h"
#include "InfoSender.h"
#include "MOVector.h"
#include "SleeperThread.h"
#include "Info.h"
#include "OneSimResult.h"
#include "OneSimulation.h"
#include "Optimization.h"
#include "LowTools.h"
#include "HighTools.h"
#include "SaveOMOptim.h"
#include "LoadOMOptim.h"
#include "MOThreads.h"
#include "ModItemsTree.h"
#include "ProblemInterface.h"
#include "ProjectBase.h"


/**
 * \brief Main class managing problems, results, models,
 * paths information, save/load main functions, threads.
 */
class Project: public ProjectBase
{
    Q_OBJECT

    //****************************
    // Attributes
    //****************************
public : 

    //Threads management
    QFileSystemWatcher _mofilesWatcher;

private:
    ModItem* _curModItem;
    MOomc *_moomc;

    // Models
    QStringList _moFiles;
    QStringList _mmoFiles;


    ModLoader* _modLoader;
    ModItemsTree* _modItemsTree;
    QMap<QString,ModModelPlus*> _mapModelPlus; //<modmodelName,modmodelplus>



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
    ModModelPlus* modModelPlus(QString modModelPlus);
    ModModelPlus* newModModelPlus(QString modelName);
    ModModel* curModModel();
    ModModelPlus* curModModelPlus();
    void setCurModItem(ModItem*);
    ModModel* findModModel(QString name);
    QList<ModModelPlus*> allModModelPlus();
    void addModModelPlus(ModModelPlus*);


    void storeMmoFilePath(QString mmoFilePath);
    void refreshAllMod();
    void reloadModModel(QString modModelName);




    //****************************
    // Get/Set functions
    //****************************
    QString modModelPlusFolder();
    QStringList moFiles();
    QStringList mmoFiles();

    MOomc* moomc(){return _moomc;};
    ModLoader* modLoader(){return _modLoader;};
    ModItemsTree* modItemsTree(){return _modItemsTree;};
    ModItem* rootModItem(){return _modItemsTree->rootElement();};
    QMap<QString,ModModelPlus*> mapModelPlus(){return _mapModelPlus;};



    //****************************
    // Save/ Load functions
    //****************************
    void save(bool saveAllOMCases);
    void save(Problem*);
    void save(Result*);
    bool load(QString);

    //****************************
    // Misc
    //****************************
    void terminateOmsThreads();
    bool checkConfiguration();

    //****************************
    // Slots
    //****************************
public slots :

    void onModItemSelectionChanged(QList<ModItem*> &classes);
    void onReloadMOFileAsked();

    //****************************
    // Signals
    //****************************
signals:


    void modifiersUpdated();
    void componentsUpdated();
    void connectionsUpdated();

    void curModItemChanged(ModItem*);
    void curModModelChanged(ModModel*);
    void modItemsTreeRefreshed(); /// when want to refresh tree view

    void modsUpdated();


};




#endif  //_PROJECT_H
