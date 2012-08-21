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
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)

  @file Project.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version

  */
#include "Project.h"
#include "onesimulationinterface.h"
#include "optimizationinterface.h"
#include "Save.h"

#include "ModModelPlus.h"
#include "MOItem.h"
#include "OMCases.h"
#include "Problems.h"
#include "Results.h"
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
#include "ProblemInterfaces.h"
#include "ModExePlus.h"


Project::Project()
{
    _isdefined = false;
    //    _curProblem = -1;


    _problems = new Problems("Problems");
    _results = new Results("Results");
    //    _curLaunchedProblem = NULL;
    setCurModItem(NULL);

    _moomc = new MOomc("OMOptim",true);
    _modLoader = new ModLoader(_moomc);
    _modItemsTree = new ModItemsTree(_modLoader,_moomc);

    // add interfaces for OneSimulation and Optimization
    addProblemInterface(new OneSimulationInterface());
    addProblemInterface(new OptimizationInterface());

    emit projectChanged();
}

Project::~Project()
{
    qDebug("deleting Project");

    terminateOmsThreads();

    _moomc->stopServer();

    _problems->clear();
    _results->clear();


    if(_modItemsTree)
        delete _modItemsTree;

    delete _moomc;
    delete _modLoader;

    for(int i=0;i<_problemsInterfaces.uniqueInterfaces().size();i++)
    {
        delete _problemsInterfaces.uniqueInterfaces().at(i);
    }
    _problemsInterfaces.clear();

}

QString Project::getFieldName(int iField, int role)
{
    return "name";
}

unsigned Project::getNbFields()
{
    return 1;
}

/**
* \brief
* Clear project (Modelica hierarchy, _problems, Solved _problems, _moFiles...)
*/
void Project::clear()
{
    // delete GUI tabs...
    emit projectAboutToBeReset();


    _modItemsTree->clear();
    _mapModelPlus.clear();

    // OMC
    terminateOmsThreads();
    _moomc->clear();


    _problems->clear();
    _results->clear();

    _isdefined=false;
    _filePath.clear();
    _name.clear();
    setCurModItem(NULL);

    _moFiles.clear();
    _mofilesWatcher.removePaths(_mofilesWatcher.files());
    _mmoFiles.clear();

    unloadPlugins();

    emit projectChanged();
}


/**
* \brief
* Load a modelica file
* \param moFilePath full file path of .mo
* \param storePath yes/no should path be stored in project file
* (as to be reloaded when loading project)
* \param forceLoad yes/no should mo file be reloaded in OMC when already loaded in OMC
*/
void Project::loadMoFile(QString moFilePath, bool storePath, bool forceLoad)
{
    // unwatch mo file (avoid recursive call)
    bool wasThere = _mofilesWatcher.files().contains(moFilePath);
    _mofilesWatcher.removePath(moFilePath);

    // add to mofileloadedlist
    if(storePath && !_moFiles.contains(moFilePath))
        _moFiles.push_back(QFileInfo(moFilePath));

    // load moFile ...
    QString msg;
    bool loadOk = _modLoader->loadMoFile(modItemsTree(),moFilePath,msg,forceLoad);

    // watch mo file
    if(storePath || wasThere)
        _mofilesWatcher.addPath(moFilePath);

    if(loadOk)
    {
        _modItemsTree->emitDataChanged();
        emit projectChanged();
    }
}

/**
* \brief Load several moFiles
* \param moFilePaths list of full file paths of .mo
* \param storePath yes/no should paths be stored in project file
* (as to be reloaded when loading project)
* \param forceLoad yes/no should mo file be reloaded in OMC when already loaded in OMC
*/
void Project::loadMoFiles(QStringList moFilePaths, bool storePath, bool forceLoad)
{
    QString moFilePath;
    for(int i=0;i<moFilePaths.size();i++)
    {
        moFilePath = moFilePaths.at(i);
        if(storePath && !_moFiles.contains(moFilePath))
            _moFiles.push_back(moFilePath);
    }

    // load _moFiles and read them
    _modLoader->loadMoFiles(modItemsTree(),moFilePaths,forceLoad);

    // watch mo files
    _mofilesWatcher.addPaths(moFilePaths);

    _modItemsTree->emitDataChanged();

    emit projectChanged();
}


