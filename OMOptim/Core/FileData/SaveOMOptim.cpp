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

    QDomDocument doc = ProjectToXml(project);

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
    QList<ModelPlus*> allModelPlus = project->allModelPlus();
    for (int m=0;m<allModelPlus.size();m++)
    {
        SaveOMOptim::saveModelPlus(project,allModelPlus.at(m));
    }
}


//void SaveOMOptim::saveExternalProject(Project* project, QDir exportFolder)
//{
//    Project* externalProject = new GhostProject();


//    QDomDocument doc = ProjectToXml(project);
//    QDomElement root = doc.documentElement();

//    // Mo files
//    QString tmpPath;
//    QString newMoFileName;
//    QDomElement domMoFiles = root.firstChildElement("MoFiles");
//    QDomElement newDomMoFiles = doc.createElement("MoFiles");

//    QDomNodeList listMoFiles = domMoFiles.elementsByTagName("MoFile");
//    for(int i=0;i<listMoFiles.size();i++)
//    {
//        tmpPath = listMoFiles.at(i).toElement().attribute("path", "" );
//        QFileInfo modelFileInfo(tmpPath);
//        if(!modelFileInfo.isRelative())
//        {
//            // get new paths
//            newMoFileName = modelFileInfo.fileName();
//            int i=2;
//            while(exportFolder.entryList().contains(newMoFileName))
//            {

//                newMoFileName = modelFileInfo.baseName()+
//                        QString::number(i)+"."+modelFileInfo.completeSuffix();
//                i++;
//            }

//            // copy file
//            QFile::copy(modelFileInfo.absoluteFilePath(),exportFolder.absoluteFilePath(newMoFileName));

//            // change xml
//            QDomElement cNewMoFile = doc.createElement("MoFile");
//            cNewMoFile.setAttribute("path",newMoFileName);
//            newDomMoFiles.appendChild(cNewMoFile);
//        }
//        else
//        {
//            QDomElement cNewMoFile = doc.createElement("MoFile");
//            cNewMoFile.setAttribute("path",tmpPath);
//            newDomMoFiles.appendChild(cNewMoFile);
//        }
//    }
//    doc.removeChild(domMoFiles);
//    doc.appendChild(newDomMoFiles);


//    //Writing in .min file
//    QFileInfo fileInfo(project->filePath());
//    QFile newProjectFile(exportFolder.absoluteFilePath(fileInfo.fileName()));

//    exportFolder.mkpath(exportFolder.absolutePath());


//    if(newProjectFile.exists())
//    {
//        newProjectFile.remove();
//    }
//    newProjectFile.open(QIODevice::WriteOnly);
//    QTextStream ts( &newProjectFile );
//    ts << doc.toString();
//    newProjectFile.close();

//    // copy problems and result files

//}

QDomDocument SaveOMOptim::ProjectToXml(Project * project)
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
    bool useRelativePath;
    QFileInfoList moFilesPath = project->moFiles();
    if(moFilesPath.size()>0)
    {
        QDomElement cMoFiles = doc.createElement("MoFiles");
        for(int i=0;i<moFilesPath.size();i++)
        {
            // if mo file is in project folder, use relative path
            useRelativePath = (moFilesPath.at(i).absoluteFilePath().indexOf(projectDir.absolutePath())==0);
            QDomElement cMoFile = doc.createElement("MoFile");
            if(useRelativePath)
                cMoFile.setAttribute("path",projectDir.relativeFilePath(moFilesPath.at(i).absoluteFilePath()));
            else
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
            relPath = projectDir.relativeFilePath(mmoFilesPath.at(i).filePath());
            qDebug(mmoFilesPath.at(i).filePath().toLatin1());
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

    return doc;
}



void SaveOMOptim::saveModelPlus(Project* project,ModelPlus* modelPlus)
{
    // MO file
    QDomDocument doc("MOModelXML");
    QDomElement domModel = modelPlus->toXmlData(doc);
    doc.appendChild(domModel);

    //Writing in MO file
    QString relMmoFilePath = modelPlus->mmoFilePath().filePath();
    QString mmoFilePath = project->folder().absoluteFilePath(relMmoFilePath);
    QFile file(mmoFilePath);
    QFileInfo fileInfo(mmoFilePath);
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


