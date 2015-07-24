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

  @file Save.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version

  */
#include "Save.h"
#include "Result.h"
#include "MOVector.h"
#include "ProjectBase.h"
#include "Problem.h"
#include "Problems.h"
#include "Results.h"
#include "ProblemInterface.h"

Save::Save(void)
{
}

Save::~Save(void)
{
}

/**
  * Save project : creates project.min file
  * @param saveAllCases : if true, save all cases (can be time consuming)
  * @param caseToSave : considered only if saveAllCases is false. Only caseToSave is then saved.
  *
  */
void Save::saveProject(ProjectBase* project,bool saveAllCases)
{

    // MO file
    QDomDocument doc("MOProjectXML");
    QDomElement root = doc.createElement( "MOProject" );
    doc.appendChild( root );


    // Project info
    QDir projectDir = project->folder();
    QDomElement cBasic = doc.createElement( "Basic" );
    cBasic.setAttribute( "name", project->name() );
    root.appendChild(cBasic);

    QString relPath;

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
            saveResult(project,curResult);

        }

        // Saving problems
        Problem* curProblem;
        for (int nr=0;nr<project->problems()->size();nr++)
        {
            curProblem = project->problems()->at(nr);
            saveProblem(project,curProblem);
        }
    }
}

void Save::saveProblem(ProjectBase* project,Problem* problem)
{
    ProblemInterface* itf = project->problemsInterfaces().interfaceOf(problem);
    if(itf)
        itf->saveProblem(problem);
}

void Save::saveResult(ProjectBase* project,Result* result)
{
    ProblemInterface* itf = project->problemsInterfaces().interfaceOf(result);
    if(itf)
        itf->saveResult(result);
}

