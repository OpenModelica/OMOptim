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
 	@version 0.9 

  */
#include "Project.h"


Project::Project()
{
	_isdefined = false;
	_curProblem = -1;
	

	_problems = new Problems();
	_solvedProblems = new Problems();
	_curLaunchedProblem = NULL;
	setCurModClass(NULL);

	_moomc = new MOomc("OMOptim",true);
	
	_rootModClass = new ModClass(_moomc);
	_modReader = new ModReader(_moomc);

#ifdef USEEI
	_eiReader = new EIReader();
#endif
}

Project::~Project()
{
	if(_rootModClass)
		delete _rootModClass;

	delete _problems;
	delete _solvedProblems;

        terminateOmsThreads();
        delete _moomc;
        delete _modReader;

#ifdef USEEI
        delete _eiReader;
#endif
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
	_rootModClass->clear();
	_mapModelPlus.clear();

        // OMC
        terminateOmsThreads();
	_moomc->clear();


	_problems->reset();
	_solvedProblems->reset();
	
	_isdefined=false;
	_filePath.clear();
	_name.clear();
	_curProblem=-1;
	_curLaunchedProblem = NULL;
	setCurModClass(NULL);

	_moFiles.clear();
	_mmoFiles.clear();

	// delete GUI tabs...
	emit projectReset();
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

	// load moFile and read it
	_modReader->readMoFile(_rootModClass,moFilePath,_mapModelPlus,forceLoad);

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
	_modReader->readMoFiles(_rootModClass,moFilePaths,_mapModelPlus,forceLoad);

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
* \brief
*	Refresh modelica tree in GUI. Do not reload in OpenModelica ! Just reread hierarchy.
*/
void Project::refreshAllMod()
{
	QStringList omcClasses = _moomc->getClassNames();
	QStringList loadedClasses = _rootModClass->getChildrenNames();
	QMap<QString,ModModelPlus*> strMapModelPlus;

	// Copy map information (using string instead of ModModel*)
	ModModel* curModModel;
	for(int i=0;i<_mapModelPlus.keys().size();i++)
	{
		curModModel = _mapModelPlus.keys().at(i);
		strMapModelPlus.insert(curModModel->name(Modelica::FULL),_mapModelPlus.value(curModModel));
	}

	_rootModClass->clear();

	for(int iO=0;iO<omcClasses.size();iO++)
	{
		_modReader->addModClass(_rootModClass,omcClasses.at(iO),_moomc->getFileOfClass(omcClasses.at(iO)));
	}

	// refreshing map
	_mapModelPlus.clear();
	QList<ModModelPlus*> listModPlus = strMapModelPlus.values();
	ModModelPlus* curModModelPlus;
	for(int iP=0;iP<listModPlus.size();iP++)
	{
		curModModelPlus = listModPlus.at(iP);
		curModModel = dynamic_cast<ModModel*>(_modReader->findInDescendants(_rootModClass,strMapModelPlus.key(curModModelPlus)));
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
* \brief
* Find a ModModel with its name.
*/
ModModel* Project::findModModel(QString modelName)
{
	ModClass* modModel = _modReader->findInDescendants(_rootModClass,modelName);

	if(!modModel || modModel->getClassRestr()!=Modelica::MODEL)
		return NULL;
	else
		return (ModModel*)modModel;
}

QList<ModModelPlus*> Project::allModModelPlus()
{
	return _mapModelPlus.values();
}

bool Project::addModModelPlus(ModModelPlus* modModelPlus)
{
	ModModel* modModel = modModelPlus->modModel();
	if(!_mapModelPlus.contains(modModel))
	{
		_mapModelPlus.insert(modModel,modModelPlus);
		return true;
	}
	else
		return false;
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
	ModModelPlus* newModModelPlus = new ModModelPlus(_moomc,this,_modReader,model,_rootModClass);
	
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

bool Project::compileModModel(ModModel* model)
{
	ModModelPlus* concernedMMPlus = modModelPlus(model);
	return compileModModelPlus(concernedMMPlus);
}

bool Project::compileModModelPlus(ModModelPlus* modModelPlus)
{
	return modModelPlus->compile();
}

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

	bool loaded = Load::loadProject(loadPath,this);
	if (loaded)
	{
		emit infoSender.send( Info(ListInfo::PROJECTLOADSUCCESSFULL,filePath()));
		emit projectChanged();
	}
	else
	{
		emit infoSender.send( Info(ListInfo::PROJECTLOADFAILED,filePath()));
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

QString Project::solvedProblemsFolder()
{
	return folder()+QDir::separator()+"SolvedProblems";
}

void Project::addNewProblem(Problem::ProblemType problemType, ModModel* modelConcerned)
{
	Problem* newProblem = NULL;
	ModModelPlus* modModelPlus = NULL;

	// Looking for corresponding modModelPlus
	if(modelConcerned)
	{
		modModelPlus = _mapModelPlus.value(modelConcerned);
		if(!modModelPlus)
			modModelPlus = newModModelPlus(modelConcerned);
	}
	switch(problemType)
	{
	case Problem::ONESIMULATION :
		newProblem  = new OneSimulation(this,_rootModClass,_modReader,_modPlusCtrl,modModelPlus);
		break;
	case Problem::OPTIMIZATION :
		newProblem = new Optimization(this,_rootModClass,_modReader,_modPlusCtrl,modModelPlus);
		break;
#ifdef USEEI
	case Problem::PROBLEMEI:
                newProblem = new ProblemTarget(this,_eiReader,_modReader,_moomc);
		break;
#endif

	}

	HighTools::checkUniqueProblemName(this,newProblem,_problems);

	if(!newProblem->isSolved())
	{
		_problems->addProblem(newProblem);
		emit addedProblem(newProblem);
	}
	else
	{
		_solvedProblems->addProblem(newProblem);
		emit addedSolvedProblem(newProblem);
	}
}



void Project::addSolvedProblem(Problem *solvedProblem)
{	
	_solvedProblems->addProblem(solvedProblem);
	
	// Saving result into file
	//Save::saveResult(result_);

	//update GUI
	emit projectChanged();
	emit addedSolvedProblem(solvedProblem);
}

void Project::addSolvedProblem(QString filePath)
{
	Problem* newSolvedProblem = Load::newSolvedProblemFromFile(filePath,this);
	if(newSolvedProblem)
		addSolvedProblem(newSolvedProblem);
}



void Project::addProblem(Problem *problem)
{
	_problems->addProblem(problem);
	
	// Saving result into file
//	Save::saveProblem(problem);

	//update GUI
	emit projectChanged();
	emit addedProblem(problem);
}



void Project::addProblem(QString filePath)
{
	Problem* newProblem = Load::newProblemFromFile(filePath,this);
	if (newProblem)
		addProblem(newProblem);
}


void Project::createTempDir()
{
	QDir tempDir(tempPath());
	if(tempDir.exists())
		LowTools::removeDir(tempPath());
	tempDir.mkdir(tempPath());

}


void Project::launchProblem(int num)
{
	if(!_problemLaunchMutex.tryLock())
	{
		// another problem is already running
	}
	else
	{
		//Copy launched problem from selected one
		Problem* curProblem;
		curProblem = _problems->items.at(num); 
		Problem* launchedProblem;
		switch(curProblem->type())
		{
		case Problem::ONESIMULATION :
			launchedProblem = new OneSimulation(*((OneSimulation*)curProblem));
			break;
		case Problem::OPTIMIZATION :
			launchedProblem = new Optimization(*((Optimization*)curProblem));
			break;
#ifdef USEEI
		case Problem::PROBLEMEI :
			launchedProblem = new ProblemTarget(*((ProblemTarget*)curProblem));
			break;
#endif
		}

		// connect signal
		connect(launchedProblem,SIGNAL(begun(Problem*)),this,SIGNAL(problemBegun(Problem*)));
		connect(launchedProblem,SIGNAL(finished(Problem*)),this,SIGNAL(problemFinished(Problem*)));
		connect(launchedProblem,SIGNAL(newProgress(float)),this,SIGNAL(newProblemProgress(float)));
		connect(launchedProblem,SIGNAL(newProgress(float,int,int)),this,SIGNAL(newProblemProgress(float,int,int)));
		connect(launchedProblem,SIGNAL(finished(Problem*)),this,SLOT(onProblemFinished(Problem*)));
		
		// Create temporary directory where calculations are performed
		createTempDir();

		// store temp problem
		_curLaunchedProblem = launchedProblem;

		//Launch problem
		ProblemConfig config(tempPath());
		MOThreads::LaunchProblem* launchThread = new MOThreads::LaunchProblem(launchedProblem,config);
		launchThread->start();
	}
}

void Project::onProblemFinished(Problem* launchedProblem)
{
	//Results
	if(!launchedProblem->isSolved())
	{
		QString logFile;
		switch(launchedProblem->type())
		{
		case Problem::ONESIMULATION :
			logFile = tempPath()+QDir::separator()+"dslog.txt";
			logFile.replace("\\","/");
			emit infoSender.send( Info(ListInfo::ONESIMULATIONFAILED,logFile));
			break;
		case Problem::OPTIMIZATION :
			emit infoSender.send( Info(ListInfo::OPTIMIZATIONFAILED));
			break;
#ifdef USEEI
		case Problem::PROBLEMEI :
			emit infoSender.send( Info(ListInfo::PROBLEMEIFAILED));
			break;
#endif
		}
	}
	else
	{
		switch(launchedProblem->type())
		{
		case Problem::ONESIMULATION :
			emit infoSender.send( Info(ListInfo::ONESIMULATIONSUCCESS));
			break;
		case Problem::OPTIMIZATION :
			emit infoSender.send( Info(ListInfo::OPTIMIZATIONSUCCESS));
			break;
#ifdef USEEI
		case Problem::PROBLEMEI :
			emit infoSender.send( Info(ListInfo::PROBLEMEISUCCESS));
			break;
#endif
		}

		launchedProblem->setName(launchedProblem->name()+" result");
		HighTools::checkUniqueProblemName(this,launchedProblem,_solvedProblems);
		
		launchedProblem->store(QString(solvedProblemsFolder()+QDir::separator()+launchedProblem->name()),tempPath());
		addSolvedProblem(launchedProblem);

		save();
	}
	_problemLaunchMutex.unlock();
	_curLaunchedProblem = NULL;
}


void Project::removeProblem()
{
	// SLOT : sender is menu, data is containing problem number
	QAction *action = qobject_cast<QAction *>(sender());
	if (action)
	{
		removeProblem(action->data().toInt());
	}
	
}

void Project::removeSolvedProblem()
{
	// SLOT : sender is menu, data is containing problem number
	QAction *action = qobject_cast<QAction *>(sender());
	if (action)
	{
		removeSolvedProblem(action->data().toInt());
	}
}

Problem* Project::restoreProblemFromSolvedOne(int numSolved)
{
	Problem* solvedPb = _solvedProblems->items.at(numSolved);
	Problem* restoredPb;

	switch(solvedPb->type())
	{
	case Problem::ONESIMULATION :
		restoredPb = new OneSimulation(*((OneSimulation*)solvedPb));
		break;
	case Problem::OPTIMIZATION :
		restoredPb = new Optimization(*((Optimization*)solvedPb));
		break;
	}
	restoredPb->clearResult();

	restoredPb->setName(restoredPb->name().replace(" result",""));
	
	HighTools::checkUniqueProblemName(this,restoredPb,_problems);

	addProblem(restoredPb);
	return restoredPb;
}


void Project::removeSolvedProblem(int num)
{
	// result to be removed
	emit beforeRemoveSolvedProblem(num);

	// remove folder and data
	QString folder = QDir(_solvedProblems->items.at(num)->saveFolder()).absolutePath();
	LowTools::removeDir(folder);
	_solvedProblems->removeRow(num);

	save();
}


void Project::removeProblem(int num)
{
	// result to be removed
	emit beforeRemoveProblem(num);

	// remove folder and data
	QString folder = QFileInfo(_problems->items.at(num)->saveFolder()).absolutePath();
	LowTools::removeDir(folder);
	_problems->removeRow(num);

	save();
}


bool Project::renameProblem(int i,QString newName)
{
	// test if name already exists
	if(_problems->findItem(newName)>-1)
		return false;

	// test if index is valid
	if((i<0) || (i>_problems->rowCount()))
		return false;

	// change name
	_problems->items.at(i)->rename(newName,true);
	save();
	return true;
}	

bool Project::renameSolvedProblem(int i,QString newName)
{
	// test if name already exists
	if(_solvedProblems->findItem(newName)>-1)
		return false;

	// test if index is valid
	if((i<0) || (i>_solvedProblems->rowCount()))
		return false;

	// change name
	_solvedProblems->items.at(i)->rename(newName,true);
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
			infoSender.send(Info(msg,ListInfo::NORMAL2));
			_moomc->getThreads().at(i)->terminate();
		}
		}
	}


void Project::onProblemStopAsked(Problem* problem)
{
	if(problem->type()==Problem::OPTIMIZATION)
	{
		((EABase*)((Optimization*)problem)->getCurAlgo())->onStopAsked();
	}
}


Problem* Project::curLaunchedProblem()
{
	return _curLaunchedProblem;
}

QStringList Project::moFiles()
{
	return _moFiles;
}

QStringList Project::mmoFiles()
{
	return _mmoFiles;
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
