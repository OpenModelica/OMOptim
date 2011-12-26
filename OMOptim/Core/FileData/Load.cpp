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

Load::Load(void)
{
}

Load::~Load(void)
{
}

bool Load::loadProject(QString filePath,Project* _project)
{

    infoSender.send(Info(ListInfo::LOADINGPROJECT,filePath));
    _project->clear();
    _project->setFilePath(filePath);
    QDir projectDir(_project->folder());

    QString tmpPath;

    // Open and read file
    QDomDocument doc( "MOProjectXML" );
    QFile file(filePath);
    if( !file.open( QIODevice::ReadOnly ) )
    {
        infoSender.send( Info(ListInfo::PROJECTFILENOTEXISTS,filePath));
        return false;
    }
    if( !doc.setContent( &file ) )
    {
        file.close();
        infoSender.send( Info(ListInfo::PROJECTFILECORRUPTED,filePath));
        return false;
    }
    file.close();
    QDomElement root = doc.documentElement();
    if( root.tagName() != "MOProject" )
    {
        infoSender.send( Info(ListInfo::PROJECTFILECORRUPTED,filePath));
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
            modelFileInfo=QFileInfo(projectDir,tmpPath); //stored in relative path
            if(!modelFileInfo.exists())
            {

                infoSender.send(Info(ListInfo::MODELFILENOTEXISTS,tmpPath));
            }
            else
            {
                tmpPath = modelFileInfo.canonicalFilePath();
                modelMoFilePaths.push_back(tmpPath);
            }
        }
        else
            modelMoFilePaths.push_back(tmpPath);
    }

    // Mmo files
    QStringList modelMmoFilePaths;
    QDomElement domMmoFiles = root.firstChildElement("MmoFiles");
    QDomNodeList listMmoFiles = domMmoFiles.elementsByTagName("MmoFile");
    for(int i=0;i<listMmoFiles.size();i++)
    {
        tmpPath = listMmoFiles.at(i).toElement().attribute("path", "" );
        QFileInfo modelFileInfo(projectDir,tmpPath);
        tmpPath = modelFileInfo.canonicalFilePath();
        modelMmoFilePaths.push_back(tmpPath);
    }

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
    // Reading Mo Files
    //**************************************
    QSettings settings("MO", "Settings");


    for(int i=0;i<modelMoFilePaths.size();i++)
    {
        QFileInfo fileinfo = QFileInfo(modelMoFilePaths.at(i));
        if (!fileinfo.exists())
            infoSender.send(Info(ListInfo::MODELFILENOTEXISTS,modelMoFilePaths.at(i)));
    }
    _project->loadMoFiles(modelMoFilePaths);

    //**************************************
    // Reading Mmo Files
    //**************************************
    for(int i=0;i<modelMmoFilePaths.size();i++)
    {
        QFileInfo fileinfo = QFileInfo(modelMmoFilePaths.at(i));
        if (!fileinfo.exists())
            infoSender.send(Info(ListInfo::MODELFILENOTEXISTS,modelMmoFilePaths.at(i)));
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
            infoSender.sendError("Plugin file does not exist : "+modelMmoFilePaths.at(i));
        else
            _project->loadPlugin(pluginsPaths.at(i));
    }

    //**************************************
    // Reading problems
    //**************************************
    for(int i=0;i<problemsPaths.size();i++)
        _project->addOMCase(problemsPaths.at(i));

    //**************************************
    // Reading results
    //**************************************
    for(int i=0;i<resultsPaths.size();i++)
        _project->addOMCase(resultsPaths.at(i));

    _project->setIsDefined(true);

    return true;
}
//Result* Load::newResultFromFile(QString filePath,Project* _project)
//{
//	QDomDocument doc( "MOResult" );
//	QFile file(filePath);
//	if( !file.open( QIODevice::ReadOnly ) )
//	{
//		infoSender.send( Info(ListInfo::RESULTFILENOTEXISTS,filePath));
//		return NULL;
//	}
//
//	QString erreur;
//	if( !doc.setContent( &file,false,&erreur ) )
//	{
//		file.close();
//		infoSender.send( Info(ListInfo::RESULTFILECORRUPTED,filePath));
//		return NULL;
//	}
//	file.close();
//
//	QDomElement root = doc.documentElement();
//	QString resultType = root.tagName();
//
//
//
//
//	if (resultType=="OneSimulationResult")
//	{
//		return newOneSimResultFromFile(filePath,_project);
//	}
//	else
//	{
//		if (resultType=="OptimResult")
//		{
//			return newOptimResultFromFile(filePath,_project);
//		}
//		else
//		{
//			return NULL;
//		}
//	}
//}
//
//Result* Load::newOneSimResultFromFile(QString filePath, Project* _project)
//{
//	OneSimResult * result = new OneSimResult();
//	result->setSavePath(filePath);
//	result->setModel(_project->getModel());
//	result->setProject(_project);
//
//	QDomDocument doc( "MOResult" );
//	QFile file(filePath);
//	if( !file.open( QIODevice::ReadOnly ) )
//	{
//		infoSender.send( Info(ListInfo::RESULTFILENOTEXISTS,filePath));
//		return NULL;
//	}
//
//	if( !doc.setContent( &file ) )
//	{
//		file.close();
//		infoSender.send( Info(ListInfo::RESULTFILECORRUPTED,filePath));
//		return NULL;
//	}
//	file.close();
//
//	QDomElement root = doc.documentElement();
//
//	QDomNode n = root.firstChild();
//	while( !n.isNull() )
//	{
//		QDomElement e = n.toElement();
//		if( !e.isNull() )
//		{
//			if( e.tagName() == "Result" )
//			{
//				result->setName(e.attribute("name", "" ));
//			}
//			if( e.tagName() == "InputVariables" )
//			{
//				QString varsString = e.text();
//				CSV::LinesToVariables(result->inputVariables,varsString);
//			}
//
//			if( e.tagName() == "FinalVariables" )
//			{
//				QString varsString = e.text();
//				CSV::LinesToVariables(result->finalVariables,varsString);
//			}
//		}
//		n = n.nextSibling();
//	}
//
//	//cloning eiStreams in results
//	result->eiStreams->cloneFromOtherVector(_project->getModel()->eiStreams);
//	result->updateEiStreams();
//
//	return result;
//}
//
//
//
////Result* Load::newOptimResultFromFile(QString filePath,Project* _project)
////{
////	OptimResult * result = new OptimResult();
////	result->setSavePath(filePath);
////	result->setModel(_project->getModel());
////	result->setProject(_project);
////
////	QDomDocument doc( "MOResult" );
////	QFile file(filePath);
////	if( !file.open( QIODevice::ReadOnly ) )
////	{
////		infoSender.send( Info(ListInfo::RESULTFILENOTEXISTS,filePath));
////		return NULL;
////	}
////
////	if( !doc.setContent( &file ) )
////	{
////		file.close();
////		infoSender.send( Info(ListInfo::RESULTFILECORRUPTED,filePath));
////		return NULL;
////	}
////	file.close();
////
////	QDomElement root = doc.documentElement();
////
////	QDomNode n = root.firstChild();
////	while( !n.isNull() )
////	{
////		QDomElement e = n.toElement();
////		if( !e.isNull() )
////		{
////			if( e.tagName() == "Result" )
////			{
////				result->setName(e.attribute("name", "" ));
////			}
////			if( e.tagName() == "OptObjectives" )
////			{
////				QString varsString = e.text();
////				CSV::LinesToOptObjectives(result->optObjectivesResults(),varsString);
////			}
////
////			if( e.tagName() == "OptVariables" )
////			{
////				QString varsString = e.text();
////				CSV::LinesToOptVariables(result->optVariablesResults,varsString);
////			}
////
////
////			if( e.tagName() == "AlgoParameters" )
////			{
////				QString paramString = e.text();
////				result->eaConfig->parameters = new MOVector<AlgoParameter>(paramString);
////			}
////
////
////
////			if( e.tagName() == "FrontFile" )
////			{
////				result->frontFileName = e.attribute("path","");
////			}
////
////
////		}
////		n = n.nextSibling();
////	}
////
////	// Filling and Sizing recomputed variables (without values)
////	for (int i=0;i<	_project->getModel()->variables->size();i++)
////	{
////		result->recomputedVariables()->addItem(new VariableResult(_project->getModel()->variables->at(i)));
////		//result->recomputedVariables()->at(i)->finalValues.resize(nbPoints);
////	}
////
////	// Filling final values from frontFile (csv)
////	QFileInfo tempInfo = QFileInfo(result->getSavePath());
////	QString folder = tempInfo.absoluteDir().absolutePath();
////	QString frontFilePath = folder + QDir::separator() + result->frontFileName;
////	loadOptimValuesFromFrontFile(result,frontFilePath);
////
////
////
////	// Looking for recomputed points
////	result->updateRecPointsFromFolder();
////
////
////	return result;
////}