void Project::loadExeFiles(QStringList exeModelNames, QStringList exeFilePaths, QStringList inputFilePaths, bool forceLoad)
{
    for(int i = 0; i<exeFilePaths.size(); i++)
    {
        //    QString name = QFileInfo(exeFilePaths.at(i)).dir().dirName();
        loadExecutableModel(exeModelNames.at(i),QFileInfo(exeFilePaths.at(i)), QFileInfo(inputFilePaths.at(i)));
    }

}


/**
* \brief Unload a modelica file
* \param moFilePath full file path of .mo
* \param storePath yes/no should path be removed from project
*/
void Project::unloadMoFile(QString moFilePath, bool removePath)
{
    // unwatch mo file (avoid recursive call)
    bool wasThere = _mofilesWatcher.files().contains(moFilePath);
    _mofilesWatcher.removePath(moFilePath);


    // remove from mofileloadedlist
    if(removePath)
        _moFiles.removeAll(moFilePath);

    // unload moFile ...
    _modLoader->unloadMoFile(modItemsTree(),moFilePath);

    _modItemsTree->emitDataChanged();


    emit projectChanged();
}

/**
* \brief Load Modelica library (calls OpenModelica load library function
* \param storePath yes/no should path be stored in project file
* (as to be reloaded when loading project)
* \param forceLoad yes/no should mo file be reloaded in OMC when already loaded in OMC
*/
bool Project::loadModelicaLibrary(bool storePath, bool forceLoad)
{
    _moomc->loadStandardLibrary();
    QFileInfo libPath = _moomc->getFileOfClass("Modelica");
    if(storePath)
        _moFiles.push_back(libPath);

    refreshAllMod();

    emit projectChanged();

    return true;
}

bool Project::loadExecutableModel(QString name,QFileInfo exeFileInfo, QFileInfo inputFileInfo)
{
    QString exeFile = exeFileInfo.absoluteFilePath();
    QString inputFile = inputFileInfo.absoluteFilePath();
    //      QString modelFolderName = modModelPlusFolder()+ QDir::separator() + name;

    //    if(!QDir(modelFolderName).exists())
    //        QDir(modelFolderName).mkpath(modelFolderName);
    //    bool copyExeFileOk = QFile::copy(exeFileInfo.absoluteFilePath(), exeFile );
    //    bool copyInputFileOk = QFile::copy(inputFileInfo.absoluteFilePath(), inputFile);

    _modItemsTree->addExeModel(_modItemsTree->rootElement(),name, inputFile, exeFile);
    _modItemsTree->emitDataChanged();

    // add modelplus
    addModelPlus(newModelPlus(name));

    // save project (and thus mmo)
    save(false);
    //Store the model
    //    QDir modPlusdir(modModelPlusFolder()+QDir::separator()+name);
    //    LowTools::mkdir(modPlusdir.absolutePath(),true);
    //    QFile::copy(exeFileInfo.absoluteFilePath(), modPlusdir.absoluteFilePath(exeFileInfo.fileName()) );
    //    QFile::copy(inputFileInfo.absoluteFilePath(), modPlusdir.absoluteFilePath(inputFileInfo.fileName()) );

    return true;
}

/**
* \brief load a ModModelPlus defined by a filePath. It will be loaded only if refers to an existing model in current workspace.
* \param mmoFilePath full file path of .mmo
*/
void Project::loadModelPlus(QString mmoFilePath)
{
    LoadOMOptim::loadModelPlus(this,mmoFilePath);
    storeMmoFilePath(mmoFilePath);

    emit projectChanged();
}

/**
* \brief store mmoFilePath in project file. It will therefore be loaded when reloading project file.
*/
void Project::storeMmoFilePath(QString mmoFilePath)
{
    QString path = QFileInfo(mmoFilePath).absoluteFilePath();

    if(!_mmoFiles.contains(path))
    {
        _mmoFiles.push_back(path);
    }

    emit projectChanged();
}

//void Project::storeExeFilePath(QString exeFilePath)
//{
//    //    QString path = QFileInfo(exeFilePath).absoluteFilePath();

//    if(!_exeFileInfoList.contains(QFileInfo(exeFilePath)))
//    {
//        _exeFileInfoList.push_back(exeFilePath);
//    }

