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

 	@file Problem.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#if !defined(_PROBLEM_H)
#define _PROBLEM_H

#include <QtCore/QObject>
#include <QtCore/QTime>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

#include "ModModelPlus.h"
#include "Result.h"
#include "ProblemConfig.h"
#include "MyAlgorithm.h"
#include "ModClass.h"
#include "ModReader.h"
#include "ModPlusCtrl.h"
#include "InfoSender.h"





class Result;
class Project;



/**
* \brief Problem is an abstract class to be inherited by problem you want to solve.
*/
class Problem: public MOItem
{
	Q_OBJECT

public :
	enum ProblemType{
		UNDEFINED = -1,
		ONESIMULATION,
		OPTIMIZATION,
		PROBLEMEI
		//VARIABLEDETERMINATION
	};

protected :

	// General information
	Project* _project;
	Result* _result;
	ProblemType _type;
	int _num; //rank in problem vector

	// Files informations
	QStringList _filesToCopyNames;
	QStringList _foldersToCopyNames;
	QStringList _neededFiles;
	QString _saveFolder;
	QString _saveFileName;

	// Model controlers
	ModReader* _modReader;
	ModPlusCtrl* _modPlusCtrl;
	ModClass* _rootModClass;

	// Algorithm information
	QList<MyAlgorithm*> _algos;
	int _iCurAlgo;

public:
	
	// CTOR
	Problem();
	Problem(const Problem & s);
	virtual ~Problem(void);
	
	// MO Item overwriting
	virtual QString getFieldName(int iField,int iRole);
	virtual unsigned getNbFields();
	virtual QString getClassName(){return "Problem";};

	// Execution
	/**
	* Description Launch problem resolution.
	* This function should be reimplemented for each kind of problem.
	*/
	virtual void launch(ProblemConfig config) = 0;

        /**
        * Description precomputation function : should be launched at beginning
        * of launch function
        */
        //virtual void precompute();

        /**
        * Description Check if problem parameters, and inputs are correct.
        * For example, check if min<max.
        * pure virtual functions : all classes inheriting Problem should have a check function.
        * @param error : contains list of errors detected.
        */
        virtual bool checkBeforeComp(QString & error) = 0;



	// Managment functions
	/**
	* Description Save problem information in XML form.
	*/
	virtual QDomElement toXMLData(QDomDocument & doc) = 0;
	virtual void store(QString destFolder, QString tempDir);
	void rename(QString name,bool changeFolder);


	// Get functions
	int getiCurAlgo();
	MyAlgorithm* getCurAlgo();
	AlgoConfig* getCurAlgoConfig();
	QStringList getAlgoNames();
	bool isSolved();
	QString saveFolder();
	QString saveFileName();
	QString entireSavePath();
	int num(){return _num;};
	ProblemType type(){return _type;};
	Result* result() const {return _result;};
	Project* project(){return _project;};
	ModReader* modReader(){return _modReader;};
	ModPlusCtrl* modPlusCtrl(){return _modPlusCtrl;};
	ModClass* rootModClass(){return _rootModClass;};

	// Set functions
	void setName(QString);
	void setNum(int);
	void setType(ProblemType);
	void setSaveFolder(QString);
	void setDefaultSaveFileName();
	void setEntireSavePath(QString);
	void setProject(Project*);
	void setResult(Result*);
	void setiCurAlgo(int);
		

public slots:
	void openFolder();
	void clearResult();

signals:
	void algoConfigsChanged();
	void finished(Problem*);
	void newProgress(float);
	void newProgress(float,int,int);
	void begun(Problem*);
	void renamed(QString);
	void sentInfo(Info);
};





#endif
