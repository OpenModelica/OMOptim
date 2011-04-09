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

 	@file ModReader.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "ModReader.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QMutex>


ModReader::ModReader(MOomc *_oms)
{
	moomc = _oms;
}



void ModReader::readMoFile(ModClass* rootClass,QString filePath,QMap<ModModel*,ModModelPlus*> & mapModelPlus, bool forceLoad)
{
	// Read it in moomc
	QFile file(filePath);
	if( !file.open( QIODevice::ReadOnly ) )
	{
		//infoSender.send( Info(ListInfo::FILENOTEXISTS,filePath));
		return ;
	}
	file.close();

	// loading file in moomc
	QString error;
	bool loadOk;

	// Load file
	moomc->loadModel(filePath,forceLoad,loadOk,error);
	QStringList newClasses = moomc->getClassesOfFile(filePath);

	////Refresh
	//if(refreshAfter)
	//	refresh(rootClass,mapModelPlus);
}

void ModReader::readMoFiles(ModClass* rootClass,QStringList filePaths,QMap<ModModel*,ModModelPlus*> & mapModelPlus, bool forceLoad)
{
	for(int i=0;i<filePaths.size();i++)
		readMoFile(rootClass,filePaths.at(i),mapModelPlus,forceLoad);
	////Refresh
	//if(refreshAfter)
	//	refresh(rootClass,mapModelPlus);
}
void ModReader::addModClass(ModClass* rootClass,QString className,QString filePath)
{
	Modelica::ClassRestr restr = moomc->getClassRestriction(className);

		ModClass* newClass = NULL;
		switch(restr)
		{
		case Modelica::PACKAGE :
			newClass = new ModPackage(moomc,rootClass,className,filePath);
			break;
		case Modelica::MODEL :
			newClass = new ModModel(moomc,rootClass,className,filePath);
			break;
		}
		if(newClass)
		{
			rootClass->addChild(newClass);	
			readFromOmcV3(newClass,rootClass,getDepthMax());
		}
}

int ModReader::getDepthMax()
{
	int depthMax = MOSettings::getValue("DepthReadWhileLoadingModModel").toInt();
	if(depthMax==-1)
			depthMax = 100000;
	return depthMax;
}

void ModReader::readFromOmc(ModClass* parent,int depthMax,int curDepth)
{
	if(parent->_readMutex.tryLock())
	{
		if((curDepth<depthMax)&&!parent->childrenReaden())
		{
			ModClass* newElement;

			bool readPackages = (parent->getClassRestr()==Modelica::PACKAGE);
			bool readModels = (parent->getClassRestr()==Modelica::PACKAGE);
			bool readComponents = (parent->getClassRestr()==Modelica::MODEL);

			QString fullParentName = parent->name(Modelica::FULL);

			// read packages
			if(readPackages)
			{
				QStringList packageNames = moomc->getPackages(fullParentName);
				for(int i=0;i<packageNames.size();i++)
				{
					newElement = new ModPackage(moomc,parent,packageNames.at(i));
					if(parent->addChild(newElement))
						readFromOmc(newElement,depthMax,curDepth+1);
				}
			}

			// read models
			if(readModels)
			{
				QStringList modelNames = moomc->getModels(fullParentName);
				for(int i=0;i<modelNames.size();i++)
				{
					newElement = new ModModel(moomc,parent,modelNames.at(i));
					if(parent->addChild(newElement))
						readFromOmc(newElement,depthMax,curDepth+1);
				}
			}

			//read components
			if(readComponents)
			{
				QStringList compNames;
				QStringList compClasses;
				moomc->getContainedComponents(fullParentName,compNames,compClasses);
				for(int i=0;i<compNames.size();i++)
				{
					newElement = new ModComponent(moomc,parent,compNames.at(i),compClasses.at(i));
					if(parent->addChild(newElement))
						readFromOmc(newElement,depthMax,curDepth+1);
				}
			}
			parent->setChildrenReaden(true);

			parent->emitModified();

			emit finishedRead();
		}
		parent->_readMutex.unlock();
	}
}


