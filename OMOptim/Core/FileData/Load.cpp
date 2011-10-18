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
    // Reading problems
    //**************************************
    for(int i=0;i<problemsPaths.size();i++)
        _project->addProblem(problemsPaths.at(i));

    //**************************************
    // Reading results
    //**************************************
    for(int i=0;i<resultsPaths.size();i++)
        _project->addResult(resultsPaths.at(i));

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

Result* Load::newResult(QString filePath,Project* project)
{
    QString error;

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
    // create problem
    QDomElement domProblem = domCase.firstChildElement("OMProblem");
    Problem* problem = newProblem(domProblem,project);

    if(!problem)
    {
        infoSender.send( Info(ListInfo::RESULTFILECORRUPTED,filePath));
        return NULL;
    }

    // create result
    QDomElement domResult = domCase.firstChildElement("OMResult");
    Result* result = newResult(domResult,project,problem,filePath);

    if(!result)
    {
        infoSender.send( Info(ListInfo::RESULTFILECORRUPTED,filePath));
        return NULL;
    }

    // attribute problem to result
    if(result)
    {
        result->setProblem(problem);

        // attribute file path to result
        result->setEntireSavePath(filePath);
    }

    return result;
}

Result* Load::newResult(QDomElement domResult,Project* project,Problem* problem,QString filePath)
{
    if(domResult.isNull())
        return NULL;

    Q_ASSERT(domResult.tagName()=="OMResult");

    Result* result = NULL;

    QDomElement resultRoot = domResult.firstChild().toElement();
    QString resultType = resultRoot.tagName();

    QFileInfo fileInfo(filePath);

    // read result
    if (resultType==OneSimResult::className())
        result = newOneSimulationResult(resultRoot,project,dynamic_cast<OneSimulation*>(problem));
    if (resultType==OptimResult::className())
        result = newOptimizationResult(resultRoot,project,dynamic_cast<Optimization*>(problem),fileInfo.absoluteDir());

#ifdef USEEI
    if (resultType==EITargetResult::className())
        result = new EITargetResult(project,project->modClassTree(),resultRoot,problem);
    if (resultType==EIHEN1Result::className())
        result = new EIHEN1Result(project,project->modClassTree(),resultRoot,problem);
#endif


    return result;
}

Problem* Load::newProblem(QString filePath,Project* project)
{
    QString error;

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
        infoSender.send( Info(ListInfo::PROBLEMFILECORRUPTED,error,filePath));
        return NULL;
    }
    file.close();

    // create problem
    QDomElement domCase = doc.firstChildElement("OMCase");
    QDomElement domProblem = domCase.firstChildElement("OMProblem");
    Problem* problem = newProblem(domProblem,project);

    // attribute filepath and project to problem
    if(problem)
    {
        problem->setProject(project);
        problem->setEntireSavePath(filePath);
    }

    return problem;
}

Problem* Load::newProblem(QDomElement domProblem,Project* project)
{

    if(domProblem.isNull())
        return NULL;

    Q_ASSERT(domProblem.tagName()=="OMProblem");

    Problem* problem = NULL;

    QDomElement problemRoot = domProblem.firstChildElement();
    QString problemType = problemRoot.tagName();

    if (problemType==OneSimulation::className())
        problem = newOneSimulation(problemRoot,project);

    if (problemType==Optimization::className())
        problem = newOptimization(problemRoot,project);

#ifdef USEEI
    if (problemType==EITarget::className())
    {
        problem = new EITarget(project,project->modClassTree(),project->moomc(),problemRoot);
    }
    if (problemType==EIHEN1Problem::className())
    {
        problem = new EIHEN1Problem(project,project->modClassTree(),project->moomc(),problemRoot);
    }
    if (problemType==EIProblem::className())
    {
        problem = new EIProblem(project,project->modClassTree(),project->moomc(),problemRoot);
    }

#endif

    return problem;
}



