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

 	@file Optimization.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "Optimization.h"
#include "EA"
#include "MyAlgoUtils.h"
#include "CSV.h"
#include "LowTools.h"


Optimization::Optimization(Project* project,ModClass* rootModClass,ModReader* modReader,ModPlusCtrl* modPlusCtrl,ModModelPlus* modModelPlus)
{
	setProject(project);
	_rootModClass = rootModClass;
	_modModelPlus = modModelPlus;
	_modReader = modReader;
	_modPlusCtrl = modPlusCtrl;

	
	_type = Problem::OPTIMIZATION;
	_name="Optimization";
	_optimizedVariables = new MOVector<OptVariable>;
	_scannedVariables = new MOVector<ScannedVariable>;
	_objectives = new MOVector<OptObjective>;
	_blockSubstitutions = new BlockSubstitutions();

	_algos = MyAlgoUtils::getNewAlgos(this,_modReader,_modPlusCtrl,_rootModClass);
	for(int i=0;i<_algos.size();i++)
		connect(_algos.at(i),SIGNAL(configChanged()),this,SIGNAL(algoConfigsChanged()));

	_iCurAlgo=0;
	_useEI=false;
}

Optimization::Optimization(const Optimization &optim)
:Problem(optim)
{

	_modModelPlus = optim._modModelPlus;

	_optimizedVariables = optim._optimizedVariables->clone();
	_scannedVariables = optim._scannedVariables->clone();
	_objectives = optim._objectives->clone();
	_blockSubstitutions = optim._blockSubstitutions->clone();

	if(optim.result())
		_result = new OptimResult(*((OptimResult*)optim.result())) ;

	_useEI = optim._useEI;
	//eiProblem(optim.eiProblem);
}

Optimization::~Optimization()
{
	for(int i=0;i<_algos.size();i++)
		delete _algos.at(i);

	delete _optimizedVariables;
	delete _objectives;
	delete _blockSubstitutions;

	clearResult();
}

/** Description : Launch optimization procedure. checkBeforeComp() is not called in this function.
*   Be sure it has been called previously.
*/
bool Optimization::checkBeforeComp(QString & error)
{
	bool ok = true;
	int nbErrors = 0;
	QString curError;
	
	// check scanned variables
	for(int i=0;i<_scannedVariables->items.size();i++)
	{
		if(!_scannedVariables->items.at(i)->check(curError))
		{
			ok = false;
			curError.sprintf("Error concerning scan variable %s : %s",
				_scannedVariables->items.at(i)->name().toLatin1().data(),
				curError.toLatin1().data());
			error += curError + "\n";
			nbErrors++;
		}
	}

	// check otpimized variables
	for(int i=0;i<_optimizedVariables->items.size();i++)
	{
		if(!_optimizedVariables->items.at(i)->check(curError))
		{
			ok = false;
			curError.sprintf("Error concerning optimized variable %s : %s",
				_optimizedVariables->items.at(i)->name().toLatin1().data(),
				curError.toLatin1().data());
			error += curError + "\n";
			nbErrors++;
		}
	}

	// check objectives
	for(int i=0;i<_objectives->items.size();i++)
	{
		if(!_objectives->items.at(i)->check(curError))
		{
			ok = false;
			curError.sprintf("Error concerning objective %s : %s",
				_objectives->items.at(i)->name().toLatin1().data(),
				curError.toLatin1().data());
			error += curError + "\n";
			nbErrors++;
		}
	}

	// check number of optimized variables and objectives
	if(_optimizedVariables->items.size()==0)
	{
		ok = false;
		curError = "Should specify at least one optimized variable";
		error += curError + "\n";
		nbErrors++;
	}
	if(_objectives->items.size()==0)
	{
		ok = false;
		curError = "Should specify at least one objective";
		error += curError + "\n";
		nbErrors++;
	}

	// check if objectives have scan function
	if(_scannedVariables->items.size()>0)
	{
		bool okObj;
		for(int i=0;i<_objectives->items.size();i++)
		{
			if(_objectives->items.at(i)->scanFunction()==OptObjective::NONE)
			{
				ok = false;
				curError.sprintf("Objective %s should have a scanFunction defined",
					_objectives->items.at(i)->name().toLatin1().data());
				error += curError + "\n";
				nbErrors++;
			}
		}
	}

	if(nbErrors>0)
	{
		if(nbErrors==1)
			error.insert(0,"1 error : \n");
		else
			error.insert(0,QString::number(nbErrors)+" errors : \n");
	}
	return ok;
}



