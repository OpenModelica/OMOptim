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

#include <QApplication>
#include <QAction>

#include "ProjectBase.h"
#include "MOSettings.h"
#include "scriptparser.h"


#include "OMCases.h"
#include "Problems.h"
#include "Results.h"
#include "InfoSender.h"
#include "MOVector.h"
#include "SleeperThread.h"
#include "Info.h"
#include "LowTools.h"
#include "HighTools.h"
#include "Save.h"
#include "Load.h"
#include "MOThreads.h"
#include "ProblemInterface.h"
#include "ProblemInterfaces.h"
#include "MOItem.h"
#include "MOThreads.h"
#include "Problem.h"
#include "Problems.h"
#include "Results.h"

ProjectBase::ProjectBase()
{
    _isdefined = false;
    //    _curProblem = -1;


    _problems = new Problems("Problems");
    _results = new Results("Results");
    _scriptParser = NULL;
    emit projectChanged();
}



ProjectBase::~ProjectBase()
{
    terminateProblemsThreads();

    delete _problems;
    delete _results;

    for(int i=0;i<_problemsInterfaces.uniqueInterfaces().size();i++)
    {
        delete _problemsInterfaces.uniqueInterfaces().at(i);
    }
    _problemsInterfaces.clear();

    if(_scriptParser)
        delete _scriptParser;

}

QString ProjectBase::getFieldName(int iField, int role) const
{
    return "name";
}

unsigned ProjectBase::getNbFields() const
{
    return 1;
}

/**
* \brief
* Clear ProjectBase (Modelica hierarchy, _problems, Solved _problems, _moFiles...)
*/
void ProjectBase::clear()
{
    // delete GUI tabs...
    emit projectAboutToBeReset();

    terminateProblemsThreads();

    _problems->clear();
    _results->clear();

    _isdefined=false;
    _filePath.clear();
    _name.clear();

    _problemLaunchMutex.unlock();

    unloadPlugins();

    emit projectChanged();
}

void ProjectBase::setName(QString name)
{
    _name=name;
    emit projectChanged();
}

/**
* \brief
* Set if project is defined (used for enabling gui)
*/
void ProjectBase::setIsDefined(bool isdefined)
{
    _isdefined=isdefined;
    emit projectChanged();
}

bool ProjectBase::isSaved()
{
    return _isSaved;
}


void ProjectBase::setSaved(bool isSaved)
{
    _isSaved = isSaved;
}

void ProjectBase::setScriptParser(ScriptParser* parser)
{
    if(_scriptParser && parser!=_scriptParser)
        delete _scriptParser;

    _scriptParser = parser;
}

/**
* Load a OMOptim plugin
* \param pluginPath full file path of .dll
* \param storePath yes/no should path be stored in project file
* (as to be reloaded when loading project)
* \param forceLoad yes/no should mo file be reloaded in OMC when already loaded in OMC
*/
bool ProjectBase::loadPlugin(QString pluginPath, bool storePath, bool forceLoad)
{
    InfoSender::instance()->sendNormal("Loading plugin : "+pluginPath);

    bool tryLoad;

    tryLoad = (forceLoad || !_pluginsLoaded.values().contains(pluginPath));

    if(!tryLoad)
        return false;

    // unload if needed
    QFileInfo pluginFile(pluginPath);
    QDir appDir(QApplication::applicationDirPath());
    QString destPluginPath = appDir.absoluteFilePath(pluginFile.fileName());

    if(forceLoad && _pluginsLoaded.values().contains(pluginPath))
        unloadPlugin(destPluginPath);

    // copy plugin in application folder since it requires libraries
    if(pluginPath!=destPluginPath)
        appDir.remove(pluginFile.fileName());
    QFile::copy(pluginPath,destPluginPath);


    // first try to load
    QPluginLoader loader(destPluginPath);
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

    return loadOk;
}

/**
* Unload a OMOptim plugin
* \param pluginPath full file path of plugin
*/
bool ProjectBase::unloadPlugin(QString pluginPath)
{
    InfoSender::instance()->sendNormal("Unloading plugin : "+pluginPath);

    _pluginsLoaded.remove(_pluginsLoaded.key(pluginPath));

    QFileInfo pluginFile(pluginPath);
    QString loadedFile = QApplication::applicationDirPath()+QDir::separator()+pluginFile.fileName();

    QPluginLoader loader(loadedFile);
    if(loader.unload())
    {
        emit projectChanged();
        return true;
    }
    else
        return false;
}

