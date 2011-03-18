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

@file ModPlusDymolaCtrl.cpp
@brief Comments for file documentation.
@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
Company : CEP - ARMINES (France)
http://www-cep.ensmp.fr/english/
@version 


*/
#include "ModPlusDymolaCtrl.h"
#include "reportingHook.h"

ModPlusDymolaCtrl::ModPlusDymolaCtrl(ModModelPlus* model,MOomc* moomc,QString mmoFolder,QString moFilePath,QString modModelName)
:ModPlusCtrl(model,moomc,mmoFolder,moFilePath,modModelName)
{
	_dsinFile = "dsin.txt";
	_dsresFile = "dsres.txt";
	_dsfinalFile = "dsfinal.txt";
	_copyAllMoOfFolder = true;
	_outputReadMode = DSFINAL;

	_parameters = new MOVector<ModModelParameter>();
	setDefaultParameters();
}

ModPlusDymolaCtrl::~ModPlusDymolaCtrl(void)
{
}

ModPlusCtrl::Type ModPlusDymolaCtrl::type()
{
	return ModPlusCtrl::DYMOLA;
}

void ModPlusDymolaCtrl::setDefaultParameters()
{
	_parameters->addItem(new ModModelParameter((int)Dymola::STOPTIME,"stop time","Stop time",1,"double",0,1E100));
}

bool ModPlusDymolaCtrl::readOutputVariables(MOVector<Variable> *finalVariables,QString folder)
{
	if(folder.isEmpty())
		folder = _mmoFolder;
	QDir dir(folder);

	switch(_outputReadMode)
	{
	case DSFINAL :
		return readOutputVariablesDSFINAL(finalVariables,dir.absoluteFilePath(_dsfinalFile));
	case DSRES :
		return readOutputVariablesDSRES(finalVariables,dir.absoluteFilePath(_dsresFile));
	}
	return false;
}

bool ModPlusDymolaCtrl::readOutputVariablesDSFINAL(MOVector<Variable> *finalVariables, QString dsfinalFile)
{
	finalVariables->clear();
	QFileInfo dsfinalInfo = QFileInfo(dsfinalFile);
	
	if(!dsfinalInfo.exists())
	{
		return false;
	}
	Dymola::getVariablesFromDsFile(dsfinalFile,finalVariables,_modModelName);

	return true;
}

bool ModPlusDymolaCtrl::readOutputVariablesDSRES(MOVector<Variable> *finalVariables, QString dsresFile)
{
	finalVariables->clear();
	


	if(dsresFile.isEmpty())
	{
		dsresFile = _mmoFolder+QDir::separator()+_dsresFile;
	}

	QFileInfo dsresInfo = QFileInfo(dsresFile);

	if (dsresInfo.exists())
	{
		Dymola::getFinalVariablesFromDsFile(dsresFile,finalVariables,_modModelName);
		return true;
	}
	else
		return false;
}

bool ModPlusDymolaCtrl::readInitialVariables(MOVector<Variable> *initVariables,QString dsinFile)
{
	bool authorizeRecreate=false;

	if(dsinFile.isEmpty())
	{
		authorizeRecreate=true;
		dsinFile = _mmoFolder+QDir::separator()+_dsinFile;
	}
	initVariables->clear();
	QFileInfo dsinInfo = QFileInfo(dsinFile);
	
	if(!dsinInfo.exists()&&authorizeRecreate)
	{
		createDsin();
	}

	if(!dsinInfo.exists())
	{
		infoSender.send(Info("Unable to create DsinFile",ListInfo::ERROR2));
		return false;
	}
	else
	{
		Dymola::getVariablesFromDsFile(dsinFile,initVariables,_modModelName);
		//sendInfo (new Info(ListInfo::READVARIABLESSUCCESS));
		return true;
	}
}

