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
        @version

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

ModModelPlus::ModModelPlus( Project* project,ModModel* modModel)
{
    _project = project;
    _moomc = _project->moomc();
    _modModel = modModel;
    _variables = new Variables(this);
    _connections = new ModelicaConnections(_project->modClassTree());
    _modifiers = new MOVector<ModelicaModifier>(true);

    _variablesRead = false;
    _modifiersRead = false;
    _connectionsRead = false;

    _name = modModel->name(Modelica::FULL);


    connect(_variables,SIGNAL(modified()),this,SIGNAL(variablesUpdated()));


    // read function
    // readAll();
}

ModModelPlus::~ModModelPlus()
{
    _connections->deleteLater();
    _modifiers->deleteLater();
    _variables->deleteLater();
}

/**
  * @brief Clear ModModelPlus content
  * Clear ModModelPlus content : includnig variables, connections
  */
void ModModelPlus::clear()
{
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
    _project->reloadModModel(_modModel);
}
void ModModelPlus::setMmoFilePath(QString filePath)
{
    _mmoFilePath = filePath;
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

void ModModelPlus::setModModel(ModModel* modModel)
{
    _modModel = modModel;
}

void ModModelPlus::addMoDependency(const QString & dep)
{
    _moDependencies.push_back(dep);
}

void ModModelPlus::addMoDependencies(const QStringList & deps)
{
    _moDependencies.append(deps);
}

void ModModelPlus::setMoDependencies(const QStringList & deps)
{
    _moDependencies.clear();
    _moDependencies.append(deps);
}

QStringList ModModelPlus::moDependencies() const
{
    return _moDependencies;
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
* Returns pointer to variables.
*/
Variables * ModModelPlus::variables()
{
//    if(!_variablesRead && readIfNot)
//        readVariables();

    return _variables;
}

/**
* Returns a vector containg the variables concerning a child element
* @Param _element : child element
*/
Variables* ModModelPlus::variables(ModClass* element)
{
    Variables* elVars = new Variables(this);
    QString elName = element->name();
    QString curElName;
    Variable* curVar;

    for(int i=0;i<_variables->size();i++)
    {
        curVar = _variables->at(i);
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

bool ModModelPlus::readAll(ModPlusCtrl *ctrl)
{
    bool varOk = false;
    bool connOk = false;

    varOk = readVariables(ctrl);
    connOk = readConnections();

    return varOk && connOk;
}

void ModModelPlus::loadDependencies()
{
    for(int i=0;i<_moDependencies.size();i++)
        _moomc->loadFile(_moDependencies.at(i));
}

bool ModModelPlus::isCompiled(ModPlusCtrl* ctrl)
{
    return ctrl->isCompiled();
}

bool ModModelPlus::compile(ModPlusCtrl* ctrl)
{
    return ctrl->compile(moDependencies());
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

bool ModModelPlus::readVariables(ModPlusCtrl* ctrl)
{
    _variablesRead = ctrl->readInitialVariables(_variables);
    return _variablesRead;
}

 bool ModModelPlus::variablesRead() const
 {
     return _variablesRead;
 }

bool ModModelPlus::readConnections(ModClass* element,bool includeChildren)
{
    QString className = ((ModComponent*)element)->getModClassName();
    QString name = element->name(Modelica::FULL);

    QStringList aNames, bNames;
    QString aName,bName;

    QMap<QString,QString> conns = _moomc->getConnections(className);

    for(int i=0;i<conns.keys().size();i++)
    {
        aName = _name+"."+conns.keys().at(i);
        bName = _name+"."+conns.value(conns.keys().at(i));
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

void ModModelPlus::openDependenciesDlg()
{
    DlgSelectFiles dlgSelectFiles(_moDependencies,"*.mo");
    dlgSelectFiles.exec();
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
    bool deleteOk = _moomc->deleteConnections(blockSub->_orgPorts,blockSub->_orgConnectedComps,modModelName());
    QString modelName = modModelName();
    QString shortOrg = blockSub->_orgComponent;
    shortOrg = shortOrg.remove(modelName+".");
    QString shortSub = blockSub->_subComponent;
    shortSub = shortSub.remove(modelName+".");
    ModClass* orgClass = _project->modClassTree()->findInDescendants(blockSub->_orgComponent,_modModel);
    if(!orgClass)
    {
        QString msg;
        msg.sprintf("Could not apply component substitution : component %s not found",
                    blockSub->_orgComponent.utf16());
        infoSender.send(Info(msg,ListInfo::WARNING2));
        return false;
    }

    ModComponent* orgComp = NULL;
    if(orgClass->getClassRestr()==Modelica::COMPONENT)
        orgComp = (ModComponent*)orgClass;

    if(orgComp)
    {
        // first save annotation and modifiers
        QString annotation ;//= _moomc->getAnnotation(blockSub->orgComponent,blockSub->_modModel->name());
        QStringList modifiersNames = _moomc->getComponentModifierNames(blockSub->_orgComponent);
        QStringList modifiersValues;
        for(int i=0;i<modifiersNames.size();i++)
            modifiersValues.push_back(_moomc->getComponentModifierValue(modelName,shortOrg,modifiersNames.at(i)));

        // delete org component
        _moomc->deleteComponent(blockSub->_orgComponent);

        // create new component
        QString newCompName = blockSub->_orgComponent;
        //keep same name will avoid problem if objective or optimized variables are in component

        _moomc->addComponent(newCompName,blockSub->_subComponent,modModelName(),annotation);

        // specify modifiers equal to orgComponent
        _moomc->setComponentModifiers(newCompName,modModelName(),modifiersNames,modifiersValues);

        // connect it
        QStringList newCompPorts;
        for(int i=0;i<blockSub->_subPorts.size();i++)
        {
            newCompPorts.push_back(newCompName + "." + blockSub->_subPorts.at(i).section(".",-1,-1));
        }

        _moomc->addConnections(newCompPorts,blockSub->_subConnectedComps);
    }

    _moomc->save(modModelName());

    return true;
}


