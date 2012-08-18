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

  @file SaveOMOptim.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version

  */
#include "SaveOMOptim.h"
#include "Result.h"
#include "Optimization.h"
#include "OneSimResult.h"
#include "Project.h"
#include "MOVector.h"
#include "CSV.h"
#include "EABase.h"
#include "OptimResult.h"
#include "version.h"
#include "Save.h"
#include "Problems.h"
#include "Results.h"


/**
  * Save project : creates project.min file
  * @param saveAllCases : if true, save all cases (can be time consuming)
  * @param caseToSave : considered only if saveAllCases is false. Only caseToSave is then saved.
  *
  */
void SaveOMOptim::saveProject(Project* project,bool saveAllCases)
{

    // MO file
    QDomDocument doc("MOProjectXML");
    QDomElement root = doc.createElement( "MOProject" );
    doc.appendChild( root );
    root.setAttribute("Version",Version::version());

    // Project info
    QDir projectDir(project->folder());
    QDomElement cBasic = doc.createElement( "Basic" );
    cBasic.setAttribute( "name", project->name() );
    root.appendChild(cBasic);

    QString relPath;

    // Mo files
    QFileInfoList moFilesPath = project->moFiles();
    if(moFilesPath.size()>0)
    {
        QDomElement cMoFiles = doc.createElement("MoFiles");
        for(int i=0;i<moFilesPath.size();i++)
        {
            QDomElement cMoFile = doc.createElement("MoFile");
            cMoFile.setAttribute("path",moFilesPath.at(i).absoluteFilePath());
            cMoFiles.appendChild(cMoFile);
        }
        root.appendChild(cMoFiles);
    }


    // Mmo files
    QFileInfoList mmoFilesPath = project->mmoFiles();
    if(mmoFilesPath.size()>0)
    {
        QDomElement cMmoFiles = doc.createElement("MmoFiles");
        for(int i=0;i<mmoFilesPath.size();i++)
        {
            QDomElement cMmoFile = doc.createElement("MmoFile");
            relPath = projectDir.relativeFilePath(mmoFilesPath.at(i).absoluteFilePath());
            cMmoFile.setAttribute("path",relPath);
            cMmoFiles.appendChild(cMmoFile);
        }
        root.appendChild(cMmoFiles);
    }

    // plugins loaded
    QStringList pluginsPaths = project->pluginsLoaded().values();
    if(pluginsPaths.size()>0)
    {
        QDomElement cPlugins = doc.createElement("Plugins");
        for(int i=0;i<pluginsPaths.size();i++)
        {
            QDomElement cPlugin = doc.createElement("Plugin");
            cPlugin.setAttribute("path",pluginsPaths.at(i));
            cPlugins.appendChild(cPlugin);
        }
        root.appendChild(cPlugins);
    }

    // Project problems
    if(project->problems()->size()>0)
    {
        QDomElement cOMCases = doc.createElement( "Problems" );
        for (int nr=0;nr<project->problems()->size();nr++)
        {
            QDomElement cProblem = doc.createElement( "Problem" );
            relPath = projectDir.relativeFilePath(project->problems()->at(nr)->entireSavePath());
            cProblem.setAttribute("path",relPath);
            cOMCases.appendChild(cProblem);
        }
        root.appendChild(cOMCases);
    }

    // Project results
    if(project->results()->size()>0)
    {
        QDomElement cResults = doc.createElement( "Results" );

        for (int nr=0;nr<project->results()->size();nr++)
        {
            QDomElement cResult = doc.createElement( "Result" );
            relPath = projectDir.relativeFilePath(project->results()->at(nr)->entireSavePath());
            cResult.setAttribute("path",relPath);
            cResults.appendChild(cResult);
        }
        root.appendChild(cResults);
    }

    //Writing in .min file
    QFile file(project->filePath());
    QFileInfo fileInfo(project->filePath());
    QDir dir = fileInfo.absoluteDir();
    dir.mkpath(dir.absolutePath());


    if(file.exists())
    {
        file.remove();
    }
    file.open(QIODevice::WriteOnly);
    QTextStream ts( &file );
    ts << doc.toString();
    file.close();


    if(saveAllCases)
    {
        // Saving results
        Result* curResult;
        for (int nr=0;nr<project->results()->size();nr++)
        {
            curResult = project->results()->at(nr);
            Save::saveResult(project,curResult);

        }

        // Saving problems
        Problem* curProblem;
        for (int nr=0;nr<project->problems()->size();nr++)
        {
            curProblem = project->problems()->at(nr);
            Save::saveProblem(project,curProblem);
        }
    }

    // Saving ModModelPlus
    QList<ModModelPlus*> allModModelPlus = project->allModModelPlus();
    for (int m=0;m<allModModelPlus.size();m++)
    {
        SaveOMOptim::saveModModelPlus(allModModelPlus.at(m));
    }
}