//    emit projectChanged();
//}
/**
*    \brief Reload the mo file of model
*/
void Project::reloadModModel(QString modModelName)
{
    ModModel* model = dynamic_cast<ModModel*>(modItemsTree()->findItem(modModelName));
    if(model)
    {
        QString moFile = model->filePath();
        this->loadMoFile(moFile,true,true);
    }

    emit projectChanged();
}


/**
* \brief Refresh modelica tree in GUI. Do not reload in OpenModelica ! Just reread hierarchy.
*/
void Project::refreshAllMod()
{
    _modItemsTree->clear();
    _modItemsTree->readFromOMCWThread(_modItemsTree->rootElement(),2);
    _modItemsTree->emitDataChanged();
}

/**
* \brief Return selected ModModel. Return NULL if no ModModel selected.
*/
ModModel* Project::curModModel()
{
    if(_curModItem && (_curModItem->getClassRestr()==Modelica::MODEL))
        return (ModModel*)_curModItem;
    else
        return NULL;
}
/**
* \brief Find a ModModel within _modItemsTree
* \arg modelName name of the model looked for
*/
//ModModel* Project::findModModel(QString modelName)
//{
//    ModItem* modModel = _modItemsTree->findInDescendants(modelName);

//    if(!modModel || modModel->getClassRestr()!=Modelica::MODEL)
//        return NULL;
//    else
//        return (ModModel*)modModel;
//}

ModItem* Project::findModModel(QString modelName)
{
    ModItem* modModel = _modItemsTree->findInDescendants(modelName);

    if(!modModel)
        return NULL;
    else
        return modModel;
}
/**
* \brief Returns all ModModelPlus contained in project
*/
QList<ModelPlus*> Project::allModelPlus()
{
    return _mapModelPlus.values();
}

/**
* \brief Adds modModelPlus in project
*/
void Project::addModelPlus(ModelPlus* modelPlus)
{
    if(modelPlus)
        _mapModelPlus.insert(modelPlus->modelName(),modelPlus);
}

/**
  * @brief Returns selected modModelPlus
  */
ModelPlus* Project::curModelPlus()
{
    ModModel* curMM = curModModel();
    if(curMM)
        return modelPlus(curMM->name());
    else
        return NULL;
}

void Project::setCurModItem(ModItem* modClass)
{
    if(_curModItem != modClass)
    {
        _curModItem = modClass;
        emit curModItemChanged(_curModItem);

        if(_curModItem && _curModItem->getClassRestr()==Modelica::MODEL)
            emit curModModelChanged((ModModel*)_curModItem);
    }
}

/**
  * @brief Returns modModelPlus of modModelName.
  * If it does not exist, this function creates a new ModModelPlus, adds it to the project,
  * and returns it.
  */
ModelPlus* Project::modelPlus(QString modelName)
{
    ModelPlus* corrModelPlus = _mapModelPlus.value(modelName,NULL);

    if(corrModelPlus)
        return corrModelPlus;
    else
    {
        corrModelPlus = newModelPlus(modelName);
        if(corrModelPlus)
            addModelPlus(corrModelPlus);
        return corrModelPlus;
    }
}

/**
  * @brief Creates a new modelPlus for a model.
  * New modModelPlus is automatically added to project.
  */
ModelPlus* Project::newModelPlus(QString modelName)
{
    // Create ModModelFile or ExeModelFile
    ModItem* item = _modItemsTree->findItem(modelName);

    ModelPlus* modelPlus=NULL;
    ModModel* modModel = dynamic_cast<ModModel*> (item);
    ExeModel* exeModel = dynamic_cast<ExeModel*> (item);

    if(modModel)
        modelPlus = new ModModelPlus(this,modelName);

    if(exeModel)
        modelPlus = new ModExePlus(this, modelName);

    if(!modelPlus)
        return NULL;

    // Add to map
    _mapModelPlus.insert(modelName,modelPlus);

    // Store it
    // create folder
    QDir allModPlusdir(modelPlusFolder());
    LowTools::mkdir(allModPlusdir.absolutePath(),false);
    QDir modPlusdir(allModPlusdir.absolutePath()+QDir::separator()+modelName);
    LowTools::mkdir(modPlusdir.absolutePath(),true);

    // update modelplus with its path
    QString newMmoFilePath = modPlusdir.absolutePath() + QDir::separator() + modelName + ".mmo";
    storeMmoFilePath(newMmoFilePath);

    // set mmoFilePath in ModelPlus
    modelPlus->setMmoFilePath(newMmoFilePath);

    // save it
    SaveOMOptim::saveModelPlus(modelPlus);

    emit projectChanged();
    return modelPlus;
}


