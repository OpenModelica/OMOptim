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

        @file Load.cpp
        @brief Comments for file documentation.
        @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
        Company : CEP - ARMINES (France)
        http://www-cep.ensmp.fr/english/
        @version

  */
#include "Load.h"

#include "Result.h"
#include "ProjectBase.h"
#include "Problem.h"
#include "MOVector.h"
#include "MOThreads.h"
#include "Info.h"
#include "ProblemInterface.h"

Load::Load(void)
{
}

Load::~Load(void)
{
}

bool Load::loadProject(QString filePath,ProjectBase* _project)
{

    InfoSender::instance()->send(Info(ListInfo::LOADINGPROJECT,filePath));
    _project->clear();
    _project->setFilePath(filePath);
    QDir projectDir = _project->folder();

    QString tmpPath;

    // Open and read file
    QDomDocument doc( "MOProjectXML" );
    QFile file(filePath);
    if( !file.open( QIODevice::ReadOnly ) )
    {
        InfoSender::instance()->send( Info(ListInfo::PROJECTFILENOTEXISTS,filePath));
        return false;
    }
    if( !doc.setContent( &file ) )
    {
        file.close();
        InfoSender::instance()->send( Info(ListInfo::PROJECTFILECORRUPTED,filePath));
        return false;
    }
    file.close();
    QDomElement root = doc.documentElement();
    if( root.tagName() != "MOProject" )
    {
        InfoSender::instance()->send( Info(ListInfo::PROJECTFILECORRUPTED,filePath));
        return false;
    }

    //**************************************
    // Reading XML file
    //**************************************
    // name
    QDomElement domBasic = root.firstChildElement("Basic");
    _project->setName(domBasic.attribute("name", "" ));

    // Plugins
    QStringList pluginsPaths;
    QDomElement domPlugins = root.firstChildElement("Plugins");
    QDomNodeList listPlugins = domPlugins.elementsByTagName("Plugin");
    for(int i=0;i<listPlugins.size();i++)
    {
        tmpPath = listPlugins.at(i).toElement().attribute("path", "" );
        QFileInfo pluginFileInfo(tmpPath);
        tmpPath = pluginFileInfo.canonicalFilePath();
        pluginsPaths.push_back(tmpPath);
    }

    // Problems to load
    QStringList problemsPaths;
    QDomElement domOMCases = root.firstChildElement("Problems");
    QDomNodeList listOMCases = domOMCases.elementsByTagName("Problem");
    for(int i=0;i<listOMCases.size();i++)
    {
        tmpPath = listOMCases.at(i).toElement().attribute("path", "" );
        QFileInfo problemFileInfo(projectDir,tmpPath);
        problemsPaths.push_back(problemFileInfo.canonicalFilePath());
    }

    // Results to load
    QStringList resultsPaths;
    QDomElement domResults = root.firstChildElement("Results");
    QDomNodeList listResults = domResults.elementsByTagName("Result");
    for(int i=0;i<listResults.size();i++)
    {
        tmpPath = listResults.at(i).toElement().attribute("path", "" );
        QFileInfo solvedFileInfo(projectDir,tmpPath);
        resultsPaths.push_back(solvedFileInfo.canonicalFilePath());
    }

    //**************************************
    // Reading plugins
    //**************************************
    for(int i=0;i<pluginsPaths.size();i++)
    {
        QFileInfo fileinfo = QFileInfo(pluginsPaths.at(i));
        if (!fileinfo.exists())
            InfoSender::instance()->sendError("Plugin file does not exist : "+pluginsPaths.at(i));
        else
            _project->loadPlugin(pluginsPaths.at(i));
    }

    //**************************************
    // Reading problems
    //**************************************
    QTime loadTime;
    QString msg;
    for(int i=0;i<problemsPaths.size();i++)
    {
        loadTime.restart();
        _project->addOMCase(problemsPaths.at(i));
        msg = "Loaded problem "+ problemsPaths.at(i)+" took " +QString::number((double)loadTime.elapsed()/1000)+"sec";
        InfoSender::instance()->debug(msg);
    }

    //**************************************
    // Reading results
    //**************************************
    for(int i=0;i<resultsPaths.size();i++)
    {
        loadTime.restart();
        _project->addOMCase(resultsPaths.at(i));
        msg = "Loaded result "+ resultsPaths.at(i)+" took " +QString::number((double)loadTime.elapsed()/1000)+"sec";
        InfoSender::instance()->debug(msg);
    }

    _project->setIsDefined(true);

    return true;
}

OMCase* Load::newOMCase(QString filePath,ProjectBase* project)
{
    QString error;
    OMCase* omCase = NULL;

    QDomDocument doc( "OMCase" );
    QFile file(filePath);
    if( !file.open( QIODevice::ReadOnly ) )
    {
        InfoSender::instance()->send( Info(ListInfo::PROBLEMFILENOTEXISTS,filePath));
        return NULL;
    }
    else if( !doc.setContent(&file,&error) )
    {
        file.close();
        InfoSender::instance()->send( Info(ListInfo::PROBLEMFILECORRUPTED,filePath));
        return NULL;
    }
    file.close();

    QDomElement domCase = doc.firstChildElement("OMCase");

    // get problem type and find out if it is a result
    bool isAResult = (!domCase.firstChildElement("OMResult").isNull());
    QString problemType;
    QDomElement domProblem = domCase.firstChildElement("OMProblem");
    if(!domProblem.isNull())
        problemType = domProblem.firstChildElement().tagName();

    // look for corresponding problem itf
    ProblemInterface* itf = project->problemsInterfaces().interfaceOf(problemType);

    // load
    if(itf)
    {
        if(isAResult)
            omCase = itf->loadResult(QFileInfo(file),domCase,project);
        else
            omCase = itf->loadProblem(QFileInfo(file),domCase,project);
    }
    else
    {
        InfoSender::instance()->sendError("Following problem type not managed : "+problemType+" ["+filePath+"]");
    }

    return omCase;
}