/** Description : Launch optimization procedure. checkBeforeComp() is not called in this function.
*   Be sure it has been called previously.
*/
void Optimization::launch(ProblemConfig _config)
{
	emit begun(this);

	// first create temp dir
	QDir dir;
	if(dir.exists(_config.tempDir))
		dir.rmdir(_config.tempDir);
	dir.mkdir(_config.tempDir);

	//create different dymosim executable for blocksubstitutions
	QList<ModModelPlus*> _subModels;
	QList<BlockSubstitutions*> _subBlocks;

	createSubExecs(_subModels,_subBlocks);

	/*EABase* _ea = EAUtils::getNewEA(project,this,curEA);
	_ea->config = eaConfigs.at(curEA);
	_ea->setSubModels(_subModels,_subBlocks);*/

	_result = ((EABase*)getCurAlgo())->launch(_config.tempDir);

	emit finished(this);
}





void Optimization::createSubExecs(QList<ModModelPlus*> & subModels, QList<BlockSubstitutions*> & subBlocks)
{

	subModels.clear();
	subBlocks.clear();

	QMultiMap<QString,QString> map;
	// fill map	
	for(int i=0; i < _blockSubstitutions->getSize();i++)
	{
		BlockSubstitution *curBlockSub = _blockSubstitutions->getAt(i);
		if(!curBlockSub->subComponent.isEmpty())
			map.insert(curBlockSub->orgComponent,curBlockSub->subComponent);
	}

	int nbOrgs = map.uniqueKeys().size();
	//adding non-moving cases for each orgComponent
	for(int i = 0; i<nbOrgs; i ++)
	{
		map.insert(map.uniqueKeys().at(i),map.uniqueKeys().at(i));
	}

	//build first index and maximum index
	QList<int> index, maxIndex;
	nbOrgs = map.uniqueKeys().size();
	for(int i = 0; i<nbOrgs; i ++)
	{
		index.push_back(0);
		QList<QString> subs = map.values(map.uniqueKeys().at(i));
		maxIndex.push_back(subs.size()-1);
	}

	int iCase=0;
        bool oneChange;
	while(!index.isEmpty())
	{
		// Display case (for debug)
		QString msg = "CASE " + QString::number(iCase) + "\n";
		for(int i=0; i < index.size(); i++)
		{
			msg += map.uniqueKeys().at(i);
			msg += " -> ";
			msg += map.values(map.uniqueKeys().at(i)).at(index.at(i));
			msg+=",";
		}
		msg.remove(msg.size()-1,1);
		msg +="\n \n";
		infoSender.debug(msg);


		// create folder
		QString newName = _modModelPlus->modModelName() + "_case_"+QString::number(iCase);
		QString newFolder = saveFolder()+ QDir::separator() + "SubModels" + QDir::separator() + newName;
		QDir dir(saveFolder());
		dir.mkpath(newFolder);
		QDir newDir(newFolder);
		QString newMmoFilePath = newDir.absoluteFilePath(newName+".mmo");
		
		// clone mo file
		QFile moFile(_modModelPlus->modModel()->filePath());
		QString newMoPath = newDir.filePath(moFile.fileName());
		
		moFile.copy(newMoPath);
		
		// create new modModel
		ModModel* newModModel = new ModModel(_project->moomc(),_project->rootModClass(),_modModelPlus->modModelName(),newMoPath);
		
		// create new modModelPlus
		ModModelPlus* newModModelPlus = new ModModelPlus(_project->moomc(),_project,_project->modReader(),newModModel,_project->rootModClass());
		newModModelPlus->setMmoFilePath(newMmoFilePath);

		// apply blocksubs
		BlockSubstitutions *curSubBlocks = new BlockSubstitutions();
	
                oneChange = false;
		for(int i=0; i<index.size();i++)
		{
			QString replacedComp = map.uniqueKeys().at(i);
			QString replacingComp = map.values(map.uniqueKeys().at(i)).at(index.at(i));

			if(replacedComp != replacingComp)
			{
				BlockSubstitution* blockSub = _blockSubstitutions->find(replacedComp,replacingComp);
				if(blockSub)
				{
                                        oneChange = oneChange || newModModelPlus->applyBlockSub(blockSub,true);
					curSubBlocks->push_back(blockSub);
				}
			}

		}

                if(oneChange)
                {
		bool compiledOk = newModModelPlus->compile();
		
		if(compiledOk)
		{
			// store subModel and subBlocks
			subModels.push_back(newModModelPlus);
			subBlocks.push_back(curSubBlocks);
			_foldersToCopyNames << newFolder;

			infoSender.send( Info(ListInfo::SUBMODELADDED,newName));
		}
		else
		{
			infoSender.send( Info(ListInfo::SUBMODELNOTADDED,newName));
		}
                }
		iCase++;
		index = LowTools::nextIndex(index,maxIndex);
	}
}