void ModReader::readFromOmcV2(ModClass* parent,int depthMax,QString direction,int curDepth)
{
	if(parent->_readMutex.tryLock())
	{
		if((curDepth<=depthMax)&&!parent->childrenReaden())
		{
			QString childrenDirection = direction.section(".",curDepth+1,curDepth+1);
			QString fullParentClass = parent->getModClassName();

			ModClass* newElement;

			//read packages, models and components
			QStringList packagesClasses,modelsClasses,compsNames,compsClasses;
			moomc->readElementInfos(fullParentClass,packagesClasses,modelsClasses,compsNames,compsClasses);

			// get inherited components
			QStringList inheritedCompNames,inheritedCompClasses;
			moomc->getInheritedComponents(fullParentClass,inheritedCompNames,inheritedCompClasses);
			compsNames << inheritedCompNames;
			compsClasses << inheritedCompClasses;

			// adding child packages and read them
			for(int i=0;i<packagesClasses.size();i++)
			{
				newElement = new ModPackage(moomc,parent,packagesClasses.at(i));
				if(parent->addChild(newElement))
					if((childrenDirection=="") || (childrenDirection==packagesClasses.at(i)))
						readFromOmcV2(newElement,depthMax,direction,curDepth+1);
			}

			// adding child models and read them
			for(int i=0;i<modelsClasses.size();i++)
			{
				newElement = new ModModel(moomc,parent,modelsClasses.at(i));
				if(parent->addChild(newElement))
					if((childrenDirection=="") || (childrenDirection==modelsClasses.at(i)))
						readFromOmcV2(newElement,depthMax,direction,curDepth+1);
			}

			// adding child components and read them
			for(int i=0;i<compsNames.size();i++)
			{
				newElement = new ModComponent(moomc,parent,compsNames.at(i),compsClasses.at(i));
				if(parent->addChild(newElement))
					if((childrenDirection=="") || (childrenDirection==compsClasses.at(i)))
						readFromOmcV2(newElement,depthMax,direction,curDepth+1);
			}

			parent->setChildrenReaden(true);
			parent->emitModified();

			emit finishedRead();
		}
		parent->_readMutex.unlock();
	}
}

void ModReader::readFromOmcV3(ModClass* parent,ModClass *rootClass,int depthMax,QString direction,int curDepth)
{
	if(parent->_readMutex.tryLock())
	{
		if((curDepth<=depthMax)&&!parent->childrenReaden())
		{
			QString childrenDirection = direction.section(".",curDepth+1,curDepth+1);
			QString fullParentClass = parent->getModClassName();
			QString fullParentName = parent->name(Modelica::FULL);

			ModClass* newElement;

			//read packages, models and components
			QStringList packagesClasses,modelsClasses,compsNames,compsClasses;
			bool foundInLib = false;


			ModClass* _corrLibComp = findInDescendants(rootClass,fullParentName);
			if(_corrLibComp)
			{
				// If class is already loaded in package, use its information
				foundInLib = true;
				childrenInfos(_corrLibComp,packagesClasses,modelsClasses,compsNames,compsClasses);
			}
			else
			{
				// Call parent->moomc
				moomc->readElementInfos(fullParentClass,packagesClasses,modelsClasses,compsNames,compsClasses);
				// get inherited components
				QStringList inheritedCompNames,inheritedCompClasses;
				moomc->getInheritedComponents(fullParentClass,inheritedCompNames,inheritedCompClasses);
				compsNames << inheritedCompNames;
				compsClasses << inheritedCompClasses;
			}


			// adding child packages and read them
			for(int i=0;i<packagesClasses.size();i++)
			{
				newElement = new ModPackage(moomc,parent,packagesClasses.at(i));
				if(parent->addChild(newElement))
					if((childrenDirection=="") || (childrenDirection==packagesClasses.at(i)))
						readFromOmcV3(newElement,rootClass,depthMax,direction,curDepth+1);
			}

			// adding child models and read them
			for(int i=0;i<modelsClasses.size();i++)
			{
				newElement = new ModModel(moomc,parent,modelsClasses.at(i));
				if(parent->addChild(newElement))
					if((childrenDirection=="") || (childrenDirection==modelsClasses.at(i)))
						readFromOmcV3(newElement,rootClass,depthMax,direction,curDepth+1);
			}

			// adding child components and read them
			for(int i=0;i<compsNames.size();i++)
			{
				newElement = new ModComponent(moomc,parent,compsNames.at(i),compsClasses.at(i));
				if(parent->addChild(newElement))
					if((childrenDirection=="") || (childrenDirection==compsClasses.at(i)))
						readFromOmcV3(newElement,rootClass,depthMax,direction,curDepth+1);
			}

			parent->setChildrenReaden(true);

			parent->emitModified();

			emit finishedRead();
		}
		parent->_readMutex.unlock();
	}
}




