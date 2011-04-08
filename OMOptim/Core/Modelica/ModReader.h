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

 	@file ModReader.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#ifndef _ModReader_H
#define _ModReader_H

#include "Modelica.h"
#include "ModClass.h"
#include "ModPackage.h"
#include "ModModel.h"

#include "ModComponent.h"
#include "MOSettings.h"

class ModModelPlus;

class ModReader : public QObject
{
	Q_OBJECT

public:

	ModReader(MOomc *_oms);

	// Load functions
	void readMoFile(ModClass* rootClass,QString filePath,QMap<ModModel*,ModModelPlus*> & mapModelPlus,bool forceLoad = true);
	void readMoFiles(ModClass* rootClass,QStringList filePaths,QMap<ModModel*,ModModelPlus*> & mapModelPlus, bool forceLoad = true);
	void refresh(ModClass* rootClass,QMap<ModModel*,ModModelPlus*> & mapModelPlus);

	// Read functions
	void readFromOmc(ModClass*,int depthMax = 1000, int curDepth = 0);	//Read data and children with OMC calls
	void readFromOmcV2(ModClass*,int depthMax = 1000, QString direction ="", int curDepth = 0);	//Read data and children with OMC calls
	void readFromOmcV3(ModClass*,ModClass* _rootClass,int depthMax = 1000, QString direction ="", int curDepth = 0);	//Read data and children with OMC calls
        void readFromOmcV4(ModClass*,ModClass* _rootClass,int depthMax = 1000, QString direction ="", int curDepth = 0);	//Read data and children with OMC calls, using OMEdit version

	// Find functions
	bool isInDescendants(ModClass* parent, QString fullName);
	ModClass* findInDescendants(ModClass* parent,QString fullName);
	QList<ModClass*> findCompOfClassInDescendants(ModClass*,QString _className);
        ModModel* modelOf(ModClass* item);
        ModModel* modelOf(QString itemName,ModClass* rootModClass);


	void childrenInfos(ModClass* parent,QStringList &packagesClasses,QStringList &modelsClasses,QStringList &compsNames,QStringList &compsClasses);
	//edit functions
	void addModClass(ModClass* rootClass,QString name,QString filePath="");

	//*****************************
	//Ports
	//*****************************
	QList<ModClass*> getPorts(ModClass* parent);
	QStringList getPorts(ModClass* parent,Modelica::NameFormat format);

private :
	MOomc* moomc;
	int getDepthMax();

signals:
	void finishedRead();
};



#endif
