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

 	@file Problem.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "Problem.h"
#include "LowTools.h"
#include "EABase.h"

Problem::Problem()
{
	_num = -1;
	_type = UNDEFINED;

	_result = NULL;
	connect(this,SIGNAL(sentInfo(Info)),&infoSender,SIGNAL(sent(Info)));
}

Problem::Problem(const Problem &problem)
{
	_name = problem._name;
	_type = problem._type;
	_num = problem._num;
	
        _filesToCopy = problem._filesToCopy;
	
	_saveFolder = problem._saveFolder;
	_saveFileName = problem._saveFileName;

	_project = problem._project;
	_modReader = problem._modReader;
	_rootModClass = problem._rootModClass;
	
	_result = NULL; //(need to be created in subclasses copy constructors)

	//copying algos
	for(int i=0;i<problem._algos.size();i++)
	{
		_algos.push_back(problem._algos.at(i)->clone());
		_algos.at(i)->setProblem(this);
	}

	_iCurAlgo = problem._iCurAlgo;
	connect(this,SIGNAL(sentInfo(Info)),&infoSender,SIGNAL(sent(Info)));
}

Problem::~Problem(void)
{
        deleteResult();
}

QString Problem::getFieldName(int field,int role)
{
	return "name";
}
unsigned Problem::getNbFields()
{
	return 1;
}

int Problem::getiCurAlgo()
{
	return _iCurAlgo;
}

MyAlgorithm* Problem::getCurAlgo()
{
	if((_iCurAlgo>-1)&&(_iCurAlgo<_algos.size()))
		return _algos.at(_iCurAlgo);
	else
		return NULL;
}
AlgoConfig* Problem::getCurAlgoConfig()
{
	if((_iCurAlgo>-1)&&(_iCurAlgo<_algos.size()))
		return _algos.at(_iCurAlgo)->_config;
	else
		return NULL;
}

QStringList Problem::getAlgoNames()
{
	QStringList names;
	for(int i=0;i<_algos.size();i++)
		names.push_back(_algos.at(i)->name());
	return names;

}
void Problem::setName(QString name)
{
	_name=name;
	emit renamed(_name);

	if(_saveFileName.isEmpty())
		setDefaultSaveFileName();
}
void Problem::setNum(int num)
{
	_num=num;
}

void Problem::setType(ProblemType type)
{
	_type=type;
}


void Problem::setProject(Project* project)
{
	_project=project;
}

void Problem::setResult(Result* result)
{
	_result = result;
}
void Problem::setiCurAlgo(int iCurAlgo)
{
	_iCurAlgo = iCurAlgo;
}

void Problem::setSaveFolder(QString saveFolder)
{
	_saveFolder=saveFolder;
        if(_result)
            _result->setSaveFolder(QDir(saveFolder));
}

void Problem::setDefaultSaveFileName()
{
	if(_name.isEmpty())
		_saveFileName= "problem.mpb";
	else
		_saveFileName = _name + ".mpb";
}

void Problem::setEntireSavePath(QString savePath)
{
	QFileInfo fInfo(savePath);
	setSaveFolder(fInfo.canonicalPath());
	_saveFileName = fInfo.fileName();
}

QString Problem::saveFolder()
{
	return _saveFolder;
}

QString Problem::saveFileName()
{
	return _saveFileName;
}

QString Problem::entireSavePath()
{
	return _saveFolder + QDir::separator() + _saveFileName;
}



//
//void Problem::updateSavePath(QString problemPath)
//{
//	QDir dir = QDir(problemPath);
//
//	if (!dir.exists())
//	{
//		dir.mkdir(problemPath);
//	}
//
//	QString folderName,tempFolderName;
//
//	folderName = name;
//	folderName.replace("(","");
//	folderName.replace(")","");
//	folderName.replace(" ","_");
//
//	QString fileName = folderName;
//
//	tempFolderName=folderName;
//
//	QStringList folderList=dir.entryList(QDir::Dirs);
//
//	int i=2;
//	while(folderList.contains(folderName))
//	{
//		folderName = tempFolderName + "_" + QString::number(i);
//		i++;
//	}
//
//	dir.mkdir(folderName);
//
//	savePath = QString(problemPath+QDir::separator()
//		+folderName+QDir::separator()
//		+fileName);	
//}



void Problem::openFolder()
{
	LowTools::openFolder(_saveFolder);
}

bool Problem::isSolved()
{
	return ((_result!=NULL) && (_result->isSuccess()));
}



/**
* Description Store problem files in destFolder. Is called when a problem resolution is finished.
* @param destFolder destination folder path.
* @param tempDir dir from where problem files are copied
*/
void Problem::store(QString destFolder, QString tempDir)
{

	// update save paths
	setSaveFolder(destFolder);
	setDefaultSaveFileName();

	QString savePath = _saveFolder + QDir::separator() + _saveFileName;


	QDir dir = QDir(_saveFolder);

	if (!dir.exists())
	{
		dir.mkpath(_saveFolder);
	}
	else
	{
		LowTools::removeDir(_saveFolder);
		dir.mkdir(_saveFolder);
	}

	//copy needed path from old place to new one
	if(tempDir != "")
	{
		QDir tmpDir(tempDir);
		QDir newDir(_saveFolder);

		// copy problem files and folders
                QStringList fileNames = tmpDir.entryList();
		for(int i=0;i<fileNames.size();i++)
		{
			QFile::copy(tempDir + QDir::separator() + fileNames.at(i),_saveFolder + QDir::separator() + fileNames.at(i));
		}

		// copy result files
		if(isSolved())
		{
			QStringList fileNames = tmpDir.entryList(_result->filesToCopyNames());
			for(int i=0;i<fileNames.size();i++)
			{
				QFile::copy(tempDir + QDir::separator() + fileNames.at(i),_saveFolder + QDir::separator() + fileNames.at(i));
			}
		}
	}

        result()->setSaveFolder(QDir(destFolder));
}

void Problem::deleteResult()
{
	if(_result)
		delete _result;
	_result = NULL;
}

/**
* Description Rename problem
* @param newName new problem name
* @param changeFolder if yes, rename folder also
*/
void Problem::rename(QString newName, bool changeFolder)
{
	QString oldName = _name;
	setName(newName);

	if(changeFolder)
	{
		QString oldSaveFolder = saveFolder();
		QString newSaveFolder = oldSaveFolder;
		newSaveFolder.replace(oldName,newName);

		QString oldSaveFileName = saveFileName();
		QString newSaveFileName = oldSaveFileName;
		newSaveFileName.replace(oldName,newName);

		QDir newDir(newSaveFolder);
		if(newDir.exists())
		{
			newDir.cd("..");
			newDir.rmdir(newSaveFolder);
			newDir.setCurrent(newSaveFolder);
		}

		LowTools::copyDir(oldSaveFolder,newSaveFolder);
		LowTools::removeDir(oldSaveFolder);

		newDir.rename(oldSaveFileName,newSaveFileName);

		setSaveFolder(newSaveFolder);
		_saveFileName = newSaveFileName;
	}
}