void ModReader::readFromOmcV4(ModClass* parent,ModClass *rootClass,int depthMax,QString direction,int curDepth)
{
        if(parent->_readMutex.tryLock())
        {
                if((curDepth<=depthMax)&&!parent->childrenReaden())
                {
                        QString childrenDirection = direction.section(".",curDepth+1,curDepth+1);
                        QString fullParentClass = parent->getModClassName();
                        QString fullParentName = parent->name(Modelica::FULL);

                        ModClass* newElement;

                        //read packages, models and components
                        QStringList packagesClasses,modelsClasses,compsNames,compsClasses;
                        bool foundInLib = false;


                        ModClass* _corrLibComp = findInDescendants(rootClass,fullParentName);
                        if(_corrLibComp)
                        {
                                // If class is already loaded in package, use its information
                                foundInLib = true;
                                childrenInfos(_corrLibComp,packagesClasses,modelsClasses,compsNames,compsClasses);
                        }
                        else
                        {
                                // Call parent->moomc
                                moomc->readElementInfos(fullParentClass,packagesClasses,modelsClasses,compsNames,compsClasses);
                                // get inherited components
                                QStringList inheritedCompNames,inheritedCompClasses;
                                moomc->getInheritedComponents(fullParentClass,inheritedCompNames,inheritedCompClasses);
                                compsNames << inheritedCompNames;
                                compsClasses << inheritedCompClasses;
                        }


                        // adding child packages and read them
                        for(int i=0;i<packagesClasses.size();i++)
                        {
                                newElement = new ModPackage(moomc,parent,packagesClasses.at(i));
                                if(parent->addChild(newElement))
                                        if((childrenDirection=="") || (childrenDirection==packagesClasses.at(i)))
                                                readFromOmcV3(newElement,rootClass,depthMax,direction,curDepth+1);
                        }

                        // adding child models and read them
                        for(int i=0;i<modelsClasses.size();i++)
                        {
                                newElement = new ModModel(moomc,parent,modelsClasses.at(i));
                                if(parent->addChild(newElement))
                                        if((childrenDirection=="") || (childrenDirection==modelsClasses.at(i)))
                                                readFromOmcV3(newElement,rootClass,depthMax,direction,curDepth+1);
                        }

                        // adding child components and read them
                        for(int i=0;i<compsNames.size();i++)
                        {
                                newElement = new ModComponent(moomc,parent,compsNames.at(i),compsClasses.at(i));
                                if(parent->addChild(newElement))
                                        if((childrenDirection=="") || (childrenDirection==compsClasses.at(i)))
                                                readFromOmcV3(newElement,rootClass,depthMax,direction,curDepth+1);
                        }

                        parent->setChildrenReaden(true);

                        parent->emitModified();

                        emit finishedRead();
                }
                parent->_readMutex.unlock();
        }
}


bool ModReader::isInDescendants(ModClass* parent, QString fullName)
{
	ModClass* foundClass = findInDescendants(parent,fullName);
	return (bool)(foundClass);
}

ModClass* ModReader::findInDescendants(ModClass* parent,QString fullName)
{
	ModClass* curChild;
	QString curFullName = parent->name(Modelica::FULL);

	int curDepth = parent->depth();
	int lookingDepth = fullName.split(".").size()-1;

	// check if it is this component
	if(curFullName == fullName)
		return parent;

	//first check name compatibility
	if(fullName.indexOf(curFullName)!=0)
		return NULL;

	// then check children are readen
	if(!parent->childrenReaden())
	{
		// if not, check in its direction
		readFromOmcV2(parent,lookingDepth,fullName,curDepth);
	}

	//get child name to visit
	QString childShortName = fullName.section(".",curDepth,curDepth);

	// looking in children
	for(int iChild=0;iChild<parent->childCount();iChild++)
	{
		curChild = parent->child(iChild);
		if(curChild->name(Modelica::SHORT)==childShortName)
			return findInDescendants(curChild,fullName);
	}
	return NULL;
}



