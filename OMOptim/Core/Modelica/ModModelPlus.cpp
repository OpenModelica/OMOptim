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

#include <QtCore>

#include "Project.h"
#include "Dialogs/DlgSelectFiles.h"
#include "ModelicaModifier.h"
#include "Variables.h"
#include "ModelicaConnections.h"
#include "BlockSubstitution.h"
#include "SaveOMOptim.h"
#include "ModPlusCtrl.h"
#include "LowTools.h"

ModModelPlus::ModModelPlus( Project* project,QString modelName)
    :ModelPlus(project,modelName)
{

    _moomc = _project->moomc();


    _variables = new Variables(true);

    _connections = new ModelicaConnections(_project->modItemsTree());

    _connectionsRead = false;
}

ModModelPlus::ModModelPlus(Project *project, const QDomElement &domRoot)
    :ModelPlus(project,domRoot)
{
    // .mo dependencies
    QDomElement cMoDeps = domRoot.firstChildElement("moDependencies");
    QStringList strMoDeps  = cMoDeps.attribute("list").split(";",QString::SkipEmptyParts);
    for (int nof=0;nof<strMoDeps.size();nof++)
    {
        this->addMoDependency(QFileInfo(strMoDeps.at(nof)));
    }

     _connections = new ModelicaConnections(_project->modItemsTree());
}

ModModelPlus::~ModModelPlus()
{
    delete _connections;
}

/**
  * @brief Clear ModModelPlus content
  * Clear ModModelPlus content : includnig variables, connections
  */
void ModModelPlus::clear()
{
    ModelPlus::clear();

    _connectionsRead = false;

    _mmoFilePath.clear();

    //connections
    _connections->clear();

}

void ModModelPlus::save()
{
    SaveOMOptim::saveModelPlus(this);
    emit saved();
}

void ModModelPlus::reloadModel()
{
    _project->reloadModModel(_modelName);
}


QFileInfo ModModelPlus::mmoFilePath()
{
    return QFileInfo(_mmoFilePath);
}

QString ModModelPlus::moFilePath()
{
    ModItem* modItem = _project->findModItem(_modelName);
    if(!modItem)
    {
        InfoSender::instance()->debug("Can't find model "+_modelName);
        return QString();
    }
    else
    {
        QString filePath = modItem->filePath();
        return filePath;
    }
}

QString ModModelPlus::mmoFileName()
{
    QFileInfo fileInfo(_mmoFilePath);
    return fileInfo.fileName();
}


void ModModelPlus::addMoDependency(const QFileInfo & dep)
{
    _moDependencies.push_back(dep);
}

void ModModelPlus::addMoDependencies(const QFileInfoList & deps)
{
    _moDependencies.append(deps);
}

void ModModelPlus::setMoDependencies(const QStringList & deps)
{
    QFileInfoList list;
    for(int i=0;i<deps.size();i++)
        list.push_back(QFileInfo(deps.at(i)));

    setMoDependencies(list);
}

void ModModelPlus::setMoDependencies(const QFileInfoList & deps)
{
    _moDependencies.clear();
    _moDependencies.append(deps);
}

QFileInfoList ModModelPlus::moDependencies() const
{
    return _moDependencies;
}

/**
* Returns a vector containg the variables concerning a child element
* @Param _element : child element
*/
/*Variables* ModModelPlus::variables(ModItem* element)
{
    Variables* elVars = new Variables(false);
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
*/



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
        _moomc->loadFile(_moDependencies.at(i).absoluteFilePath());
}

QDomElement ModModelPlus::toXmlData(QDomDocument &doc)
{
    QDomElement root = ModelPlus::toXmlData(doc) ;
    root.setTagName(ModModelPlus::className());

    // .mo dependencies
    QDomElement cMoDeps = doc.createElement( "moDependencies" );
    QString strMoDeps;
    for (int nof=0;nof<moDependencies().size();nof++)
    {
        strMoDeps.append(moDependencies().at(nof).absoluteFilePath()+";");
    }
    cMoDeps.setAttribute("list",strMoDeps);
    root.appendChild(cMoDeps);

    return root;
}

bool ModModelPlus::isCompiled(ModPlusCtrl* ctrl)
{
    return ctrl->isCompiled();
}

bool ModModelPlus::compile(ModPlusCtrl* ctrl)
{
    QFileInfoList modelsToLoad = moDependencies();

    return ctrl->compile(modelsToLoad);
}

