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

 	@file ModModelPlus.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */

#include "ModModelPlus.h"
#include "Dymola.h"
#include "Variable.h"
#include <iostream> 
#include <vector>
#include <QtCore/QObject>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>
#include <QtCore/QStringList>
#include "MOomc.h"
#include "LowTools.h"
#include "HighTools.h"
#include "Load.h"
#include "Save.h"
#include "ModPlusCtrl.h"
#include "ModPlusOMCtrl.h"
#include "ModPlusDymolaCtrl.h"
#include "MOParametersDlg.h"


using std::vector;



ModModelPlus::ModModelPlus(MOomc* moomc, Project* project,ModClassTree* modClassTree,ModModel* modModel_)
{
	_project = project;
	_moomc = moomc;
        _modClassTree = modClassTree;
	_modModel = modModel_;
	_variables = new MOVector<Variable>;
        _connections = new ModelicaConnections(modClassTree);
	_modifiers = new MOVector<ModelicaModifier>;

	_isDefined = false;
	_elementsRead = false;
	_variablesRead = false;
	_modifiersRead = false;
	_connectionsRead = false;

	_name = modModel_->name(Modelica::FULL);
	
	// Controller
	_modPlusCtrls.insert(ModPlusCtrl::OPENMODELICA,new ModPlusOMCtrl(this,_moomc,mmoFolder(),modModel()->filePath(),modModelName()));
	_modPlusCtrls.insert(ModPlusCtrl::DYMOLA,new ModPlusDymolaCtrl(this,_moomc,mmoFolder(),modModel()->filePath(),modModelName()));

#if DEFAULTSIMULATOR==0
	setCtrlType(ModPlusCtrl::OPENMODELICA);
#else
	setCtrlType(ModPlusCtrl::DYMOLA);
#endif
	// read function
	// readAll();

	//Connect signals and slots for error and progress management
	//connect(this, SIGNAL(sendInfo(Info*)),project, SIGNAL(sendInfo(Info*)));
	connect(this, SIGNAL(sendProgress(float)),_project, SIGNAL(sendProgress(float)));
	connect(this, SIGNAL(sendProgress(float,int,int)),_project, SIGNAL(sendProgress(float,int,int)));
}

ModModelPlus::~ModModelPlus()
{
	delete _connections;
	delete _modifiers;
	delete _variables;
}



void ModModelPlus::clear()
{
	_isDefined = false;
	_elementsRead = false;
	_variablesRead = false;
	_modifiersRead = false;
	_connectionsRead = false;


	_name.clear();
	_mmoFilePath.clear();

	//connections
	_connections->clear();
	_modifiers->clear();
	_variables->clear();
}

void ModModelPlus::save()
{
	Save::saveModModelPlus(this);
	emit saved();
}


void ModModelPlus::reloadModel()
{
	_modModel->reloadInOMC();
}

void ModModelPlus::setMmoFilePath(QString filePath)
{
	_mmoFilePath = filePath;
	_isDefined = true;
	
	for(int i=0;i<_modPlusCtrls.values().size();i++)
 		_modPlusCtrls.values().at(i)->setMmoFolder(mmoFolder());
}

QString ModModelPlus::mmoFilePath()
{
	return _mmoFilePath;
}

QString ModModelPlus::mmoFileName()
{
	QFileInfo fileInfo(_mmoFilePath);
	return fileInfo.fileName();
}

QString ModModelPlus::mmoFolder()
{
	QFileInfo fileInfo(_mmoFilePath);
	return fileInfo.absolutePath();
}

QStringList ModModelPlus::neededFilesNames()
{
	return _neededFiles;
}

QStringList ModModelPlus::neededFoldersNames()
{
	return _neededFolders;
}



bool ModModelPlus::isDefined()
{
	return _isDefined;
}

ModPlusCtrl* ModModelPlus::ctrl()
{
	ModPlusCtrl* returnedCtrl = _modPlusCtrls.value(ctrlType());
	return returnedCtrl;
}

QMap<ModPlusCtrl::Type,ModPlusCtrl*>* ModModelPlus::ctrls()
{
	return &_modPlusCtrls;
}