/**
* Unload all plugins
*/
bool ProjectBase::unloadPlugins()
{
    QStringList pluginsPaths(_pluginsLoaded.values());
    bool ok = true;
    for(int i=0;i<pluginsPaths.size();i++)
        ok = unloadPlugin(pluginsPaths.at(i)) && ok;

    emit projectChanged();

    return ok;
}

void ProjectBase::terminateProblemsThreads()
{
    QList<MOThreads::ProblemThread *> allLaunchedThreads = _problemsThreads.values();

    for(int i=0;i<allLaunchedThreads.size();i++)
    {
        QString msg ="Stopping thread "+allLaunchedThreads.at(i)->_name;
        InfoSender::instance()->sendCurrentTask(msg);
        //allLaunchedThreads.at(i)->terminate();
        //allLaunchedThreads.at(i)->exit(-1);
        allLaunchedThreads.at(i)->setIsAlive(false); // used for script purpose
        allLaunchedThreads.at(i)->stop();
        allLaunchedThreads.at(i)->wait();
        qDebug("finished Thread");
        InfoSender::instance()->eraseCurrentTask();
    }

    emit forgetProblems();
}


void ProjectBase::setFilePath(QString filePath)
{
    _filePath=filePath;

    //create models folder
    QFileInfo fileInfo(_filePath);
    QString modelsDir = fileInfo.dir().absolutePath()+QDir::separator()+"Models";
    LowTools::mkpath(modelsDir,false);

    emit projectChanged();
}

void ProjectBase::save(bool saveAllOMCases)
{
    Save::saveProject(this,saveAllOMCases);

    setSaved(true);
    emit projectChanged();
}

void ProjectBase::save(Result* result)
{
    // save project but not all omcases
    Save::saveProject(this,false);

    // save result
    Save::saveResult(this,result);

    emit projectChanged();
}

void ProjectBase::save(Problem* problem)
{
    // save project but not all omcases
    Save::saveProject(this,false);

    // save problem
    Save::saveProblem(this,problem);

    emit projectChanged();
}

bool ProjectBase::load(QString loadPath)
{
    if(_saveLoadMutex.tryLock())
    {
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
        _saveLoadMutex.unlock();
        return loaded;
    }
}


QString ProjectBase::filePath()
{
    return _filePath;
}

QDir ProjectBase::folder()
{
    QFileInfo fileInfo(_filePath);
    return fileInfo.absoluteDir();
}

QString ProjectBase::tempPath()
{
    return folder().absoluteFilePath("temp");
}


QString ProjectBase::problemsFolder()
{
    return folder().absoluteFilePath("Problems");
}

QString ProjectBase::resultsFolder()
{
    return folder().absoluteFilePath("Results");
}

void ProjectBase::addProblemInterface(ProblemInterface* problemInterface)
{
    _problemsInterfaces.addProblemInterface(problemInterface);
    emit interfacesModified();
    emit projectChanged();
}

void ProjectBase::removeProblemInterface(QString interfaceName)
{
    if(_problemsInterfaces.removeProblemInterface(interfaceName))
    {
        emit interfacesModified();
        emit projectChanged();
    }
}

void ProjectBase::addNewProblem(ProblemInterface* itf, QStringList modelsList,QString problemType)
{
    Problem* newProblem = itf->createNewProblem(this,modelsList,problemType);
    if(newProblem)
    {
        HighTools::checkUniqueProblemName(this,newProblem,_problems);

        _problems->addCase(newProblem);

        save(newProblem);

        emit addedProblem(newProblem);
    }
}

bool ProjectBase::addOMCase(QString filePath)
{
    // do not reload if already loaded
    if(!casesFiles().contains(QFileInfo(filePath)))
    {
        OMCase* newCase = Load::newOMCase(filePath,this);
        if(!newCase)
            return false;

        Problem* problem = dynamic_cast<Problem*>(newCase);
        if(problem)
            addProblem(problem);
        else
        {
            Result* result = dynamic_cast<Result*>(newCase);
            if(result)
                addResult(result);
        }
        return true;
    }
    else
    {
        InfoSender::instance()->sendWarning(QString("OMCase already loaded. Won't be loaded a second time. [")+filePath+QString("]"));
        return false;
    }
}


void ProjectBase::addOMCases(QDir folder)
{
    QString caseExtension = "mpb";

    QFileInfoList omCasesFiles = LowTools::findFiles(folder,caseExtension);

    for(int i=0;i<omCasesFiles.size();i++)
        addOMCase(omCasesFiles.at(i).absoluteFilePath());

}

