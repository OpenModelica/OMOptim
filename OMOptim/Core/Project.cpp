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
    setCurModClass(NULL);

    _moomc = new MOomc("OMOptim",true);
    _modLoader = new ModLoader(_moomc);
    _modClassTree = new ModClassTree(_modLoader,_moomc);

    // add interfaces for OneSimulation and Optimization
    addProblemInterface(new OneSimulationInterface());
    addProblemInterface(new OptimizationInterface());

    // create infosender
  //  _infoSender = new InfoSender();
}

Project::~Project()
{
    qDebug("deleting Project");
    terminateOmsThreads();

    _problems->deleteLater();
    _results->deleteLater();


    if(_modClassTree)
        _modClassTree->deleteLater();

    _moomc->deleteLater();
    _modLoader->deleteLater();

    for(int i=0;i<_problemsInterfaces.uniqueInterfaces().size();i++)
        _problemsInterfaces.uniqueInterfaces().at(i)->deleteLater();

    //delete _infoSender;
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


    _modClassTree->clear();
    _mapModelPlus.clear();

    // OMC
    terminateOmsThreads();
    _moomc->clear();


    _problems->reset();
    _results->reset();

    _isdefined=false;
    _filePath.clear();
    _name.clear();
    setCurModClass(NULL);

    _moFiles.clear();
    _mmoFiles.clear();


}

void Project::setName(QString name)
{
    _name=name;
}

/**
* \brief
* Set if project is defined (used for enabling gui)
*/
void Project::setIsDefined(bool isdefined)
{
    _isdefined=isdefined;
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
    // add to mofileloadedlist
    if(storePath && !_moFiles.contains(moFilePath))
        _moFiles.push_back(moFilePath);

    // load moFile ...
    _modLoader->loadMoFile(rootModClass(),moFilePath,_mapModelPlus,forceLoad);

    // and read it add class in ModClassTree
    refreshAllMod();
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
    _modLoader->loadMoFiles(rootModClass(),moFilePaths,_mapModelPlus,forceLoad);

    refreshAllMod();
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
}

/**
*	Reload the mo file of model and refresh entire tree
*/
void Project::reloadModModel(ModModel* modModel)
{
    modModel->reloadInOMC();
    refreshAllMod();
}

/**
* \brief
*	Refresh modelica tree in GUI. Do not reload in OpenModelica ! Just reread hierarchy.
*/
void Project::refreshAllMod()
{

    QMap<QString,ModModelPlus*> strMapModelPlus;

    // Copy map information (using string instead of ModModel*)
    ModModel* curModModel;
    for(int i=0;i<_mapModelPlus.keys().size();i++)
    {
        curModModel = _mapModelPlus.keys().at(i);
        strMapModelPlus.insert(curModModel->name(Modelica::FULL),_mapModelPlus.value(curModModel));
    }

    _modClassTree->clear();
    _modClassTree->readFromOmc(_modClassTree->rootElement(),2);

    // refreshing map
    _mapModelPlus.clear();
    QList<ModModelPlus*> listModPlus = strMapModelPlus.values();
    ModModelPlus* curModModelPlus;
    for(int iP=0;iP<listModPlus.size();iP++)
    {
        curModModelPlus = listModPlus.at(iP);
        curModModel = dynamic_cast<ModModel*>(_modClassTree->findInDescendants(strMapModelPlus.key(curModModelPlus)));
        if(curModModel)
        {
            _mapModelPlus.insert(curModModel,curModModelPlus);
            curModModelPlus->setModModel(curModModel);
        }
        else
        {
            LowTools::removeDir(curModModelPlus->mmoFolder());
            delete curModModelPlus;
        }
    }
}

/**
* \brief
* Load a OMOptim plugin
* \param moFilePath full file path of .dll
* \param storePath yes/no should path be stored in project file
* (as to be reloaded when loading project)
* \param forceLoad yes/no should mo file be reloaded in OMC when already loaded in OMC
*/
bool Project::loadPlugin(QString pluginPath, bool storePath, bool forceLoad)
{

    bool tryLoad;

    tryLoad = (forceLoad || !_pluginsLoaded.values().contains(pluginPath));

    if(!tryLoad)
        return false;

    // first try to load
    QPluginLoader loader(pluginPath);
    QObject *plugin = loader.instance();
    ProblemInterface* pbInter = qobject_cast<ProblemInterface*>(plugin);

    if(pbInter)
    {
        InfoSender::instance()->sendNormal("Loaded plugin successfully : "+pbInter->name());
        this->addProblemInterface(pbInter);
    }
    else
    {
        InfoSender::instance()->sendError("Loaded plugin failed : "+pluginPath
                              +"\n("+loader.errorString()+")");
    }

    bool loadOk = (pbInter!=NULL);

    // add to stored list
    if(loadOk && storePath)
        _pluginsLoaded.insert(pbInter->name(),pluginPath);
}

