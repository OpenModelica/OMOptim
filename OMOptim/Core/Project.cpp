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

void Project::setName(QString name)
{
    _name=name;
    emit projectChanged();
}

/**
* \brief
* Set if project is defined (used for enabling gui)
*/
void Project::setIsDefined(bool isdefined)
{
    _isdefined=isdefined;
    emit projectChanged();
}

bool Project::isSaved()
{
    return _isSaved;
}


void Project::setSaved(bool isSaved)
{
    _isSaved = isSaved;
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
        _moFiles.push_back(moFilePath);

    // load moFile ...
    _modLoader->loadMoFile(modItemsTree(),moFilePath,forceLoad);

    // watch mo file
    if(storePath || wasThere)
        _mofilesWatcher.addPath(moFilePath);

    _modItemsTree->emitDataChanged();


    emit projectChanged();
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

/**
* \brief
* Load Modelica library (calls OpenModelica load library function
* \param storePath yes/no should path be stored in project file
* (as to be reloaded when loading project)
* \param forceLoad yes/no should mo file be reloaded in OMC when already loaded in OMC
*/
bool Project::loadModelicaLibrary(bool storePath, bool forceLoad)
{
    _moomc->loadStandardLibrary();
    QString libPath = _moomc->getFileOfClass("Modelica");
    if(storePath)
        _moFiles.push_back(libPath);

    refreshAllMod();

    emit projectChanged();

    return true;
}

/**
* \brief
* Load a ModModelPlus defined by a filePath. It will be loaded only if refers to an existing model in current workspace.
* \param mmoFilePath full file path of .mmo
*/
void Project::loadModModelPlus(QString mmoFilePath)
{
    Load::loadModModelPlus(this,mmoFilePath);
    storeMmoFilePath(mmoFilePath);

    emit projectChanged();
}

/**
* \brief
* Store mmoFilePath in project file. It will therefore be loaded when reloading project file.
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

/**
*	Reload the mo file of model
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
* \brief
*	Refresh modelica tree in GUI. Do not reload in OpenModelica ! Just reread hierarchy.
*/
void Project::refreshAllMod()
{
    _modItemsTree->clear();
    _modItemsTree->readFromOMCWThread(_modItemsTree->rootElement(),2);
    _modItemsTree->emitDataChanged();
}

/**
* Load a OMOptim plugin
* \param pluginPath full file path of .dll
* \param storePath yes/no should path be stored in project file
* (as to be reloaded when loading project)
* \param forceLoad yes/no should mo file be reloaded in OMC when already loaded in OMC
*/
bool Project::loadPlugin(QString pluginPath, bool storePath, bool forceLoad)
{
    InfoSender::instance()->sendNormal("Loading plugin : "+pluginPath);

    bool tryLoad;

    tryLoad = (forceLoad || !_pluginsLoaded.values().contains(pluginPath));

    if(!tryLoad)
        return false;

    // unload if needed
    if(forceLoad && _pluginsLoaded.values().contains(pluginPath))
        unloadPlugin(pluginPath);

    // first try to load
    QPluginLoader loader(pluginPath);
    QObject *plugin = loader.instance();
    ProblemInterface* pbInter = qobject_cast<ProblemInterface*>(plugin);

    if(pbInter)
    {
        InfoSender::instance()->sendNormal("Loaded plugin successfully : "+pbInter->name());
        this->addProblemInterface(pbInter);

        // added corresponding settings
        MOParameters* pluginParams = pbInter->parameters();
        MOSettings::addParameters(pluginParams,pbInter->name());
    }
    else
    {
        InfoSender::instance()->sendError("Loaded plugin failed : "+pluginPath
                                          +"\n("+loader.errorString()+")");
    }

    bool loadOk = (pbInter!=NULL);

    // add to stored list
    if(loadOk && storePath)
    {
        _pluginsLoaded.insert(pbInter->name(),pluginPath);
        emit projectChanged();
    }
}

/**
* Unload a OMOptim plugin
* \param pluginPath full file path of plugin
*/
bool Project::unloadPlugin(QString pluginPath)
{
    InfoSender::instance()->sendNormal("Unloading plugin : "+pluginPath);

    _pluginsLoaded.remove(_pluginsLoaded.key(pluginPath));

    QPluginLoader loader(pluginPath);
    if(loader.unload())
    {
        emit projectChanged();
    }
}

/**
* Unload all plugins
*/
bool Project::unloadPlugins()
{
    QStringList pluginsPaths(_pluginsLoaded.values());
    bool ok = true;
    for(int i=0;i<pluginsPaths.size();i++)
        ok = unloadPlugin(pluginsPaths.at(i)) && ok;

    emit projectChanged();

    return ok;
}


/**
* \brief
* Return selected ModModel. Return NULL if no ModModel selected.
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
* Find a ModModel within _modItemsTree.
* \arg modelName name of the model looked for
*/
ModModel* Project::findModModel(QString modelName)
{
    ModItem* modModel = _modItemsTree->findInDescendants(modelName);

    if(!modModel || modModel->getClassRestr()!=Modelica::MODEL)
        return NULL;
    else
        return (ModModel*)modModel;
}

/**
* \brief Returns allModModelPlus contained in project
* Returns allModModelPlus contained in project.
*/
QList<ModModelPlus*> Project::allModModelPlus()
{
    return _mapModelPlus.values();
}

void Project::addModModelPlus(ModModelPlus* modModelPlus)
{
    _mapModelPlus.insert(modModelPlus->modModelName(),modModelPlus);
}

ModModelPlus* Project::curModModelPlus()
{
    ModModel* curMM = curModModel();
    if(curMM)
        return modModelPlus(curMM->name());
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

ModModelPlus* Project::modModelPlus(QString modModelName)
{
    ModModelPlus* corrModModelPlus = _mapModelPlus.value(modModelName,NULL);

    if(corrModModelPlus)
        return corrModModelPlus;
    else
    {
        corrModModelPlus = newModModelPlus(modModelName);
        addModModelPlus(corrModModelPlus);
        return corrModModelPlus;
    }
}


ModModelPlus* Project::newModModelPlus(QString modelName)
{
    // Create ModModelFile
    ModModelPlus* newModModelPlus = new ModModelPlus(this,modelName);

    // Add to map
    _mapModelPlus.insert(modelName,newModModelPlus);

    // Store it
    // create folder
    QDir allModPlusdir(modModelPlusFolder());
    if(!allModPlusdir.exists())
    {
        QDir tmpDir(folder());
        tmpDir.mkdir(allModPlusdir.absolutePath());
    }

    // modModelPlus dir
    QDir modPlusdir(allModPlusdir.absolutePath()+QDir::separator()+modelName);
    if(!modPlusdir.exists())
    {
        allModPlusdir.mkdir(modPlusdir.absolutePath());
    }

    // mmo file
    QString newMmoFilePath = modPlusdir.absolutePath() + QDir::separator() + _name + ".mmo";

    // set mmoFilePath in ModModelPlus
    newModModelPlus->setMmoFilePath(newMmoFilePath);

    // save it
    Save::saveModModelPlus(newModModelPlus);

    // store path
    storeMmoFilePath(newMmoFilePath);

    emit projectChanged();

    return newModModelPlus;
}

void Project::setFilePath(QString filePath)
{
    _filePath=filePath;

    //create models folder
    QFileInfo fileInfo(_filePath);
    QString modelsDir = fileInfo.dir().absolutePath()+QDir::separator()+"Models";
    fileInfo.dir().mkdir(modelsDir);

    emit projectChanged();
}

void Project::save(bool saveAllOMCases)
{
    Save::saveProject(this,saveAllOMCases);

    setSaved(true);
    emit projectChanged();
}

void Project::save(Result* result)
{
    // save project but not all omcases
    Save::saveProject(this,false);

    // save result
    Save::saveResult(this,result);

    emit projectChanged();
}

void Project::save(Problem* problem)
{
    // save project but not all omcases
    Save::saveProject(this,false);

    // save problem
    Save::saveProblem(this,problem);

    emit projectChanged();
}

bool Project::load(QString loadPath)
{
    terminateOmsThreads();
    bool configOk = checkConfiguration();
    bool loaded = false;

    if(configOk)
        loaded = Load::loadProject(loadPath,this);

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


QString Project::filePath()
{
    return _filePath;
}

QString Project::folder()
{
    QFileInfo fileInfo(_filePath);
    return fileInfo.absolutePath();
}

QString Project::tempPath()
{
    return folder()+QDir::separator()+"temp";
}

QString Project::modModelPlusFolder()
{
    return folder()+QDir::separator()+"Models";
}

QString Project::problemsFolder()
{

    return folder()+QDir::separator()+"Problems";
}

QString Project::resultsFolder()
{
    return folder()+QDir::separator()+"Results";
}

void Project::addProblemInterface(ProblemInterface* problemInterface)
{
    _problemsInterfaces.addProblemInterface(problemInterface);
    emit interfacesModified();
    emit projectChanged();
}

void Project::removeProblemInterface(QString interfaceName)
{
    if(_problemsInterfaces.removeProblemInterface(interfaceName))
    {
        emit interfacesModified();
        emit projectChanged();
    }
}

void Project::addNewProblem(ProblemInterface* interface, QList<ModModelPlus*> modModelPlusList,QString problemType)
{
    Problem* newProblem = interface->createNewProblem(this,modModelPlusList,problemType);
    HighTools::checkUniqueProblemName(this,newProblem,_problems);

    _problems->addCase(newProblem);

    save(newProblem);

    emit addedProblem(newProblem);
}

void Project::addOMCase(QString filePath)
{
    OMCase* newCase = Load::newOMCase(filePath,this);

    Problem* problem = dynamic_cast<Problem*>(newCase);
    if(problem)
        addProblem(problem);
    else
    {
        Result* result = dynamic_cast<Result*>(newCase);
        if(result)
            addResult(result);
    }
}



void Project::addProblem(Problem *problem)
{
    _problems->addCase(problem);

    //update GUI
    emit sendInfo(Info(ListInfo::ADDEDPROBLEM,problem->name()));
    emit projectChanged();
    emit addedProblem(problem);
}

void Project::addResult(Result *result)
{
    _results->addCase(result);

    emit sendInfo(Info(ListInfo::ADDEDPROBLEM,result->name()));

    //update GUI
    emit projectChanged();
    emit addedResult(result);
}

bool Project::checkConfiguration()
{
    bool ok = true;

    if(!_moomc->isStarted())
        ok = ok && _moomc->startServer();

    return ok;
}


void Project::createTempDir()
{
    QDir tempDir(tempPath());
    if(tempDir.exists())
        LowTools::removeDir(tempPath());
    tempDir.mkdir(tempPath());

}


void Project::launchProblem(Problem* problem)
{
    if(!_problemLaunchMutex.tryLock())
    {
        QString msg = "Another problem is already running. Could not launch a new one.";
        InfoSender::instance()->send(Info(msg));
    }
    else
    {
        //Copy launched problem from selected one
        Problem* launchedProblem;
        launchedProblem = problem->clone();

        // Create temporary directory where calculations are performed
        createTempDir();

        //Create problem thread
        ProblemConfig config;
        MOThreads::ProblemThread* launchThread = new MOThreads::ProblemThread(launchedProblem,config);

        // connect signals
        connect(launchThread,SIGNAL(begun(Problem*)),this,SIGNAL(problemBegun(Problem*)));
        connect(launchThread,SIGNAL(newProgress(float)),this,SIGNAL(newProblemProgress(float)));
        connect(launchThread,SIGNAL(newProgress(float,int,int)),this,SIGNAL(newProblemProgress(float,int,int)));
        connect(launchThread,SIGNAL(finished(Problem*,Result*)),this,SLOT(onProblemFinished(Problem*,Result*)));
        connect(launchThread,SIGNAL(finished(Problem*,Result*)),this,SIGNAL(problemFinished(Problem*,Result*)));

        // store thread-problem
        _launchedThreads.insert(launchedProblem,launchThread);

        // start problem
        launchThread->start();
    }
}

void Project::onProjectChanged()
{
    _isSaved = false;
    emit projectChanged();
}

void Project::onProblemStopAsked(Problem* problem)
{
    //    MOThreads::ProblemThread *thread = _launchedThreads.value(problem,NULL);

    //    if(thread)
    //        thread->onStopAsked();
    problem->stop();
}

void Project::onProblemFinished(Problem* problem,Result* result)
{
    if(result)
    {
        //Results
        if(!result->isSuccess())
        {
            QString msg = "Problem "+ problem->getClassName()+ " has failed";
            InfoSender::instance()->send(Info(msg,ListInfo::ERROR2));
        }
        else
        {
            QString msg = "Problem "+ problem->getClassName()+ " succeeded";
            InfoSender::instance()->send(Info(msg,ListInfo::NORMAL2));

            if(result->name().isEmpty())
                result->setName(result->problem()->name()+" result");

            HighTools::checkUniqueResultName(this,result,_results);

            result->store(QString(resultsFolder()+QDir::separator()+result->name()),tempPath());

            addResult(result);
            save(result);
        }
    }
    _problemLaunchMutex.unlock();
    _launchedThreads.remove(problem);
}

Problem* Project::restoreProblemFromResult(int numResult)
{
    return restoreProblemFromResult(_results->at(numResult));
}

Problem* Project::restoreProblemFromResult(Result* result)
{
    Problem* restoredPb = result->problem()->clone();
    restoredPb->setName(restoredPb->name().replace(" result",""));
    HighTools::checkUniqueProblemName(this,restoredPb,_problems);

    addProblem(restoredPb);
    return restoredPb;
}


void Project::removeResult(Result* result)
{
    int num = results()->items.indexOf(result);
    if(num>-1)
    {
        // result to be removed
        emit beforeRemoveResult(result);

        // remove folder and data
        QString folder = QDir(result->saveFolder()).absolutePath();
        LowTools::removeDir(folder);
        _results->removeRow(num);

        save(false);
    }
}


void Project::removeProblem(Problem* problem)
{
    int num = problems()->items.indexOf(problem);
    if(num>-1)
    {
        // result to be removed
        emit beforeRemoveProblem(problem);

        // remove folder and data
        QString folder = QFileInfo(problem->saveFolder()).absolutePath();
        LowTools::removeDir(folder);
        _problems->removeRow(num);

        save(false);
    }
}


void Project::removeCases(QList<OMCase*> cases)
{
    Problem* problem;
    Result* result;
    for(int i=0;i<cases.size();i++)
    {
        problem = dynamic_cast<Problem*>(cases.at(i));
        if(problem)
            removeProblem(problem);
        else
        {
            result = dynamic_cast<Result*>(cases.at(i));
            if(result)
                removeResult(result);
        }
    }
}

void Project::renameCase(OMCase* curCase,QString newName)
{
    Problem* problem;
    Result* result;
    problem = dynamic_cast<Problem*>(curCase);
    if(problem)
        renameProblem(problem,newName);
    else
    {
        result = dynamic_cast<Result*>(curCase);
        if(result)
            renameResult(result,newName);
    }
}


bool Project::renameProblem(Problem* problem,QString newName)
{
    // test if name already exists
    if(_problems->findItem(newName)>-1)
        return false;

    // change name
    problem->rename(newName,true);
    save(problem);
    return true;
}	

bool Project::renameResult(Result* result,QString newName)
{
    // test if name already exists
    if(_results->findItem(newName)>-1)
        return false;

    // change name
    result->rename(newName,true);
    save(result);
    return true;
}


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


QStringList Project::moFiles()
{
    return _moFiles;
}

QStringList Project::mmoFiles()
{
    return _mmoFiles;
}

QMap<QString,QString> Project::pluginsLoaded()
{
    return _pluginsLoaded;
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
