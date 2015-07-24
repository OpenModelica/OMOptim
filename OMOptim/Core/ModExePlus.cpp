#include "ModExePlus.h"

// $Id: ModExePlus.cpp 11099 2012-02-13 11:08:34Z hubert.thieriot $
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

        @file ModExePlus.cpp
        @brief Comments for file documentation.
        @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
        Company : CEP - ARMINES (France)
        http://www-cep.ensmp.fr/english/
        @version

  */


#include <iostream>
#include <vector>

#include "MOomc.h"
#include "LowTools.h"
#include "HighTools.h"
#include "Load.h"
#include "Save.h"
#include "ModPlusCtrl.h"
#include "MOParametersDlg.h"
#include "Project.h"
#include "ExeModel.h"
#include "ModItemsTree.h"
#include "Variable.h"
#include "Variables.h"

ModExePlus::ModExePlus( Project* project,QString modelName)
    :ModelPlus(project,modelName)
{
    //    _moomc = _project->moomc();
    //    _connections = new ModelicaConnections(_project->modItemsTree());
    //    _modifiers = new MOVector<ModelicaModifier>(true);
}

ModExePlus::ModExePlus(Project * project, const QDomElement & domRoot)
    :ModelPlus(project,domRoot)
{
    // files
    // add input file and exe file
    QDomElement cFiles = domRoot.firstChildElement("Files" );
    QString exeFile = cFiles.attribute("exeFile");
    QString inputFile = cFiles.attribute( "inputFile");

    QDir::setCurrent(project->folder().absolutePath());

    QFileInfo exeFileInfo(exeFile);
    QFileInfo inputFileInfo(inputFile);

    // load/create exeModel
    project->loadExecutableModel(_modelName,exeFileInfo,inputFileInfo,this);
}

ModExePlus::~ModExePlus()
{

}

/**
  * @brief Clear ModExePlus content
  * Clear ModExePlus content : includnig variables, connections
  */
void ModExePlus::clear()
{
    ModelPlus::clear();

    //connections
    //  _connections->clear();
    //  _modifiers->clear();

}

void ModExePlus::save()
{
    // Save::saveModModelPlus(this);
    emit saved();
}





