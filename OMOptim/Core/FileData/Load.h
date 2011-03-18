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

 	@file Load.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#ifndef LOAD_H
#define LOAD_H

#include <stdio.h>
#include <QtCore/QObject>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>
#include <QtXml/QDomDocument>
#include "QtCore/QSettings"

#include "Result.h"
#include "OneSimResult.h"
#include "OptimResult.h"
#include "Project.h"
#include "Problem.h"
#include "OneSimulation.h"
#include "Optimization.h"
#include "MOVector.h"
#include "ModClass.h"
#include "CSV.h"
#include "Software.h"
#include "MOThreads.h"
#include "Info.h"
#include "Dymola.h"
//#include "XML.h"

#ifdef USEEI
	#include "ProblemTarget.h"
	#include "EITargetResult.h"
#endif

class OptimResult;

class Load : public QObject
{
	Q_OBJECT
public:
	Load(void);
	~Load(void);

	//static Result* newResultFromFile(QString,Project*);
	static Problem* newSolvedProblemFromFile(QString,Project*);
	static Problem* newProblemFromFile(QString,Project*);
	
	static bool loadProject(QString,Project*);
	static bool loadModModelPlus(Project*, QString mmoFilePath);
	static void loadOptimValuesFromFrontFile(OptimResult*,QString);

	Project* project;


private:
	
	
	static Problem* newOneSimulationFromFile(QString,Project*);
	static Problem* newOptimizationFromFile(QString,Project*);
	static Problem* newOneSimulationSolvedFromFile(QString,Project*);
	static Problem* newOptimizationSolvedFromFile(QString,Project*);
	
#ifdef USEEI
	static Problem* newProblemTargetFromFile(QString,Project*);
	static Problem* newProblemTargetSolvedFromFile(QString,Project*);
#endif

signals:
	void sendInfo(Info*);
	
};

#endif
