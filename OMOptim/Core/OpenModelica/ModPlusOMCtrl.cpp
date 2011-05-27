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

@file ModPlusOMCtrl.cpp
@brief Comments for file documentation.
@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
Company : CEP - ARMINES (France)
http://www-cep.ensmp.fr/english/
@version 


*/
#include "ModPlusOMCtrl.h"


ModPlusOMCtrl::ModPlusOMCtrl(ModModelPlus* model,MOomc* moomc,QString mmoFolder,QString moFilePath,QString modModelName)
:ModPlusCtrl(model,moomc,mmoFolder,moFilePath,modModelName)
{
	QDir tDir(mmoFolder);

	_initFile = _modModelName+"_init.txt";
	_resFile = _modModelName+"_res.csv";
	_exeFile = _modModelName+".exe";

	_copyAllMoOfFolder = true;

	_parameters = new MOVector<ModModelParameter>();
	setDefaultParameters();
}

ModPlusOMCtrl::~ModPlusOMCtrl(void)
{
}

ModPlusCtrl::Type ModPlusOMCtrl::type()
{
	return ModPlusCtrl::OPENMODELICA;
}

// Parameters
void ModPlusOMCtrl::setDefaultParameters()
{
	_parameters->addItem(new ModModelParameter(OpenModelica::STOPVALUE,"stop value","Stop value",1,"double",0,1E100));
}

bool ModPlusOMCtrl::readOutputVariables(MOVector<Variable> *finalVariables,QString resFile)
{
	if(resFile.isEmpty())
		resFile = _resFile;

	return OpenModelica::getFinalVariablesFromFile(resFile,finalVariables,_modModelName);
}


bool ModPlusOMCtrl::readInitialVariables(MOVector<Variable> *initVariables,QString initFile)
{
	bool authorizeRecreate=false;
	if(initFile.isEmpty())
	{
		authorizeRecreate=true;
		initFile = _mmoFolder+QDir::separator()+_initFile;
	}
	initVariables->clear();
	QFileInfo initFileInfo = QFileInfo(initFile);
	
	if(!initFileInfo.exists()&&authorizeRecreate)
	{
		createInitFile();
	}

	if(!initFileInfo.exists())
	{
		return false;
	}
	else
	{
		OpenModelica::getInputVariablesFromFile(_moomc,initFile,initVariables,_modModelName);
		return true;
	}

	return true;	
}

bool ModPlusOMCtrl::compile()
{

        infoSender.send(Info("Compiling model "+_modModelName,ListInfo::NORMAL2));

	// compile
	bool success = OpenModelica::compile(_moomc,_moFilePath,_modModelName,_mmoFolder);

	// Inform
	ListInfo::InfoNum iMsg;
	if(success)
                iMsg = ListInfo::MODELCOMPILATIONSUCCESS;
	else
		iMsg = ListInfo::MODELCOMPILATIONFAIL;

	infoSender.send(Info(iMsg,_modModelName,_moFilePath));

	return success;
}


bool ModPlusOMCtrl::isCompiled()
{
	bool filesExist = true;
	QStringList filesNeeded;
	filesNeeded << _exeFile;

	QString filePath;
	QFile file;
	for(int i=0;i<filesNeeded.size();i++)
	{
		filePath = _mmoFolder+QDir::separator()+filesNeeded.at(i);
		file.setFileName(filePath);
		filesExist = filesExist && file.exists();
	}
	return filesExist;
}


bool ModPlusOMCtrl::simulate(QString tempFolder,MOVector<Variable> * inputVars,MOVector<Variable> * outputVars,QStringList filesToCopy)
{
	// eventually compile model
	if(!isCompiled())
		compile();

	// clear outputVars
	outputVars->clear();

	// Create tempDir
	QDir modelTempDir(tempFolder);
	modelTempDir.mkdir(tempFolder);

	// copy files in temp dir (#TODO : optimize with a config.updateTempDir in case of several consecutive launches)
        QStringList allFilesToCopy;
	QDir mmoDir = QDir(_mmoFolder);
        allFilesToCopy << mmoDir.filePath(_initFile) << mmoDir.filePath(_exeFile);
        allFilesToCopy.append(filesToCopy);

        QDir tempDir = QDir(tempFolder);
        QFileInfo fileToCopyInfo;
        QFile fileToCopy;

        for(int i=0; i< allFilesToCopy.size();i++)
	{
            fileToCopy.setFileName(allFilesToCopy.at(i));
            fileToCopyInfo.setFile(fileToCopy);
            tempDir.remove(fileToCopyInfo.fileName());
            fileToCopy.copy(tempDir.filePath(fileToCopyInfo.fileName()));
	}
	
	
	// remove previous log files
	QStringList filesToRemove;
	filesToRemove << _resFile;
	for(int i=0;i<filesToRemove.size();i++)
		tempDir.remove(filesToRemove.at(i));

	QString tempInitFile = tempDir.absoluteFilePath(_initFile);
	QString tempResFile = tempDir.absoluteFilePath(_resFile);
	QString tempExeFile = tempDir.absoluteFilePath(_exeFile);
	
	
	// Specifying new Variables values in dymosim input file
	OpenModelica::setInputVariables(tempInitFile,inputVars,_modModelName,parameters());

        // Info
        infoSender.send(Info("Simulating model "+_modModelName,ListInfo::NORMAL2));
	// Launching Dymosim
	OpenModelica::start(tempExeFile);

		//getting results
	//Checking if successed
	bool success=QFile::exists(tempResFile);
	
	if(!success)
		return false;

	bool readOk = readOutputVariables(outputVars,tempResFile);
	return readOk;
}
bool ModPlusOMCtrl::createInitFile()
{
	return compile();
}

void ModPlusOMCtrl::setMmoFolder(QString mmoFolder)
{
	_mmoFolder = mmoFolder;
}