Problem* Load::newOneSimulation(QDomElement domProblem,Project* project)
{
    if(domProblem.isNull())
        return NULL;

    Q_ASSERT(domProblem.tagName()==OneSimulation::className());


    QDomElement domInfos = domProblem.firstChildElement("Infos");
    QString modelName = domInfos.attribute("model");

    // Find model
    ModModel* modModel = project->findModModel(modelName);
    if(modModel == NULL)
    {
        infoSender.sendWarning("Unable to find model "+modelName);
        return NULL;
    }

    ModModelPlus* modModelPlus = project->modModelPlus(modModel);
    OneSimulation* problem= new OneSimulation(project,project->modClassTree(),
                                              modModelPlus);

    // Infos
    problem->setType((Problem::ProblemType)domInfos.attribute("type", "" ).toInt());
    problem->setName(domInfos.attribute("name", "" ));

    // Scanned Variables
    QDomElement domScanVars = domProblem.firstChildElement("ScannedVariables");
    problem->scannedVariables()->setItems(domScanVars);

    // Overwrited Variables
    QDomElement domOverVars = domProblem.firstChildElement("OverwritedVariables");
    problem->overwritedVariables()->setItems(domOverVars);

    // Files to copy
    QDomElement cFilesToCopy = domProblem.firstChildElement("FilesToCopy");
    QString text = cFilesToCopy.text();
    problem->_filesToCopy = text.split("\n",QString::SkipEmptyParts);

    // addOverWritedCVariables
    // make their value editable
    for(int iV=0;iV<problem->overwritedVariables()->size();iV++)
        problem->overwritedVariables()->at(iV)->setIsEditableField(Variable::VALUE,true);

    return problem;
}

Result* Load::newOneSimulationResult(QDomElement domResult,Project* project,OneSimulation *problem)
{
    if(domResult.isNull())
        return NULL;

    OneSimResult* result = new OneSimResult(project,problem->modModelPlus(),problem,project->modClassTree());

    Q_ASSERT(domResult.tagName()==OneSimResult::className());

    //Infos
    QDomElement domResInfos = domResult.firstChildElement("Infos");
    result->setName(domResInfos.attribute("name", "" ));

    //FinalVariables
    QDomElement domFinalVars = domResult.firstChildElement("FinalVariables");
    result->finalVariables()->setItems(domFinalVars);


    return result;
}





Problem* Load::newOptimization(QDomElement domProblem,Project* project)
{
    if(domProblem.isNull())
        return NULL;

    Q_ASSERT(domProblem.tagName()==Optimization::className());


    QDomElement domInfos = domProblem.firstChildElement("Infos");
    QString modelName = domInfos.attribute("model");
    QString problemName = domInfos.attribute("name");


    // Find model
    ModModel* _modModel = project->findModModel(modelName);
    if(_modModel == NULL)
    {
        infoSender.send( Info(ListInfo::PROBLEMMODELNOTFOUND,modelName,problemName));
        return NULL;
    }

    // Create problem
    ModModelPlus* _modModelPlus = project->modModelPlus(_modModel);
    Optimization* problem= new Optimization(project,project->modClassTree(),
                                            _modModelPlus);
    // Infos
    problem->setType((Problem::ProblemType)domInfos.attribute("type", "" ).toInt());
    problem->setName(problemName);

    // Optimized Variables
    QDomElement domOptVars = domProblem.firstChildElement("OptimizedVariables");
    problem->optimizedVariables()->setItems(domOptVars);

    // Objectives
    QDomElement domObj = domProblem.firstChildElement("Objectives");
    problem->objectives()->setItems(domObj);

    // Scanned Variables
    QDomElement domScann = domProblem.firstChildElement("ScannedVariables");
    problem->scannedVariables()->setItems(domScann);

    // Files to copy
    QDomElement cFilesToCopy = domProblem.firstChildElement("FilesToCopy");
    QString text = cFilesToCopy.text();
    problem->_filesToCopy = text.split("\n",QString::SkipEmptyParts);


    // BlockSubstitutions
    QDomElement domBlockSubs = domProblem.firstChildElement("BlockSubstitutions");
    problem->setBlockSubstitutions(new BlockSubstitutions(project,_modModelPlus,domBlockSubs,project->modClassTree()));

    // EA
    QDomElement domEA = domProblem.firstChildElement("EA");
    QDomElement domEAInfos = domEA.firstChildElement("Infos");
    if(!domEAInfos.isNull())
    {
        problem->setiCurAlgo(domEAInfos.attribute("num", "" ).toInt());
    }
    QDomElement domEAParameters = domEA.firstChildElement("Parameters");
    if(!domEAParameters.isNull())
    {
        problem->getCurAlgo()->_parameters->update(domEAParameters);
    }

    return problem;
}