ModPlusCtrl::Type ModModelPlus::ctrlType()
{
	return _ctrlType;
}

void ModModelPlus::setModModel(ModModel* modModel)
{
	_modModel = modModel;
}

//************************
//  Variables
//************************
void ModModelPlus::addVariable(Variable* var)
{
	// add item in variables vector
	_variables->addItem(var);
}

/**
* Description Returns pointer to variables.
*/
MOVector<Variable> * ModModelPlus::variables(bool readIfNot)
{
        if(!_variablesRead && readIfNot)
		readVariables();
	
	return _variables;
}

/**
* Description Return a vector containg the variables concerning a child element
* @Param _element : child element
*/
MOVector<Variable>* ModModelPlus::variables(ModClass* element)
{
	MOVector<Variable>* elVars = new MOVector<Variable>;
	QString elName = element->name();
	QString curElName;
	Variable* curVar;

	for(int i=0;i<_variables->items.size();i++)
	{
		curVar = _variables->items.at(i);
		curElName = curVar->name().section(".",0,-2);

		if(QString::compare(curElName,elName,Qt::CaseInsensitive)==0)
			elVars->addItem(curVar);
	}
	return elVars;
}



QStringList ModModelPlus::otherFiles()
{
	return _otherFiles;
}

QString ModModelPlus::infos()
{
	return _infos;
}

ModModel* ModModelPlus::modModel()
{
	return _modModel;
}
void ModModelPlus::clearOtherFiles()
{
	_otherFiles.clear();
}
void ModModelPlus::setOtherFiles(QStringList otherFiles)
{
	int i=0;
	// removing empty files and caracters
	while(i <otherFiles.size())
	{
		if(otherFiles[i].replace(" ","").isEmpty())
			otherFiles.removeAt(i);
		else
			i++;
	}
	_otherFiles= otherFiles;
}
void ModModelPlus::setInfos(QString infos)
{
	_infos = infos;
}

bool ModModelPlus::readAll()
{
	bool varOk = false;
	bool connOk = false;

	varOk = readVariables();
	connOk = readConnections();

	_isDefined = varOk&&connOk;
	return _isDefined;
}

bool ModModelPlus::isCompiled()
{
	return ctrl()->isCompiled();
}

bool ModModelPlus::compile()
{
	return ctrl()->compile();
}

void ModModelPlus::addConnection(ModClass* a, ModClass* b)
{
	ModelicaConnection* newConnection = new ModelicaConnection(a->name(Modelica::FULL),b->name(Modelica::FULL));
	_connections->addItem(newConnection);
}

void ModModelPlus::addConnection(QString aName,QString bName)
{
	ModelicaConnection* newConnection = new ModelicaConnection(aName,bName);
	_connections->addItem(newConnection);
}

ModelicaConnections* ModModelPlus::connections()
{
	if(!_connectionsRead)
		readConnections();

	return _connections;
}

bool ModModelPlus::readVariables()
{
	_variablesRead = ctrl()->readInitialVariables(_variables);
	return _variablesRead;
}

bool ModModelPlus::readConnections(ModClass* element,bool includeChildren)
{
	QString className = ((ModComponent*)element)->getModClassName();
	QString name = element->name(Modelica::FULL);

	QStringList aNames, bNames;
	QString aName,bName;

	_moomc->getConnections(className,aNames,bNames);

	for(int i=0;i<aNames.size();i++)
	{
		aName = _name+"."+aNames.at(i);
		bName = _name+"."+bNames.at(i);
		addConnection(aName,bName);
	}

	if(includeChildren)
	{
		// call it for all children packages
		int nbChildren = element->packageChildCount();
		for(int iChild = 0; iChild<nbChildren; iChild++)
			readConnections(element->packageChild(iChild),true);

		// call it for all children models
		nbChildren = element->modelChildCount();
		for(int iChild = 0; iChild<nbChildren; iChild++)
			readConnections(element->modelChild(iChild),true);


		// call it for all children components
		nbChildren = element->compChildCount();
		for(int iChild = 0; iChild<nbChildren; iChild++)
			readConnections(element->compChild(iChild),true);
	}
	return true;
}

