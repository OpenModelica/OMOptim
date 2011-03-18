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

 	@file MOSettings.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "MOSettings.h"
#include "Dymola.h"

// tricks to have a "purely" static class
QStringList MOSettings::names = QStringList();
QStringList MOSettings::descs = QStringList();
QVariantList MOSettings::defaultValues = QVariantList();
QVector<int> MOSettings::types = QVector<int>();

MOSettings MOSettings::instance = MOSettings();


MOSettings::MOSettings(void)
{
}

MOSettings::~MOSettings(void)
{
}


void MOSettings::initialize(bool eraseCurrentValues)
{

	names.clear();
	descs.clear();
	types.clear();
	defaultValues.clear();


	QSettings settings("MO", "Settings");

	//*******************************
	// MO Extra Path
	//*******************************
	names << QString("path/mainFolder");
	descs << QString("Path of installation");
	defaultValues << getenv("MOPATH");
	types.push_back(FOLDERPATH);

	//*******************************
	// Default modelica library
	//*******************************
	names << QString("path/modLib");
	descs << QString("Path of modelica library");
	QString modLibPath(getenv("MOPATH"));
	modLibPath += QDir::separator();
	modLibPath += "Modelica";
	modLibPath += QDir::separator();
	modLibPath += "Modelica.mo";
	defaultValues << modLibPath;
	types.push_back(FILEPATH);

	//*******************************
	// Default ei library
	//*******************************
	names << QString("path/eiLib");
	descs << QString("Path of EI library");
	QString eiLibPath(getenv("MOPATH"));
	eiLibPath += QDir::separator();
	eiLibPath += "Modelica";
	eiLibPath += QDir::separator();
	eiLibPath += "EI.mo";
	defaultValues << eiLibPath;
	types.push_back(FILEPATH);

	//*******************************
	// Dymola path
	//*******************************
	names << QString("path/dymolaExe");
	descs << QString("Path of Dymola executable");
	QString dymolaPath = Dymola::getExecutablePath();
	defaultValues << dymolaPath;
	types.push_back(FILEPATH);

	//*******************************
	// Quit omc at end of program
	//*******************************
	names << QString("stopOMCwhenQuit");
	descs << QString("Automatically end OMC when quitting");
	defaultValues << false;
	types.push_back(BOOL);

	//*******************************
	// Model depth read at beginning
	//*******************************
	names << QString("DepthReadWhileLoadingModModel");
	descs << QString("Requires time at beginning but less while using (-1 : entire model)");
	defaultValues << 2;
	types.push_back(INT);
	
	//*******************************
	// Gnuplot enabling
	//*******************************
	names << QString("bool/useGnuplot");
	descs << QString("Use gnuplot for moo temp results");
	defaultValues << false;
	types.push_back(BOOL);


	//*******************************
	// Gnuplot path
	//*******************************
	names << QString("path/pgnuplotEXE");
	descs << QString("Path of pgnuplot executable");
	defaultValues << "pgnuplot.exe";
	types.push_back(FILEPATH);

	//*******************************
	// Gnuplot path
	//*******************************
	names << QString("path/MILPFolder");
	descs << QString("Path of Milp models folder");
	QString MOPath(getenv("MOPATH"));
	defaultValues << MOPath+QDir::separator()+"Milp";
	types.push_back(FOLDERPATH);
	
	for(int i=0; i<names.size();i++)
	{
		if(eraseCurrentValues || settings.value(names.at(i))==QVariant())
			settings.setValue(names.at(i),defaultValues.at(i));
	}
}



void MOSettings::setValue(QString _name, QVariant _value)
{
	QSettings globalSettings("MO", "Settings");
	globalSettings.setValue(_name,_value);
}

void MOSettings::setValue(int i, QVariant _value)
{
	QSettings globalSettings("MO", "Settings");
	globalSettings.setValue(names.at(i),_value);
}
QVariant MOSettings::getValue(QString _name)
{
	QSettings globalSettings("MO", "Settings");
	return globalSettings.value(_name);
}
QVariant MOSettings::getValue(int i)
{
	QSettings globalSettings("MO", "Settings");
	return globalSettings.value(names.at(i));
}