OMCase* Load::newOMCase(QString filePath,Project* project)
{
    QString error;
    OMCase* omCase = NULL;

    QDomDocument doc( "OMCase" );
    QFile file(filePath);
    if( !file.open( QIODevice::ReadOnly ) )
    {
        infoSender.send( Info(ListInfo::PROBLEMFILENOTEXISTS,filePath));
        return NULL;
    }
    else if( !doc.setContent(&file,&error) )
    {
        file.close();
        infoSender.send( Info(ListInfo::PROBLEMFILECORRUPTED,filePath));
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

    // look for corresponding problem interface
    ProblemInterface* interface = project->problemsInterfaces().interfaceOf(problemType);

    // load
    if(interface)
    {
        if(isAResult)
            omCase = interface->loadResult(QFileInfo(file),domCase,project);
        else
            omCase = interface->loadProblem(QFileInfo(file),domCase,project);
    }
    else
    {
        infoSender.sendError("Following problem type not managed : "+problemType+" ["+filePath+"]");
    }

    return omCase;
}


//Result* Load::newResult(QString filePath,Project* project)
//{
//    QDomDocument doc( "OMCase" );
//    QFile file(filePath);
//    if( !file.open( QIODevice::ReadOnly ) )
//    {
//        infoSender.send( Info(ListInfo::PROBLEMFILENOTEXISTS,filePath));
//        return NULL;
//    }
//    else if( !doc.setContent(&file,&error) )
//    {
//        file.close();
//        infoSender.send( Info(ListInfo::PROBLEMFILECORRUPTED,filePath));
//        return NULL;
//    }
//    file.close();

//    QDomElement domCase = doc.firstChildElement("OMCase");
//    // create problem
//    QDomElement domProblem = domCase.firstChildElement("OMProblem");
//    Problem* problem = newProblem(QFileInfo(),domProblem,project);

//    if(!problem)
//    {
//        infoSender.send( Info(ListInfo::RESULTFILECORRUPTED,filePath));
//        return NULL;
//    }

//    // create result
//    QDomElement domResult = domCase.firstChildElement("OMResult");
//    Result* result = newResult(domResult,project,*problem,filePath);

//    if(!result)
//    {
//        infoSender.send( Info(ListInfo::RESULTFILECORRUPTED,filePath));
//        return NULL;
//    }

//    // attribute problem to result
//    if(result)
//    {
//        // attribute file path to result
//        result->setEntireSavePath(filePath);
//    }

//    // can delete problem since it has been copied in result
//    delete problem;

//    return result;
//}

//Result* Load::newResult(QDomElement domResult,Project* project,const Problem & problem,QString filePath)
//{
//    if(domResult.isNull())
//        return NULL;

//    Q_ASSERT(domResult.tagName()=="OMResult");

//    Result* result = NULL;

//    QDomElement resultRoot = domResult.firstChild().toElement();
//    QString resultType = resultRoot.tagName();

//    QFileInfo fileInfo(filePath);
//    bool ok = true;
//    // read result
//    if (resultType==OneSimResult::className())
//    {
//        const OneSimulation & oneSim = dynamic_cast<const OneSimulation &>(problem);
//        result = new OneSimResult(project,resultRoot,oneSim,ok);
//    }
//    if (resultType==OptimResult::className())
//    {
//         const Optimization & optim = dynamic_cast<const Optimization &>(problem);
//         result = new OptimResult(project,resultRoot,optim,fileInfo.absoluteDir(),ok);
//    }

//#ifdef USEEI
//    if (resultType==EITargetResult::className())
//        result = new EITargetResult(project,resultRoot,problem);
//    if (resultType==EIHEN1Result::className())
//        result = new EIHEN1Result(project,resultRoot,problem);
//#endif

//    if(!ok)
//    {
//        delete result;
//        result = NULL;
//    }

//    return result;
//}

//Problem* Load::newProblem(QString filePath,Project* project)
//{
//    QString error;

//    QDomDocument doc( "OMCase" );
//    QFile file(filePath);
//    if( !file.open( QIODevice::ReadOnly ) )
//    {
//        infoSender.send( Info(ListInfo::PROBLEMFILENOTEXISTS,filePath));
//        return NULL;
//    }
//    else if( !doc.setContent(&file,&error) )
//    {
//        file.close();
//        infoSender.send( Info(ListInfo::PROBLEMFILECORRUPTED,error,filePath));
//        return NULL;
//    }
//    file.close();

//    // Read first nodes
//    QDomElement domCase = doc.firstChildElement("OMCase");
//    QDomElement domProblem = domCase.firstChildElement("OMProblem");

//    if(domProblem.isNull())
//        return NULL;

//    // get problemType
//    QDomElement problemRoot = domProblem.firstChildElement();
//    QString problemType = problemRoot.tagName();

//    // look for corresponding problem interface
//    ProblemInterface* interface = project->problemsInterfaces().interfaceOf(problemType);

//    // load
//    if(interface)
//    {
//        problem = interface->load(file,domCase,project);
//    }

//    return problem;
//}



bool Load::loadModModelPlus(Project* project,QString mmoFilePath)
{
    infoSender.send( Info(ListInfo::LOADINGMODEL,mmoFilePath));

    // Open file
    QDomDocument doc( "MOModelXML" );
    QFileInfo mmoFileInfo(mmoFilePath);
    QDir mmoDir(mmoFileInfo.absolutePath());
    QFile file(mmoFilePath);
    if( !file.open( QIODevice::ReadOnly ) )
    {
        infoSender.send( Info(ListInfo::MODELFILENOTEXISTS,mmoFilePath));
        return false;
    }
    QString error;
    if( !doc.setContent( &file,&error ) )
    {
        file.close();
        infoSender.send( Info(ListInfo::MODMODELFILECORRUPTED,error,mmoFilePath));
        return false;
    }
    file.close();
    QDomElement root = doc.documentElement();
    if( root.tagName() != "MOModel" )
    {
        error = "Root tagname should be <MOModel>";
        infoSender.send( Info(ListInfo::MODMODELFILECORRUPTED,error,mmoFilePath));
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
        ModModelPlus* newModelPlus = new ModModelPlus(project,modModel);
        // Other files
        QDomElement domOtherFiles = root.firstChildElement("OtherFiles");
        QString allOtherFiles = domOtherFiles.attribute( "list", "" );
        newModelPlus->setOtherFiles(allOtherFiles.split(";"));


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