/**
* Returns a vector containg the variables concerning a child element
* @Param _element : child element
*/
/*
Variables* ModExePlus::variables(ModItem* element)
{
    Variables* elVars = new Variables(true);
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




//QString ModExePlus::infos()
//{
//    return _infos;
//}

ExeModel* ModExePlus::modModel()
{
    return dynamic_cast<ExeModel*>(_project->modItemsTree()->findItem(_modelName));
}

//void ModExePlus::setInfos(QString infos)
//{
//    _infos = infos;
//}

bool ModExePlus::readAll(ModPlusCtrl *ctrl)
{
    bool varOk = false;
    bool connOk = false;

    varOk = readVariables(ctrl);

    return varOk && connOk;
}


QDomElement ModExePlus::toXmlData(QDomDocument &doc)
{
    QDomElement root = ModelPlus::toXmlData(doc);
    root.setTagName(ModExePlus::className());

    // add input file and exe file
    QDomElement cFiles = doc.createElement( "Files" );

    // Mo files
    bool useRelativePath;

    QDir projectDir = _project->folder();

    // if file is in project folder, use relative path
    useRelativePath = (exeFile().absoluteFilePath().indexOf(projectDir.absolutePath())==0);
    if(useRelativePath)
        cFiles.setAttribute( "exeFile", projectDir.relativeFilePath(exeFile().absoluteFilePath()));
    else
        cFiles.setAttribute( "exeFile", this->exeFile().absoluteFilePath());


    useRelativePath = (inputFile().absoluteFilePath().indexOf(projectDir.absolutePath())==0);
    if(useRelativePath)
        cFiles.setAttribute( "inputFile", projectDir.relativeFilePath(inputFile().absoluteFilePath()));
    else
        cFiles.setAttribute( "inputFile", this->inputFile().absoluteFilePath());

    root.appendChild(cFiles);
    return root;
}

bool ModExePlus::isCompiled(ModPlusCtrl* ctrl)
{
    return ctrl->isCompiled();
}

bool ModExePlus::compile(ModPlusCtrl* ctrl,QFileInfoList filesToCopy)
{
    return ctrl->compile(QFileInfoList(),filesToCopy);
}

QFileInfo ModExePlus::exeFile()
{
    if(!modModel())
        return QFileInfo();

    QFileInfo exeFileInfo = modModel()->exeFile();
    qDebug(exeFileInfo.absoluteFilePath().toLatin1().data());
    if(exeFileInfo.isRelative())
        exeFileInfo = QFileInfo(_project->folder(),exeFileInfo.filePath());
    qDebug(exeFileInfo.absoluteFilePath().toLatin1().data());

    return exeFileInfo;
}

QFileInfo ModExePlus::inputFile()
{
    if(!modModel())
        return QFileInfo();

    QFileInfo inputFileInfo = modModel()->inputFile();
    qDebug(inputFileInfo.absoluteFilePath().toLatin1().data());
    if(inputFileInfo.isRelative())
        inputFileInfo = QFileInfo(_project->folder(),inputFileInfo.filePath());
    qDebug(inputFileInfo.absoluteFilePath().toLatin1().data());

    return inputFileInfo;
}



bool ModExePlus::readVariables(ModPlusCtrl* ctrl, bool forceRecompile)
{
    _variablesRead = ctrl->readInitialVariables(_variables,QFileInfoList(), forceRecompile);
    return _variablesRead;
}

bool ModExePlus::variablesRead() const
{
    return _variablesRead;
}




//bool ModExePlus::applyBlockSub(BlockSubstitution *blockSub,bool compile)
//{
//    // delete org connections
//    bool deleteOk = _moomc->deleteConnections(blockSub->_orgPorts,blockSub->_orgConnectedComps,modelName());
//    QString modelName = modelName();
//    QString shortOrg = blockSub->_orgComponent;
//    shortOrg = shortOrg.remove(modelName+".");
//    QString shortSub = blockSub->_subComponent;
//    shortSub = shortSub.remove(modelName+".");
//    ModItem* orgClass = _project->modItemsTree()->findInDescendants(blockSub->_orgComponent,modModel());
//    if(!orgClass)
//    {
//        QString msg;
//        msg.sprintf("Could not apply component substitution : component %s not found",
//                    blockSub->_orgComponent.utf16());
//        InfoSender::instance()->send(Info(msg,ListInfo::WARNING2));
//        return false;
//    }

//    ModComponent* orgComp = NULL;
//    if(orgClass->getClassRestr()==Modelica::COMPONENT)
//        orgComp = (ModComponent*)orgClass;

//    if(orgComp)
//    {
//        // first save annotation and modifiers
//        QString annotation;// = _moomc->getAnnotation(blockSub->_orgComponent,blockSub->_model);
//        QStringList modifiersNames = _moomc->getComponentModifierNames(blockSub->_orgComponent);
//        QStringList modifiersValues;
//        for(int i=0;i<modifiersNames.size();i++)
//            modifiersValues.push_back(_moomc->getComponentModifierValue(modelName,shortOrg,modifiersNames.at(i)));

//        // delete org component
//        _moomc->deleteComponent(blockSub->_orgComponent);

//        // create new component
//        QString newCompName = blockSub->_orgComponent;
//        //keep same name will avoid problem if objective or optimized variables are in component

//        _moomc->addComponent(newCompName,blockSub->_subComponent,modelName(),annotation);

//        // specify modifiers equal to orgComponent
//        _moomc->setComponentModifiers(newCompName,modelName(),modifiersNames,modifiersValues);

//        // connect it
//        QStringList newCompPorts;
//        for(int i=0;i<blockSub->_subPorts.size();i++)
//        {
//            newCompPorts.push_back(newCompName + "." + blockSub->_subPorts.at(i).section(".",-1,-1));
//        }

//        _moomc->addConnections(newCompPorts,blockSub->_subConnectedComps);
//    }

//    _moomc->save(modelName());

//    return true;
//}