bool ModModelPlus::readConnections()
{
	_connections->clear();

	sendInfo (new Info(ListInfo::BEGINREADINGCONNECTIONS));
	_connectionsRead = readConnections(_modModel,true);
	sendInfo (new Info(ListInfo::READCONNECTIONSSUCCESS));

	return _connectionsRead;
}

void ModModelPlus::openMoFolder()
{
	//LowTools::openFolder(modModel()->moFolder());
}
void ModModelPlus::openMmoFolder()
{
	LowTools::openFolder(mmoFolder());
}

void ModModelPlus::openMoFile()
{
	//LowTools::openFile(moFilePath());
}

void ModModelPlus::openParametersDlg()
{
	MOParametersDlg *parametersDlg = new MOParametersDlg(ctrl()->parameters());
	parametersDlg->exec();
}

QString ModModelPlus::modModelName()
 {
	return _modModel->name(Modelica::FULL);
}

//vector<ModModelPlusicaModifier*>* MOomc::getComponentModifiers(QString componentName,ModClass* component)
//{
//
//
//	QStringList modNames = getComponentModifierNames(componentName);
//
//	vector<ModModelPlusicaModifier*> *_compModifiers = new vector<ModModelPlusicaModifier*>;
//	_compModifiers->reserve(modNames.size());
//
//	ModModelPlusicaModifier* curMod;
//
//	for(int iM=0;iM<modNames.size();iM++)
//	{
//		curMod = new ModModelPlusicaModifier();
//		curMod->setName(modNames.at(iM));
//		curMod->setValue(getComponentModifierValue(componentName,modNames.at(iM)));
//		curMod->setComponent(component);
//		_compModifiers->push_back(curMod);
//	}
//
//	return _compModifiers;
//
//}




bool ModModelPlus::applyBlockSub(BlockSubstitution *blockSub,bool compile)
{

	// delete org connections
	_moomc->deleteConnections(blockSub->orgPorts,blockSub->orgConnectedComps,modModelName());
	
        ModClass* orgClass = _modClassTree->findInDescendants(blockSub->orgComponent,_modModel);
        if(!orgClass)
        {
            QString msg;
            msg.sprintf("Could not apply component substitution : component %s not found",
                        blockSub->orgComponent.utf16());
            infoSender.send(Info(msg,ListInfo::WARNING2));
            return false;
        }

	ModComponent* orgComp = NULL;
	if(orgClass->getClassRestr()==Modelica::COMPONENT)
		orgComp = (ModComponent*)orgClass;

	if(orgComp)
	{
		// first save annotation and modifiers
		QString annotation = _moomc->getAnnotation(blockSub->orgComponent,orgComp->getModClassName());
		QStringList modifiersNames = _moomc->getComponentModifierNames(blockSub->orgComponent);
		QStringList modifiersValues;
		for(int i=0;i<modifiersNames.size();i++)
			modifiersValues.push_back(_moomc->getComponentModifierValue(blockSub->orgComponent,modifiersNames.at(i)));
		
		// delete org component
		_moomc->deleteComponent(blockSub->orgComponent);
	
		// create new component
		QString newCompName = blockSub->orgComponent;
		//keep same name will avoid problem if objective or optimized variables are in component 

		_moomc->addComponent(newCompName,blockSub->subComponent,modModelName(),annotation);

		// specify modifiers equal to orgComponent
		_moomc->setComponentModifiers(newCompName,modModelName(),modifiersNames,modifiersValues);
	
		// connect it
		QStringList newCompPorts;
		for(int i=0;i<blockSub->subPorts.size();i++)
		{
			newCompPorts.push_back(newCompName + "." + blockSub->subPorts.at(i).section(".",-1,-1));
		}

		_moomc->addConnections(newCompPorts,blockSub->subConnectedComps);
	}

	_moomc->save(modModelName());

	return true;
}

void ModModelPlus::setCtrlOpenModelica()
{
	this->setCtrlType(ModPlusCtrl::OPENMODELICA);
}
void ModModelPlus::setCtrlDymola()
{
	this->setCtrlType(ModPlusCtrl::DYMOLA);
}

void ModModelPlus::setCtrlType(ModPlusCtrl::Type type)
{
	_ctrlType = type;
}