QList<ModClass*> ModReader::findCompOfClassInDescendants(ModClass* parent,QString _className)
{
	ModClass* curComponent;
	QList<ModClass*> curComponents;
	int iChild;

	int nbCompChild = parent->compChildCount();
	int nbModelChild = parent->modelChildCount();
	int nbPackageChild = parent->packageChildCount();

	// looking if one child corresponds
	for(iChild=0;iChild<nbCompChild;iChild++)
	{
		curComponent = parent->compChild(iChild);
		if(curComponent->getFieldValue(ModComponent::MODCLASSNAME)==_className)
			curComponents.push_back(curComponent);
	}

	//look deeper in components children
	iChild=0;
	while(iChild<nbCompChild)
	{
		curComponents <<  findCompOfClassInDescendants(parent->compChild(iChild),_className);
		iChild++;
	}

	//look deeper in models children
	iChild=0;
	while(iChild<nbModelChild)
	{
		curComponents <<   findCompOfClassInDescendants(parent->modelChild(iChild),_className);
		iChild++;
	}

	//look deeper in packages children
	iChild=0;
	while(iChild<nbPackageChild)
	{
		curComponents <<  findCompOfClassInDescendants(parent->packageChild(iChild),_className);
		iChild++;
	}

	return curComponents;
}

/**
* \brief
* Return first ModModel in item parents (including himself).
* This function can for instance be used to know which model compile
* in order to update item value.
*/
ModModel* ModReader::modelOf(ModClass* item)
{
    if(item==NULL)
        return NULL;

    if(item->getClassRestr()==Modelica::MODEL)
        return dynamic_cast<ModModel*>(item);
    else
        return modelOf(item->parent());
}

/**
* \brief
* Return first ModModel in item parents (including himself).
* This function can for instance be used to know which model compile
* in order to update item value.
* @param itemName : full name of item whom who are looking parent model
* @param rootModClass : root of the ModClassTree where we look for item and its parent model
*/
ModModel* ModReader::modelOf(QString itemName,ModClass* rootModClass)
{
    ModClass* item = findInDescendants(rootModClass,itemName);
    return modelOf(item);
}


void ModReader::childrenInfos(ModClass* parent,QStringList &packagesClasses,QStringList &modelsClasses,QStringList &compsNames,QStringList &compsClasses)
{
	// read children if necessary
	if(!parent->childrenReaden())
	{
		int curDepth = parent->depth();
		readFromOmcV2(parent,curDepth+1,QString(),curDepth);
	}

	int nbCompChild = parent->compChildCount();
	int nbModelChild = parent->modelChildCount();
	int nbPackageChild = parent->packageChildCount();

	//clear
	packagesClasses.clear();
	modelsClasses.clear();
	compsClasses.clear();
	compsNames.clear();

	// packages
	for(int i=0;i<nbPackageChild;i++)
		packagesClasses.push_back(parent->packageChild(i)->name(Modelica::SHORT));

	// models
	for(int i=0;i<nbModelChild;i++)
		modelsClasses.push_back(parent->modelChild(i)->name(Modelica::SHORT));

	// components
	for(int i=0;i<nbCompChild;i++)
	{
		compsClasses.push_back(parent->compChild(i)->getFieldValue(ModComponent::MODCLASSNAME).toString());
		compsNames.push_back(parent->compChild(i)->name(Modelica::SHORT));
	}
}


// get all ports of component parent (not only those connected)
QList<ModClass*> ModReader::getPorts(ModClass* parent)
{
	QList<ModClass*> ports;

	int curDepth = parent->depth();
	if(!parent->childrenReaden())
	{
		readFromOmcV2(parent,curDepth+1,QString(),curDepth);
	}

	int nbCompChild = parent->compChildCount();
	for(int i=0; i<nbCompChild;i++)
	{
		ModComponent* curComp = (ModComponent*)parent->compChild(i);
		if(moomc->isConnector(curComp->getFieldValue(ModComponent::MODCLASSNAME).toString()))
			ports.push_back(curComp);
	}
	return ports;
}

QStringList ModReader::getPorts(ModClass* parent,Modelica::NameFormat format)
{
	QList<ModClass*> _ports = getPorts(parent);

	QStringList _portsNames;
	for(int i=0;i<_ports.size();i++)
		_portsNames.push_back(_ports.at(i)->name(format));

	return _portsNames;
}