void ProjectBase::addProblem(Problem *problem)
{
    HighTools::checkUniqueProblemName(this,problem,_problems);
    _problems->addCase(problem);

    //update GUI
    emit sendInfo(Info(ListInfo::ADDEDPROBLEM,problem->name()));
    emit projectChanged();
    emit addedProblem(problem);
}

void ProjectBase::addResult(Result *result)
{
    _results->addCase(result);

    emit sendInfo(Info(ListInfo::ADDEDPROBLEM,result->name()));

    //update GUI
    emit projectChanged();
    emit addedResult(result);
}

bool ProjectBase::checkConfiguration()
{
    bool ok = true;

    return ok;
}


bool ProjectBase::createTempDir()
{
    return LowTools::mkpath(tempPath(),true);
}


MOThreads::ProblemThread*  ProjectBase::launchProblem(Problem* problem, bool useSeparateThread)
{
    if(useSeparateThread && !_problemLaunchMutex.tryLock())
    {
        QString msg = "Another problem is already running. Could not launch a new one.";
        InfoSender::instance()->send(Info(msg));
        return NULL;
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
        MOThreads::ProblemThread* launchThread = new MOThreads::ProblemThread(this,launchedProblem,config);

        // connect signals
        connect(launchThread,SIGNAL(begun(Problem*)),this,SIGNAL(problemBegun(Problem*)));
        connect(launchThread,SIGNAL(newProgress(float)),this,SIGNAL(newProblemProgress(float)));
        connect(launchThread,SIGNAL(newProgress(float,int,int)),this,SIGNAL(newProblemProgress(float,int,int)));
        connect(launchThread,SIGNAL(finished(Problem*,Result*)),this,SLOT(onProblemFinished(Problem*,Result*)));
        connect(launchThread,SIGNAL(finished(Problem*,Result*)),this,SIGNAL(problemFinished(Problem*,Result*)));

        // store thread-problem
        _problemsThreads.insert(launchedProblem,launchThread);

        // start problem
        if(useSeparateThread)
            launchThread->start();
        else
            launchThread->run();

        return launchThread;
    }
}

MOThreads::ProblemThread* ProjectBase::launchProblem(QString problemName, bool useSeparateThread )
{
    Problem* problem = dynamic_cast<Problem*>(_problems->findItem(problemName));
    if(problem)
        return launchProblem(problem,useSeparateThread);

    return NULL;
}

void ProjectBase::launchProblems(QList<Problem*> problems, bool useSeparateThread )
{
    if(!_problemLaunchMutex.tryLock())
    {
        QString msg = "Another problem is already running. Could not launch a new one.";
        InfoSender::instance()->send(Info(msg));
    }
    else
    {

        // Create temporary directory where calculations are performed
        createTempDir();

        QList<MOThreads::ProblemThread*> threads;

        for(int i=0;i<problems.size();i++)
        {
            //Copy launched problem from selected one
            Problem* launchedProblem;
            launchedProblem = problems.at(i)->clone();


            //Create problem thread
            ProblemConfig config;
            MOThreads::ProblemThread* launchThread = new MOThreads::ProblemThread(this,launchedProblem,config);

            // connect signals
            connect(launchThread,SIGNAL(begun(Problem*)),this,SIGNAL(problemBegun(Problem*)));
            connect(launchThread,SIGNAL(newProgress(float)),this,SIGNAL(newProblemProgress(float)));
            connect(launchThread,SIGNAL(newProgress(float,int,int)),this,SIGNAL(newProblemProgress(float,int,int)));
            connect(launchThread,SIGNAL(finished(Problem*,Result*)),this,SLOT(onProblemFinished(Problem*,Result*)));
            connect(launchThread,SIGNAL(finished(Problem*,Result*)),this,SIGNAL(problemFinished(Problem*,Result*)));

            // store thread-problem
            _problemsThreads.insert(launchedProblem,launchThread);

            threads.push_back(launchThread);
        }

        for(int i=0;i<threads.size()-1;i++)
        {
            connect(threads.at(i),SIGNAL(finished()),threads.at(i+1),SLOT(start()));
        }


        // start first thread
        if(threads.size()>0)
        {   if(useSeparateThread)
                threads.at(0)->start();
            else
                threads.at(0)->run();
        }
        else
            _problemLaunchMutex.unlock();
    }
}

void ProjectBase::onProjectChanged()
{
    _isSaved = false;
    emit projectChanged();
}