/** @brief Save project.
  * @param saveAllOMCases Should we save all problems and results.
  */
void Project::save(bool saveAllOMCases)
{
    SaveOMOptim::saveProject(this,saveAllOMCases);

    setSaved(true);
    emit projectChanged();
}

/** @brief Save project and result given as parameter.
  */
void Project::save(Result* result)
{
    // save project but not all omcases
    SaveOMOptim::saveProject(this,false);

    // save result
    Save::saveResult(this,result);

    emit projectChanged();
}

/** @brief Save project and problem given as parameter.
  */
void Project::save(Problem* problem)
{
    // save project but not all omcases
    SaveOMOptim::saveProject(this,false);

    // save problem
    Save::saveProblem(this,problem);

    emit projectChanged();
}

/** @brief Load project from file given as parameter.
  */
bool Project::load(QString loadPath)
{
    terminateOmsThreads();
    bool configOk = checkConfiguration();
    bool loaded = false;

    if(configOk)
        loaded = LoadOMOptim::loadProject(loadPath,this);

    if (loaded)
    {
        emit InfoSender::instance()->send( Info(ListInfo::PROJECTLOADSUCCESSFULL,filePath()));
        emit projectChanged();
    }
    else
    {
        emit InfoSender::instance()->send( Info(ListInfo::PROJECTLOADFAILED,filePath()));
        clear();
        emit projectChanged();
    }
    setSaved(true);
    emit projectChanged();
    return loaded;
}


/** @brief Return folder containting modelPlus saving files
  */
QString Project::modelPlusFolder()
{
    return folder().absoluteFilePath("Models");
}


/**
  * @brief Check configuration. e.g. : checks omc is started.
  */
bool Project::checkConfiguration()
{
    bool ok = true;

    if(!_moomc->isStarted())
        ok = ok && _moomc->startServer();

    return ok;
}

/**
  * @brief All threads using OMC are terminated.
  */
void Project::terminateOmsThreads()
{
    if(_moomc!=NULL)
    {
        // terminating current threads using omc
        for(int i=0;i<_moomc->getThreads().size();i++)
        {
            QString msg ="Stopping "+_moomc->getThreadsNames().at(i);
            InfoSender::instance()->send(Info(msg,ListInfo::NORMAL2));
            _moomc->getThreads().at(i)->terminate();
        }
    }
}


QFileInfoList Project::moFiles()
{
    return _moFiles;
}

QFileInfoList Project::mmoFiles()
{
    return _mmoFiles;
}

//QFileInfoList Project::exeFiles()
//{
//    return _exeFileInfoList;
//}

//QFileInfoList Project::inputFiles()
//{
//    return _inputFileInfoList;
//}

//QStringList Project::fileNameListForExec()
//{
//    return _fileNameListForExec;
//}

ModItem *Project::rootModItem()
{
    return _modItemsTree->rootElement();
}


void Project::onModItemSelectionChanged(QList<ModItem*> &classes)
{
    if(classes.size()==1)
    {
        emit curModItemChanged(classes.at(0));

        if(classes.at(0)->getClassRestr()==Modelica::MODEL)
            emit curModModelChanged((ModModel*)classes.at(0));
        else
            emit curModModelChanged(NULL);
    }
    else
    {
        curModItemChanged(NULL);
        curModModelChanged(NULL);
    }
}

void Project::onReloadMOFileAsked()
{
    QAction* button = dynamic_cast<QAction*>(sender());
    if(button)
    {
        QString moFile = button->data().toString();
        this->loadMoFile(moFile,true,true);
        _modItemsTree->readFromOMCWThread(_modItemsTree->rootElement(),2);
    }
}

void Project::onUnloadMOFileAsked()
{
    QAction* button = dynamic_cast<QAction*>(sender());
    if(button)
    {
        QString moFile = button->data().toString();
        this->unloadMoFile(moFile,true);
        _modItemsTree->readFromOMCWThread(_modItemsTree->rootElement(),2);
    }
}