void ModModelPlus::addConnection(ModItem* a, ModItem* b)
{
    ModelicaConnection* newConnection = new ModelicaConnection(a->name(ModItem::FULL),b->name(ModItem::FULL));
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

bool ModModelPlus::readVariables(ModPlusCtrl* ctrl, bool forceRecompile)
{
    _variablesRead = ctrl->readInitialVariables(_variables,forceRecompile);
    return _variablesRead;

}

bool ModModelPlus::variablesRead() const
{
    return _variablesRead;
}

bool ModModelPlus::readConnections(ModItem* element,bool includeChildren)
{
    if(!element)
        return false;

    QString className = ((ModComponent*)element)->getModClassName();
    QString name = element->name(ModItem::FULL);

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
    _connectionsRead = readConnections(modModel(),true);
    sendInfo (new Info(ListInfo::READCONNECTIONSSUCCESS));

    return _connectionsRead;
}

void ModModelPlus::openMoFolder()
{
    //LowTools::openFolder(modModel()->moFolder());
}
void ModModelPlus::openMmoFolder()
{
    LowTools::openFolder(mmoFolder().absolutePath());
}

void ModModelPlus::openMoFile()
{
    //LowTools::openFile(moFilePath());
}

void ModModelPlus::openDependenciesDlg()
{
    DlgSelectFiles dlgSelectFiles(_moDependencies,NULL,"*.mo");
    dlgSelectFiles.exec();
}

void ModModelPlus::openNeededFilesDlg()
{
    QWidget widget;
    DlgSelectFiles dlgSelectFiles(_neededFiles,&widget,"*.*");
    dlgSelectFiles.exec();
}


//vector<ModModelPlusicaModifier*>* MOomc::getComponentModifiers(QString componentName,ModItem* component)
//{
//
//
//    QStringList modNames = getComponentModifierNames(componentName);
//
//    vector<ModModelPlusicaModifier*> *_compModifiers = new vector<ModModelPlusicaModifier*>;
//    _compModifiers->reserve(modNames.size());
//
//    ModModelPlusicaModifier* curMod;
//
//    for(int iM=0;iM<modNames.size();iM++)
//    {
//        curMod = new ModModelPlusicaModifier();
//        curMod->setName(modNames.at(iM));
//        curMod->setValue(getComponentModifierValue(componentName,modNames.at(iM)));
//        curMod->setComponent(component);
//        _compModifiers->push_back(curMod);
//    }
//
//    return _compModifiers;
//
//}




bool ModModelPlus::applyBlockSub(BlockSubstitution *blockSub,bool compile)
{
    // delete org connections
    bool deleteOk = _moomc->deleteConnections(blockSub->_orgPorts,blockSub->_orgConnectedComps,modelName());
    QString shortOrg = blockSub->_orgComponent;
    shortOrg = shortOrg.remove(modelName()+".");
    QString shortSub = blockSub->_subComponent;
    shortSub = shortSub.remove(modelName()+".");
    ModItem* orgClass = _project->modItemsTree()->findInDescendants(blockSub->_orgComponent,modModel());
    if(!orgClass)
    {
        QString msg;
        msg.sprintf("Could not apply component substitution : component %s not found",
                    blockSub->_orgComponent.utf16());
        InfoSender::instance()->send(Info(msg,ListInfo::WARNING2));
        return false;
    }

    ModComponent* orgComp = NULL;
    if(orgClass->getClassRestr()==Modelica::COMPONENT)
        orgComp = (ModComponent*)orgClass;

    if(orgComp)
    {
        // first save annotation and modifiers
        QString annotation;// = _moomc->getAnnotation(blockSub->_orgComponent,blockSub->_model);
        QStringList modifiersNames = _moomc->getComponentModifierNames(blockSub->_orgComponent);
        QStringList modifiersValues;
        for(int i=0;i<modifiersNames.size();i++)
            modifiersValues.push_back(_moomc->getComponentModifierValue(modelName(),shortOrg,modifiersNames.at(i)));

        // delete org component
        _moomc->deleteComponent(blockSub->_orgComponent);

        // create new component
        QString newCompName = blockSub->_orgComponent;
        //keep same name will avoid problem if objective or optimized variables are in component

        _moomc->addComponent(newCompName,blockSub->_subComponent,modelName(),annotation);

        // specify modifiers equal to orgComponent
        _moomc->setComponentModifiers(newCompName,modelName(),modifiersNames,modifiersValues);

        // connect it
        QStringList newCompPorts;
        for(int i=0;i<blockSub->_subPorts.size();i++)
        {
            newCompPorts.push_back(newCompName + "." + blockSub->_subPorts.at(i).section(".",-1,-1));
        }

        _moomc->addConnections(newCompPorts,blockSub->_subConnectedComps);
    }

    _moomc->save(modelName());

    return true;
}