ModModelPlus* Optimization::modModelPlus()
{
	return _modModelPlus;
}

//void Optimization::setEAConfigs(QVector<EAConfig*> _eaConfigs)
//{
//	eaConfigs = _eaConfigs;
//
//	for(int i=0;i<eaConfigs.size();i++)
//		eaConfigs.at(i)->setProblem(this);
//}
//EAConfig* Optimization::getEAConfig(int _iEA)
//{
//	return ((EABase*)algos.at(_iEA))->config;
//}
//QVector<EAConfig*> Optimization::getEAConfigs()
//{
//	QVector<EAConfig*> configs;
//	for(int i=0;i<algos.size();i++)
//		configs.push_back(getEAConfig(i));
//	return configs;
//}





void Optimization::setBlockSubstitutions(BlockSubstitutions* blockSubstitutions)
{
	if(_blockSubstitutions)
		delete _blockSubstitutions;

	_blockSubstitutions = blockSubstitutions;
}

/**
* Description Save problem in a folder in XML form.
*/
void Optimization::store(QString destFolder, QString tempDir)
{
	Problem::store(destFolder,tempDir);
}

QDomElement Optimization::toXMLData(QDomDocument & doc)
{
	
	QDomElement cProblem = doc.createElement(getClassName());
	//***********************
	// Problem definition
	//***********************
	QDomElement cInfos = doc.createElement("Infos");
	cProblem.appendChild(cInfos);
	cInfos.setAttribute("name", name());
	cInfos.setAttribute("type", type());
	cInfos.setAttribute("model", _modModelPlus->modModelName());

	// Optimized variables
	QDomElement cOptVariables = _optimizedVariables->toXmlData(doc,"OptimizedVariables");
	cProblem.appendChild(cOptVariables);

	// objectives
	QDomElement cObjectives = _objectives->toXmlData(doc,"Objectives");
	cProblem.appendChild(cObjectives);

	// ScannedVariables
	QDomElement cScanned = _scannedVariables->toXmlData(doc,"ScannedVariables");
	cProblem.appendChild(cScanned);

	//BlockSubstitutions
	QDomElement cBlocks = _blockSubstitutions->toXMLData(doc);
	cProblem.appendChild(cBlocks);

	// ea configuration
	MyAlgorithm *curAlgo = getCurAlgo();

	if(curAlgo)
	{
		QDomElement cEA = doc.createElement("EA");
		cProblem.appendChild(cEA);
		
		QDomElement cEAInfos = doc.createElement("Infos");
		cEAInfos.setAttribute("num",getiCurAlgo());
		cEA.appendChild(cEAInfos);

		QDomElement cAlgoParameters = curAlgo->_config->parameters->toXmlData(doc,"Parameters");
		cEA.appendChild(cAlgoParameters);
	}

	return cProblem;
}

int Optimization::nbScans()
{
	if(_scannedVariables->items.size()==0)
		return 1;
	
	int nbScans = 1;
	for(int i=0;i<_scannedVariables->items.size();i++)
		nbScans = nbScans*_scannedVariables->items.at(i)->nbScans();

	return nbScans;
}
