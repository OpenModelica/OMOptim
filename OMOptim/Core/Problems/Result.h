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

 	@file Result.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
/*!
 * \file Results.h
 * \brief File containing Results class description.
 * \author Hubert Thieriot (CEP-Armines)
 * \version 0.1
 */

#if !defined(_RESULT_H)
#define _RESULT_H

#include "ModModelPlus.h"
#include "ModPlusCtrl.h"
#include "Problem.h"
#include "MOVector.h"
#include "QtCore/QTime"
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include "Info.h"
#include <QtXml/QDomDocument>
#include "MyAlgorithm.h"

class Problem;

class Result: public MOItem
{
	Q_OBJECT
	
	/*! \class Result
   *
   *  All problems should get a personnalized result class derived from this one.
   */

public :
	QTime _computationTime;
	QTime _hour;
	QDate _date;

protected :
	bool _success;

	ModModelPlus* _modModelPlus;
	ModReader* _modReader;
	ModPlusCtrl* _modPlusReader;
	Project* _project;
	Problem* _problem;
	MyAlgorithm *_algo;
	QStringList _filesToCopyNames;

public:
	Result();
	Result(Project*, ModModelPlus*, Problem*,ModReader*,ModPlusCtrl*);
	Result(const Result &r);
	~Result(void);

	virtual int problemType()=0;
	virtual QDomElement toXMLData(QDomDocument &)=0;


	virtual QString getFieldName(int iField,int iRole);
	virtual unsigned getNbFields();

	QStringList filesToCopyNames();
	Project* project();
	ModModelPlus* modModelPlus();
	ModModel* modModel();
	Problem* problem();
	bool isSuccess();


	void setName(QString);
	void setModModelPlus(ModModelPlus*);
	void setProject(Project*);

	void setProblem(Problem*);
	void setAlgo(MyAlgorithm*);
	void setSuccess(bool);

	
};



#endif