/**
* \brief
* Return selected ModModel. Return NULL if no ModModel selected.
*/
ModModel* Project::curModModel()
{
    if(_curModClass && (_curModClass->getClassRestr()==Modelica::MODEL))
        return (ModModel*)_curModClass;
    else
        return NULL;
}
/**
* \brief Find a ModModel within _modClassTree
* Find a ModModel within _modClassTree.
* \arg modelName name of the model looked for
*/
ModModel* Project::findModModel(QString modelName)
{
    ModClass* modModel = _modClassTree->findInDescendants(modelName);

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
    ModModel* modModel = modModelPlus->modModel();
    _mapModelPlus.insert(modModel,modModelPlus);
}

ModModelPlus* Project::curModModelPlus()
{
    ModModel* curMM = curModModel();
    if(curMM)
        return modModelPlus(curMM);
    else
        return NULL;
}

void Project::setCurModClass(ModClass* modClass)
{
    if(_curModClass != modClass)
    {
        _curModClass = modClass;
        emit curModClassChanged(_curModClass);

        if(_curModClass && _curModClass->getClassRestr()==Modelica::MODEL)
            emit curModModelChanged((ModModel*)_curModClass);
    }
}

ModModelPlus* Project::modModelPlus(ModModel* model)
{
    ModModelPlus* corrModModelPlus = NULL;
    if(!model)
        return corrModModelPlus;

    corrModModelPlus = _mapModelPlus.value(model,NULL);

    if(corrModModelPlus)
        return corrModModelPlus;
    else
        return newModModelPlus(model);
}

ModModelPlus* Project::newModModelPlus(ModModel* model)
{
    // Create ModModelFile
    ModModelPlus* newModModelPlus = new ModModelPlus(this,model);

    // Add to map
    _mapModelPlus.insert(model,newModModelPlus);

    // Store it
    // create folder
    QDir allModPlusdir(modModelPlusFolder());
    if(!allModPlusdir.exists())
    {
        QDir tmpDir(folder());
        tmpDir.mkdir(allModPlusdir.absolutePath());
    }

    // modModelPlus dir
    QString modelName = model->name();
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

    return newModModelPlus;
}

//bool Project::compileModModel(ModModel* model)
//{
//    ModModelPlus* concernedMMPlus = modModelPlus(model);
//    return compileModModelPlus(concernedMMPlus);
//}

//bool Project::compileModModelPlus(ModModelPlus* modModelPlus)
//{
//    return modModelPlus->compile();
//}

void Project::setFilePath(QString filePath)
{
    _filePath=filePath;

    //create models folder
    QFileInfo fileInfo(_filePath);
    QString modelsDir = fileInfo.dir().absolutePath()+QDir::separator()+"Models";
    fileInfo.dir().mkdir(modelsDir);
}

void Project::save()
{
    Save::saveProject(this);
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
}

void Project::addNewProblem(ProblemInterface* interface, QList<ModModelPlus*> modModelPlusList,QString problemType)
{
    Problem* newProblem = interface->createNewProblem(this,modModelPlusList,problemType);
    HighTools::checkUniqueProblemName(this,newProblem,_problems);

    _problems->addCase(newProblem);
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

    // Saving result into file
    //	Save::saveProblem(problem);

    //update GUI
    emit sendInfo(Info(ListInfo::ADDEDPROBLEM,problem->name()));

    emit projectChanged();
    emit addedProblem(problem);
}

void Project::addResult(Result *result)
{
    _results->addCase(result);

    // Saving result into file
    //Save::saveResult(result_);

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

            save();
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

        save();
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

        save();
    }
}


bool Project::renameProblem(Problem* problem,QString newName)
{
    // test if name already exists
    if(_problems->findItem(newName)>-1)
        return false;

    // test if index is valid
    int i = _problems->items.indexOf(problem);
    if(i<0)
        return false;

    // change name
    problem->rename(newName,true);
    save();
    return true;
}	

bool Project::renameResult(Result* result,QString newName)
{
    // test if name already exists
    if(_results->findItem(newName)>-1)
        return false;

    // test if index is valid
    int i = _problems->items.indexOf(result);
    if(i<0)
        return false;

    // change name
    result->rename(newName,true);
    save();
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

void Project::onModClassSelectionChanged(QList<ModClass*> &classes)
{
    if(classes.size()==1)
    {
        emit curModClassChanged(classes.at(0));

        if(classes.at(0)->getClassRestr()==Modelica::MODEL)
            emit curModModelChanged((ModModel*)classes.at(0));
        else
            emit curModModelChanged(NULL);
    }
    else
    {
        curModClassChanged(NULL);
        curModModelChanged(NULL);
    }
}