Result* Load::newOptimizationResult(QDomElement domResult,Project* project,Optimization* optimization,QDir resultDir)
{

    if(domResult.isNull())
        return NULL;

    if(optimization==NULL)
        return NULL;

    Q_ASSERT(domResult.tagName()==OptimResult::className());

    OptimResult* result = new OptimResult(project,optimization->modModelPlus(),optimization,project->modClassTree(),optimization->getCurAlgo());
    result->setSuccess(true);

    //**********
    // Result
    //**********
    // OptVarResult from optVar, OptObjResult from OptObj...
    result->optVariablesResults()->clear();
    for(int i=0;i<optimization->optimizedVariables()->size();i++)
    {
        result->optVariablesResults()->addItem(new VariableResult(*optimization->optimizedVariables()->at(i)));
    }

    result->optObjectivesResults()->clear();
    for(int i=0;i<optimization->objectives()->size();i++)
    {
        result->optObjectivesResults()->addItem(new VariableResult(*optimization->objectives()->at(i)));
    }

    //Infos
    QDomElement domResInfos = domResult.firstChildElement("Infos");
    result->setName(domResInfos.attribute("name", "" ));
    QString date = domResInfos.attribute("date");
    result->_date = QDateTime::fromString(date);
    QString duration = domResInfos.attribute("duration");
    result->_duration = QTime::fromString(duration);






    // Blocksubs
    QDomElement domBlocks = domResult.firstChildElement("AllBlockSubstitutions");
    QDomElement domBlock = domBlocks.firstChildElement();
    QRegExp regExp("BlockSubstitutions[\\d]+");

    while(!domBlock.isNull())
    {
        if(regExp.exactMatch(domBlock.tagName()))
        {
            QString number = domBlock.tagName();
            number.remove(QRegExp("[\\D]*"));
            domBlock.setTagName("BlockSubstitutions");

            result->_subBlocks.push_back(new BlockSubstitutions(project,result->modModelPlus(),domBlock,project->modClassTree()));
        }
        domBlock = domBlock.nextSiblingElement();
    }

    // Filling and Sizing recomputed variables (without values)
    if(result->modModelPlus()->variables()->items.isEmpty())
        result->modModelPlus()->readVariables();

    for (int i=0;i<result->modModelPlus()->variables()->size();i++)
    {
        result->recomputedVariables()->addItem(new VariableResult(*result->modModelPlus()->variables()->at(i)));
    }

    // Filling final values from frontFile (csv)


    QFileInfo frontFileInfo(resultDir,result->_optVarsFrontFileName);
    if(frontFileInfo.exists())
        loadOptimValuesFromFrontFile(result,frontFileInfo.absoluteFilePath());

    // Filling recomputed values from folder point_ (csv)
    result->updateRecomputedPointsFromFolder();

    return result;
}




