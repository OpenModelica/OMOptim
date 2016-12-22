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

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
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
#include <QObject>
#include <QAction>
#endif

#include "ProjectBase.h"

class ModItem;
class MOomc;
class ModLoader;
class ModItemsTree;
class ModModelPlus;
class ModModel;
class ModelPlus;
class ExeModel;
class ModExePlus;

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
    QFileInfoList _moFiles;
    QFileInfoList _mmoFiles;
    bool _useOmc;
//    QList<QFileInfo> _exeFileInfoList;
//    QList<QFileInfo> _inputFileInfoList;
//    QStringList _fileNameListForExec;


    ModLoader* _modLoader;
    ModItemsTree* _modItemsTree;
    QMap<QString,ModelPlus*> _mapModelPlus; //<modmodelName,modmodelplus>

    void setMoFiles(QFileInfoList moFiles);

 public:
    explicit Project(bool useOMC=true);
    ~Project();

    virtual bool launchScript(QFileInfo scriptFile);
    virtual bool launchScript(const QString & text);

    QString getClassName() const {return "Project";};

    void clear();
    QString getFieldName(int iField,int role) const;
    unsigned getNbFields() const;

    bool useOmc() const {return _useOmc;}



    //****************************
    //Model managment
    //****************************
    void loadMoFile(QString filePath, bool storePath=true, bool forceLoad = true);
    void unloadMoFile(QString filePath, bool removePath=true);
    void loadMoFiles(QStringList filePaths, bool storePath=true, bool forceLoad = true);
    void loadExeFiles(QStringList exeModelNames, QStringList exeFilePaths, QStringList inputFilePaths, bool forceLoad= true);
    bool loadModelicaLibrary(bool storePath=true, bool forceLoad = true);
    bool loadExecutableModel(QString name,QFileInfo exeFileInfo, QFileInfo inputFileInfo);
    bool loadExecutableModel(QString name,QFileInfo exeFileInfo, QFileInfo inputFileInfo, ModExePlus* );
    void loadModelPlus(QString mmoFilePath, bool uncompile);
    ModelPlus *modelPlus(QString modelName);
    ModelPlus* newModelPlus(QString modelName);
    ModModel* curModModel();
    ModelPlus *curModelPlus();
    void setCurModItem(ModItem*);
    ModItem *findModItem(QString name);
    QList<ModelPlus*> allModelPlus();
    void addModelPlus(ModelPlus*);
    QList<ModModelPlus *> modModelPlusOfFile(QString moFile);


    void storeMmoFilePath(QString mmoFilePath);
    //void storeExeFilePath(QString exeFilePath);
    void refreshAllMod();
    void reloadModModel(QString modModelName);



    //****************************
    // Get/Set functions
    //****************************
    QString modelPlusFolder();
    QFileInfoList moFiles();
    QFileInfoList mmoFiles();
//    QFileInfoList exeFiles();
//    QFileInfoList inputFiles();
//    QStringList fileNameListForExec();

    MOomc* moomc(){return _moomc;}
    ModLoader* modLoader(){return _modLoader;}
    ModItemsTree* modItemsTree(){return _modItemsTree;}
    ModItem* rootModItem();
    QMap<QString,ModelPlus*> mapModelPlus(){return _mapModelPlus;}



    //****************************
    // Save/ Load functions
    //****************************
    void save(bool saveAllOMCases);
    void exportProjectFolder(QDir externalFolder, bool includeMSL);
    void save(Problem*);
    void save(Result*);
    bool load(QString);

    //****************************
    // Misc
    //****************************
    void terminateOms();
    bool checkConfiguration();

    //****************************
    // Slots
    //****************************


public slots :

    void onModItemSelectionChanged(QList<ModItem*> &classes);
    void onReloadMOFileAsked();
    void onUnloadMOFileAsked();

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