void ProjectBase::onProblemStopAsked(Problem* problem)
{
    problem->stop();
    _problemLaunchMutex.unlock();
    _problemsThreads.remove(problem);
}

void ProjectBase::onProblemFinished(Problem* problem,Result* result)
{
    if(result)
    {
        //Results
        if(!result->isSuccess())
        {
            QString msg = "Problem "+ problem->getClassName()+ " has failed";
            InfoSender::instance()->send(Info(msg,ListInfo::ERROR2));
            delete result;
        }
        else
        {
            QString msg = "Problem "+ problem->getClassName()+ " succeeded";
            InfoSender::instance()->send(Info(msg,ListInfo::NORMAL2));

            if(result->name().isEmpty())
                result->setName(result->problem()->name()+" result");

            HighTools::checkUniqueResultName(this,result,_results);

            // prepare directory (erase content/create folder)
            QString storeFolder = resultsFolder()+QDir::separator()+result->name();
            LowTools::mkpath(storeFolder,true);

            result->store(storeFolder,tempPath());
            // result->setParent(this);
            addResult(result);
            save(result);
        }
    }
    _problemLaunchMutex.unlock();
    _problemsThreads.remove(problem);

    /// @todo check whether it is really deleted
    problem->deleteLater();
}

Problem* ProjectBase::restoreProblemFromResult(int numResult)
{
    return restoreProblemFromResult(_results->at(numResult));
}

Problem* ProjectBase::restoreProblemFromResult(Result* result)
{
    Problem* restoredPb = result->problem()->clone();
    restoredPb->setName(restoredPb->name().replace(" result",""));
    HighTools::checkUniqueProblemName(this,restoredPb,_problems);

    addProblem(restoredPb);
    return restoredPb;
}


void ProjectBase::removeResult(OMCase* result,bool saveProject )
{
    int num = results()->items.indexOf(result);
    if(num>-1)
    {

        // result to be removed
        emit beforeRemoveResult(dynamic_cast<Result*>(result));

        // remove folder and data
        QDir folder(result->saveFolder());
        if(folder!=QDir(resultsFolder()))
            LowTools::removeDir(folder.absolutePath());

        _results->removeRow(num);

        if(saveProject)
            save(false);
    }
}

void ProjectBase::removeAllResults()
{
    for(int i=0;i<results()->size();i++)
        removeResult(results()->items[i],false); // avoid multiple saving

    save(false);
}


void ProjectBase::removeProblem(Problem* problem)
{
    int num = problems()->items.indexOf(problem);
    if(num>-1)
    {
        // result to be removed
        emit beforeRemoveProblem(problem);

        // remove folder and data
        QDir problemFolderInfo(problem->saveFolder());
        QDir problemsFolderInfo(problemsFolder());

        if(problemFolderInfo!=problemsFolderInfo)
            LowTools::removeDir(problemFolderInfo.absolutePath());

        _problems->removeRow(num);

        save(false);
    }
}


void ProjectBase::removeCases(QList<OMCase*> cases)
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

void ProjectBase::renameCase(OMCase* curCase,QString newName)
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

OMCase *ProjectBase::findOMCase(QString name)
{
    OMCase* omCase = _problems->findItem(name);
    if(omCase)
        return omCase;

    omCase = _results->findItem(name);
    if(omCase)
        return omCase;

    return NULL;
}


bool ProjectBase::renameProblem(Problem* problem,QString newName)
{
    // test if name already exists
    if(_problems->findItem(newName))
        return false;

    // change name
    problem->rename(newName,true);
    save(problem);
    return true;
}

bool ProjectBase::renameResult(Result* result,QString newName)
{
    // test if name already exists
    if(_results->findItem(newName))
        return false;

    // change name
    result->rename(newName,true);
    save(result);
    return true;
}

void ProjectBase::openOMCaseFolder()
{
    QAction* actionSender = dynamic_cast<QAction*>(sender());
    if(actionSender)
    {
        QString omCaseName = actionSender->data().toString();
        OMCase* omCase = this->findOMCase(omCaseName);
        if(omCase)
            omCase->openFolder();
    }
}

QMap<QString,QString> ProjectBase::pluginsLoaded()
{
    return _pluginsLoaded;
}

/**
  * Returns list of files of problems and results.
  */
QFileInfoList ProjectBase::casesFiles()
{
    QFileInfoList result;

    for(int i=0;i<_problems->size();i++)
        result.push_back(_problems->at(i)->entireSavePath());

    for(int i=0;i<_results->size();i++)
        result.push_back(_results->at(i)->entireSavePath());

    return result;
}