void Load::loadOptimValuesFromFrontFile(OptimResult* _result,QString fileName)
{
    QFile frontFile(fileName);
    if(!frontFile.exists())
    {
        return;
    }
    frontFile.open(QIODevice::ReadOnly);
    QTextStream tsfront( &frontFile );
    QString text = tsfront.readAll();
    frontFile.close();

    // Clearing previous values
    for (int i=0; i<_result->optObjectivesResults()->size(); i++)
    {
        _result->optObjectivesResults()->at(i)->clearFinalValues();
    }
    for (int i=0; i<_result->optVariablesResults()->size(); i++)
    {
        _result->optVariablesResults()->at(i)->clearFinalValues();
    }
    for (int i=0; i<_result->recomputedVariables()->size(); i++)
    {
        _result->recomputedVariables()->at(i)->clearFinalValues();
    }

    QStringList lines = text.split("\n",QString::KeepEmptyParts);

    QStringList firstLine = lines[0].split("\t",QString::SkipEmptyParts);
    int nbCols = firstLine.size();

    int *objIndex = new int[nbCols];
    int *optVarIndex = new int[nbCols];
    int *recompVarIndex = new int[nbCols];

    bool useScan = (dynamic_cast<Optimization*>(_result->problem())->nbScans()>1);
    _result->recomputedVariables()->setUseScan(useScan);

    // Filling index tables
    for(int i=0; i<nbCols; i++)
    {
        objIndex[i] = _result->optObjectivesResults()->findItem(firstLine.at(i));
        optVarIndex[i] = _result->optVariablesResults()->findItem(firstLine.at(i));
        if(!useScan)
            recompVarIndex[i] = _result->recomputedVariables()->findItem(firstLine.at(i));
    }

    int iiSubBlock = firstLine.indexOf("subBlocksIndex");

    // Filling variables and objectives values
    QStringList curLine;
    int curIndex = 0; // to skip empty, partial or comment lines
    for (int iLine = 1; iLine<lines.size(); iLine++)
    {
        curLine = lines[iLine].split("\t",QString::SkipEmptyParts);

        if(curLine.size()==nbCols)
        {
            for (int iCol = 0; iCol < nbCols; iCol++)
            {
                if (objIndex[iCol]>-1)
                {
                    _result->optObjectivesResults()->at(objIndex[iCol])->appendFinalValue(curLine[iCol].toDouble(),0);
                }
                if (optVarIndex[iCol]>-1)
                {
                    _result->optVariablesResults()->at(optVarIndex[iCol])->appendFinalValue(curLine[iCol].toDouble(),0);
                }
                if ((recompVarIndex[iCol]>-1)&&(!useScan))
                {
                    _result->recomputedVariables()->at(recompVarIndex[iCol])->appendFinalValue(curLine[iCol].toDouble(),0);
                }
            }

            if(iiSubBlock>-1)
                _result->_iSubModels.push_back(curLine[iiSubBlock].toInt());

            curIndex ++;
        }
    }
}





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
    ModModel* _modModel = project->findModModel(modelName);
    if(_modModel==NULL)
        return false;
    else
    {
        ModModelPlus* newModelPlus = new ModModelPlus(project->moomc(),project,project->modClassTree(),
                                                      _modModel);
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

        // Controler type
        QDomElement cControlers = root.firstChildElement("Controlers");
        if(!cControlers.isNull())
            newModelPlus->setCtrlType((ModPlusCtrl::Type)cControlers.attribute("curType","").toInt());

        // Controler parameters
        QDomNodeList domControlerList = cControlers.elementsByTagName("Controler");
        ModPlusCtrl::Type _type;

        QDomElement cParams;
        QDomElement cControler;
        ModPlusCtrl* curCtrl;
        for(int iC=0;iC<domControlerList.size();iC++)
        {
            cControler = domControlerList.at(iC).toElement();
            _type = (ModPlusCtrl::Type)cControler.attribute("type","-1").toInt();
            if(_type>-1)
            {
                cParams = cControler.firstChildElement("parameters");

                if(!cParams.isNull())
                {
                    curCtrl = newModelPlus->ctrls()->value(_type);
                    if(curCtrl)
                        curCtrl->parameters()->update(cParams);
                }
            }
        }

        bool ok = project->addModModelPlus(newModelPlus);
        return ok;
    }
}


