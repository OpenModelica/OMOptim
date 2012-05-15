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

        @file LoadOMOptim.cpp
        @brief Comments for file documentation.
        @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
        Company : CEP - ARMINES (France)
        http://www-cep.ensmp.fr/english/
        @version

  */
#include "LoadOMOptim.h"


bool LoadOMOptim::loadProject(QString filePath,Project* _project)
{
    InfoSender::instance()->send(Info(ListInfo::LOADINGPROJECT,filePath));
    _project->clear();
    _project->setFilePath(filePath);
    QDir projectDir = _project->folder();

    // set current dir as project dir : allows to use relative paths
    QDir::setCurrent(projectDir.absolutePath());

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

    // Mo files
    QStringList modelMoFilePaths;
    QDomElement domMoFiles = root.firstChildElement("MoFiles");
    QDomNodeList listMoFiles = domMoFiles.elementsByTagName("MoFile");
    for(int i=0;i<listMoFiles.size();i++)
    {
        tmpPath = listMoFiles.at(i).toElement().attribute("path", "" );
        QFileInfo modelFileInfo(tmpPath);
        if(!modelFileInfo.exists())
        {
            InfoSender::instance()->send(Info(ListInfo::MODELFILENOTEXISTS,tmpPath));
        }
        else
            modelMoFilePaths.push_back(modelFileInfo.absoluteFilePath());
    }

    // Mmo files
    QStringList modelMmoFilePaths;
    QDomElement domMmoFiles = root.firstChildElement("MmoFiles");
    QDomNodeList listMmoFiles = domMmoFiles.elementsByTagName("MmoFile");
    for(int i=0;i<listMmoFiles.size();i++)
    {
        tmpPath = listMmoFiles.at(i).toElement().attribute("path", "" );
        QFileInfo modelFileInfo(projectDir,tmpPath);
        modelMmoFilePaths.push_back(modelFileInfo.absoluteFilePath());
    }

    // Plugins
    QStringList pluginsPaths;
    QDomElement domPlugins = root.firstChildElement("Plugins");
    QDomNodeList listPlugins = domPlugins.elementsByTagName("Plugin");
    for(int i=0;i<listPlugins.size();i++)
    {
        tmpPath = listPlugins.at(i).toElement().attribute("path", "" );
        QFileInfo pluginFileInfo(tmpPath);
        pluginsPaths.push_back(pluginFileInfo.absoluteFilePath());
    }

    // Problems to load
    QStringList problemsPaths;
    QDomElement domOMCases = root.firstChildElement("Problems");
    QDomNodeList listOMCases = domOMCases.elementsByTagName("Problem");
    for(int i=0;i<listOMCases.size();i++)
    {
        tmpPath = listOMCases.at(i).toElement().attribute("path", "" );
        QFileInfo problemFileInfo(projectDir,tmpPath);
        problemsPaths.push_back(problemFileInfo.absoluteFilePath());
    }

    // Results to load
    QStringList resultsPaths;
    QDomElement domResults = root.firstChildElement("Results");
    QDomNodeList listResults = domResults.elementsByTagName("Result");
    for(int i=0;i<listResults.size();i++)
    {
        tmpPath = listResults.at(i).toElement().attribute("path", "" );
        QFileInfo solvedFileInfo(projectDir,tmpPath);
        resultsPaths.push_back(solvedFileInfo.absoluteFilePath());
    }


    //**************************************
    // Reading Mo Files
    //**************************************
    QSettings settings("MO", "Settings");
    for(int i=0;i<modelMoFilePaths.size();i++)
    {
        QFileInfo fileinfo = QFileInfo(modelMoFilePaths.at(i));
        qDebug(fileinfo.absoluteFilePath().toLatin1().data());

        if (!fileinfo.exists())
            InfoSender::instance()->send(Info(ListInfo::MODELFILENOTEXISTS,modelMoFilePaths.at(i)));
    }
    _project->loadMoFiles(modelMoFilePaths);

    //**************************************
    // Reading Mmo Files
    //**************************************
    for(int i=0;i<modelMmoFilePaths.size();i++)
    {
        QFileInfo fileinfo = QFileInfo(modelMmoFilePaths.at(i));

        if (!fileinfo.exists())
            InfoSender::instance()->send(Info(ListInfo::MODELFILENOTEXISTS,modelMmoFilePaths.at(i)));
        else
            _project->loadModModelPlus(modelMmoFilePaths.at(i));
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

bool LoadOMOptim::loadModModelPlus(Project* project,QString mmoFilePath)
{
    InfoSender::instance()->send( Info(ListInfo::LOADINGMODEL,mmoFilePath));

    // Open file
    QDomDocument doc( "MOModelXML" );
    QFileInfo mmoFileInfo(mmoFilePath);
    QDir mmoDir(mmoFileInfo.absolutePath());
    QFile file(mmoFilePath);
    if( !file.open( QIODevice::ReadOnly ) )
    {
        InfoSender::instance()->send( Info(ListInfo::MODELFILENOTEXISTS,mmoFilePath));
        return false;
    }
    QString error;
    if( !doc.setContent( &file,&error ) )
    {
        file.close();
        InfoSender::instance()->send( Info(ListInfo::MODMODELFILECORRUPTED,error,mmoFilePath));
        return false;
    }
    file.close();
    QDomElement root = doc.documentElement();
    if( root.tagName() != "MOModel" )
    {
        error = "Root tagname should be <MOModel>";
        InfoSender::instance()->send( Info(ListInfo::MODMODELFILECORRUPTED,error,mmoFilePath));
        return false;
    }
    // Read file
    QDomElement domBasic = root.firstChildElement("Basic");
    QString name = domBasic.attribute("name");
    QString modelName = domBasic.attribute("modelName");

    // Check if model exist
    ModModel* modModel = project->findModModel(modelName);
    if(modModel==NULL)
        return false;
    else
    {
        ModModelPlus* newModelPlus = new ModModelPlus(project,modelName);

        // .mo dependencies
        QDomElement domMoDeps = root.firstChildElement("moDependencies");
        QString moDeps = domMoDeps.attribute( "list", "" );
        newModelPlus->setMoDependencies(moDeps.split(";",QString::SkipEmptyParts));

        // Infos
        QDomElement domInfos = root.firstChildElement("Infos");
        newModelPlus->setInfos(domInfos.attribute("text",""));

        // FilePath
        newModelPlus->setMmoFilePath(mmoFilePath);

        // Variables
        QDomElement domVariables = root.firstChildElement("Variables");
        newModelPlus->variables()->setItems(domVariables);

        project->addModModelPlus(newModelPlus);
        return true;
    }
}

QStringList LoadOMOptim::getModelsPath(QString projectFilePath)
{
    QStringList result;

    // Open and read file
    QDomDocument doc( "MOProjectXML" );
    QFile file(projectFilePath);
    if( !file.open( QIODevice::ReadOnly ) || ! doc.setContent( &file ) )
    {
        return result;
    }
    file.close();
    QDomElement root = doc.documentElement();
    if( root.tagName() != "MOProject" )
    {
        return result;
    }

    //**************************************
    // Reading XML file
    //**************************************
    QDomElement domMoFiles = root.firstChildElement("MoFiles");
    QDomNodeList listMoFiles = domMoFiles.elementsByTagName("MoFile");
    QString tmpPath;
    for(int i=0;i<listMoFiles.size();i++)
    {
        tmpPath = listMoFiles.at(i).toElement().attribute("path", "" );
        result.push_back(tmpPath);
    }
    return result;
}