bool ModPlusDymolaCtrl::compile()
{
	//// dir
	//QDir dir(mmoFolder);
	//
	//// Copy .mo file in model folder
	//QFileInfo orgMoFileInfo(moFilePath);
	//QString copiedMoPath = dir.absoluteFilePath(orgMoFileInfo.fileName());
	//QFile orgMoFile(moFilePath);
	//QFile dstMoFile(copiedMoPath);
	//if(dstMoFile.exists())
	//	dstMoFile.remove();
	//orgMoFile.copy(copiedMoPath);


	//// copy dependencies
	//QStringList depFileNames = moomc->getDependenciesPaths(moFilePath,false);
	//for(int i=0;i<depFileNames.size();i++)
	//{
	//	// check if file exists
	//	QFileInfo depFileInfo(depFileNames.at(i));
	//	QString depFilePath;
	//	if(!depFileInfo.exists())
	//	{
	//		// check in folder
	//		depFileInfo = QFileInfo(moFilePath+QDir::separator()+depFileNames.at(i));
	//	}

	//	// copy
	//	if(depFileInfo.exists())
	//	{
	//		QFile depFile(depFileInfo.absoluteFilePath());
	//		depFile.copy(dir.absoluteFilePath(depFileInfo.fileName()));
	//	}
	//}

	////copy mo files situated in same folder
	//if(copyAllMoOfFolder)
	//{
	//	QDir orgDir = orgMoFileInfo.absoluteDir();
	//	QStringList filters;
	//	filters << "*.mo";
	//	QStringList annexMoFiles = orgDir.entryList(filters);

	//	QFile annexMoFile;
	//	for(int i=0;i<annexMoFiles.size();i++)
	//	{
	//		annexMoFile.setFileName(orgDir.filePath(annexMoFiles.at(i)));
	//		annexMoFile.copy(dir.absoluteFilePath(annexMoFiles.at(i)));
	//	}

	//}

	// compile
	bool success = Dymola::firstRun(_moFilePath,_modModelName,_mmoFolder);

	// Inform
	ListInfo::InfoNum iMsg;
	if(success)
		iMsg = ListInfo::MODELCOMPILATIONSUCCESS;
	else
		iMsg = ListInfo::MODELCOMPILATIONFAIL;

	infoSender.send(Info(iMsg,_modModelName,_moFilePath));

	return success;
}


bool ModPlusDymolaCtrl::isCompiled()
{
	bool filesExist = true;
	QStringList filesNeeded;
	filesNeeded << "dsin.txt" << "dymosim.exe";

	QString filePath;
	for(int i=0;i<filesNeeded.size();i++)
	{
		filePath = _mmoFolder+QDir::separator()+filesNeeded.at(i);
		QFile file(filePath);
		filesExist = filesExist && file.exists();
	}
	return filesExist;
}


bool ModPlusDymolaCtrl::simulate(QString tempFolder,MOVector<Variable> * updatedVars,MOVector<Variable> * outputVars)
{
	// eventually compile model
	if(!isCompiled())
	{
		bool compileOk = compile();
		if(!compileOk)
			return false;
	}

	// clear outputVars
	outputVars->clear();

	// Create tempDir
	QDir modelTempDir(tempFolder);
	modelTempDir.mkdir(tempFolder);

	// copy files in temp dir (#TODO : optimize with a config.updateTempDir in case of several consecutive launches)
	QStringList filesToCopy;
	filesToCopy << "dsin.txt" << "dymosim.exe";
	QDir tempDir = QDir(tempFolder);
	QDir mmoDir = QDir(_mmoFolder);

	for(int i=0; i< filesToCopy.size();i++)
	{
		QFile copiedFile(mmoDir.filePath(filesToCopy.at(i)));
		tempDir.remove(filesToCopy.at(i));
		copiedFile.copy(tempDir.filePath(filesToCopy.at(i)));
	}
	
	// remove previous dymola log files
	QStringList filesToRemove;
	filesToRemove << "status" << "failure" << "success" << "dslog.txt";
	for(int i=0;i<filesToRemove.size();i++)
		tempDir.remove(filesToRemove.at(i));

	QString tempDsin = tempDir.absoluteFilePath("dsin.txt");
	QString tempDsres = tempDir.absoluteFilePath("dsres.txt");
	
	
	// Specifying new Variables values in dymosim input file
	Dymola::setVariablesToDsin(tempDsin,_modModelName,updatedVars,_parameters);

	// Launching Dymosim
	Dymola::start(tempFolder);

		//getting results
	//Checking if successed
	bool success=QFile::exists(tempDir.absoluteFilePath("success"));
	
	if(!success)
		return false;

	bool readOk = readOutputVariables(outputVars,tempFolder);
	return readOk;
}



bool ModPlusDymolaCtrl::createDsin()
{
	QDir dir(_mmoFolder);

	// copy dependencies
	QStringList depFileNames = _moomc->getDependenciesPaths(_moFilePath,false);
	for(int i=0;i<depFileNames.size();i++)
	{
		// check if file exists
		QFileInfo depFileInfo(depFileNames.at(i));
		QString depFilePath;
		if(!depFileInfo.exists())
		{
			// check in folder
			depFileInfo = QFileInfo(_moFilePath+QDir::separator()+depFileNames.at(i));
		}

		// copy
		if(depFileInfo.exists())
		{
			QFile depFile(depFileInfo.absoluteFilePath());
			depFile.copy(dir.absoluteFilePath(depFileInfo.fileName()));
		}
	}

	// compile
	bool success = Dymola::createDsin(_moFilePath,_modModelName,_mmoFolder);

	// Return
	return success;
}
void ModPlusDymolaCtrl::setMmoFolder(QString mmoFolder)
{
	_mmoFolder = mmoFolder;
}