void SaveOMOptim::saveModModelPlus(ModModelPlus* modModelPlus)
{
    // MO file
    QDomDocument doc("MOModelXML");
    QDomElement root = doc.createElement( "MOModel" );
    doc.appendChild(root);

    // Project info
    QDir mmoDir(modModelPlus->mmoFolder());
    QDomElement cBasic = doc.createElement( "Basic" );
    cBasic.setAttribute( "name", modModelPlus->name() );
    cBasic.setAttribute( "modelName", modModelPlus->modModelName());
    root.appendChild(cBasic);

    // Infos
    QDomElement cInfos = doc.createElement( "Infos" );
    cInfos.setAttribute("text",modModelPlus->infos());
    root.appendChild(cInfos);

    // Variables
    QDomElement cVariables = modModelPlus->variables()->toXmlData(doc,"Variables");
    root.appendChild(cVariables);



    // .mo dependencies
    QDomElement cMoDeps = doc.createElement( "moDependencies" );
    QString strMoDeps;
    for (int nof=0;nof<modModelPlus->moDependencies().size();nof++)
    {
        strMoDeps.append(modModelPlus->moDependencies().at(nof).absoluteFilePath()+";");
    }
    cMoDeps.setAttribute("list",strMoDeps);
    root.appendChild(cMoDeps);


    //Writing in MO file
    QFile file(modModelPlus->mmoFilePath());
    QFileInfo fileInfo(modModelPlus->mmoFilePath());
    QDir dir = fileInfo.absoluteDir();
    dir.mkpath(dir.absolutePath());

    if(file.exists())
    {
        file.remove();
    }
    file.open(QIODevice::WriteOnly);
    QTextStream ts( &file );
    ts << doc.toString();
    file.close();
}

bool SaveOMOptim::setModelsPath(QString projectFilePath, QStringList modelsPaths)
{
    // Open and read file
    QDomDocument doc( "MOProjectXML" );
    QFile file(projectFilePath);
    if( !file.open( QIODevice::ReadOnly ) || ! doc.setContent( &file ) )
    {
        return false;
    }
    file.close();
    QDomElement root = doc.documentElement();
    if( root.tagName() != "MOProject" )
    {
        return false;
    }

    // remove old moFiles
    QDomElement oldDomMoFiles = root.firstChildElement("MoFiles");
    root.removeChild(oldDomMoFiles);

    // set new
    // Mo files
    QDomElement newMoFiles = doc.createElement("MoFiles");
    for(int i=0;i<modelsPaths.size();i++)
    {
        QDomElement newMoFile = doc.createElement("MoFile");
        newMoFile.setAttribute("path",modelsPaths.at(i));
        newMoFiles.appendChild(newMoFile);
    }
    root.appendChild(newMoFiles);


    //Writing in .min file
    QFileInfo fileInfo(projectFilePath);
    QDir dir = fileInfo.absoluteDir();
    dir.mkpath(dir.absolutePath());


    if(file.exists())
    {
        file.remove();
    }
    file.open(QIODevice::WriteOnly);
    QTextStream ts( &file );
    ts << doc.toString();
    file.close();
}


