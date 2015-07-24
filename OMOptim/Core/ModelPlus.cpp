
#include "ModelPlus.h"

#include "MOVector.h"
#include "MOomc.h"
#include "Info.h"
//#include "ModelicaConnections.h"
#include "ModelicaModifier.h"
//#include "BlockSubs/BlockSubstitution.h"
#include "MOSettings.h"
#include "ModModel.h"
#include "InfoSender.h"
#include "ModItemsTree.h"

#include "Variable.h"
#include "Variables.h"


// $Id: ModModelPlus.cpp 11099 2012-02-13 11:08:34Z hubert.thieriot $
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

#include <iostream>
#include <vector>
#include <QtCore/QObject>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>
#include <QtCore/QStringList>


#include "Variable.h"
//#include "MOomc.h"
#include "LowTools.h"
#include "HighTools.h"
#include "Load.h"
#include "Save.h"
#include "ModPlusCtrl.h"
//#include "ModPlusOMCtrl.h"
//#include "ModPlusDymolaCtrl.h"
#include "MOParametersDlg.h"
#include "Project.h"
#include "MOItem.h"


ModelPlus::ModelPlus( Project* project,QString modelName)
{
    _project = project;
    _variables = new Variables(true,this);
    _variablesRead = false;
    _name = modelName;
    _modelName = modelName;

    connect(this,SIGNAL(modified()),this,SIGNAL(variablesUpdated()));
}

ModelPlus::ModelPlus(Project *project, const QDomElement &domRoot)
{
    _project = project;
    _variablesRead = false;


    QDomElement domBasic = domRoot.firstChildElement("Basic");
    _name = domBasic.attribute("name");
    _modelName = domBasic.attribute("modelName");

    // Infos
    QDomElement cInfos = domRoot.firstChildElement( "Infos" );
    _infos = cInfos.attribute("text");

    // Variables
    QDomElement cVariables = domRoot.firstChildElement( "Variables" );
    _variables = new Variables(true,this);
    _variables->update(cVariables);


    connect(this,SIGNAL(modified()),this,SIGNAL(variablesUpdated()));
}


ModelPlus::~ModelPlus()
{
    delete _variables;
}


/**
  * @brief Clear ModModelPlus content
  * Clear ModelPlus content: here only variables
  * But in inheriting classes, can clear more (e.g. connections in ModModelPlus)
  */
void ModelPlus::clear()
{
    // clear variables
    _variablesRead = false;
    //connections
    _variables->clear();
}


void ModelPlus::setOtherFiles(QStringList otherFiles)
{
    _otherFiles = otherFiles;
}

void ModelPlus::setInfos(QString infos)
{
    _infos = infos;
}

QString ModelPlus::infos()
{
    return _infos;
}

QStringList ModelPlus::otherFiles()
{
    return _otherFiles;
}

void ModelPlus::clearOtherFiles()
{
    _otherFiles.clear();
}

Project *ModelPlus::getProject()
{
    return _project;
}

ModModel* ModelPlus::modModel()
{
    return dynamic_cast<ModModel*>(_project->modItemsTree()->findItem(_modelName));
}

QFileInfo ModelPlus::mmoFilePath()
{
    return _project->folder().absoluteFilePath(_relMmoFilePath);
}



QString ModelPlus::modelName()
{
        return _modelName;
}


void ModelPlus::save()
{
    //Save::saveModModelPlus(this);
    emit saved();
}


QFileInfoList ModelPlus::neededFiles()
{
    return _neededFiles;
}


QFileInfoList ModelPlus::neededFolders()
{
    return _neededFolders;
}

QDir ModelPlus::mmoFolder()
{
    QFileInfo fileInfo(mmoFilePath());
    return fileInfo.dir();
}


//************************
//  Variables
//************************
void ModelPlus::addVariable(Variable* var)
{
    // add item in variables vector
    _variables->addItem(var);
}

/**
* Returns pointer to variables.
*/
Variables * ModelPlus::variables()
{
    //    if(!_variablesRead && readIfNot)
    //        readVariables();

    return _variables;
}

QDomElement ModelPlus::toXmlData(QDomDocument &doc)
{
    QDomElement root = doc.createElement(ModelPlus::className());

    // Project info
    QDomElement cBasic = doc.createElement( "Basic" );
    cBasic.setAttribute( "name", name() );
    cBasic.setAttribute( "modelName", modelName());
    cBasic.setAttribute( "modelType", modelType());
    root.appendChild(cBasic);

    // Infos
    QDomElement cInfos = doc.createElement( "Infos" );
    cInfos.setAttribute("text",infos());
    root.appendChild(cInfos);

    // Variables
    QDomElement cVariables = variables()->toXmlData(doc,"Variables");
    root.appendChild(cVariables);

    return root;
}

/**
* Returns a vector containg the variables concerning a child element
* @Param _element : child element
*/
/*
Variables* ModelPlus::variables(ModItem* element)
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


//bool ModelPlus::readAll(ModPlusCtrl *ctrl)
//{
//    bool varOk = false;
//    bool connOk = false;

//    varOk = readVariables(ctrl);
//    connOk = readConnections();

//    return varOk && connOk;
//}


bool ModelPlus::readVariables(ModPlusCtrl* ctrl, const QFileInfoList filesToCopy, bool forceRecompile)
{
    _variablesRead = ctrl->readInitialVariables(_variables,filesToCopy,forceRecompile);
    return _variablesRead;
}

bool ModelPlus::variablesRead() const
{
    return _variablesRead;
}

void ModelPlus::setMmoFilePath(QString filePath)
{
    _relMmoFilePath = _project->folder().relativeFilePath(filePath);
}
