// $Id$
/**
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linkpings universitet, Department of Computer and Information Science,
 * SE-58183 Linkping, Sweden.
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
#include "Optimization.h"
#include "OneSimResult.h"
#include "Project.h"
#include "MOVector.h"
#include "CSV.h"
#include "EABase.h"
#include "OptimResult.h"
#include "version.h"

Save::Save(void)
{
}

Save::~Save(void)
{
}
//void Save::saveResult(Result* result)
//{
//	switch (result->problemType())
//	{
//	case Problem::ONESIMULATIONTYPE:
//		saveResult((OneSimResult*) result);
//		break;
//	case Problem::OPTIMIZATIONTYPE:
//		saveResult((OptimResult*) result);
//		break;
//	}
//}
//void Save::saveResult(OneSimResult* result)
//{
//	result->save();
//}

//void Save::saveResult(OptimResult* result)
//{
//
//	QString rootName;
//
//	rootName = result->name();
//	
//	// Root element
//	QDomDocument doc("MOResult");
//	QDomElement root = doc.createElement("OptimResult");
//	doc.appendChild(root);
//
//	// Problem definition
//	QDomElement cResult = doc.createElement("Result");
//	cResult.setAttribute("name", result->name());
//	cResult.setAttribute("type", result->problemType());
//	cResult.setAttribute("EAused",result->iEA);
//	cResult.setAttribute("date", result->date.toString());
//	cResult.setAttribute("hour", result->hour.toString());
//	cResult.setAttribute("comptime", result->computationTime.toString());
//	root.appendChild(cResult);
//
//
//	// Opt objectives
//	QDomElement cOptObjectives = doc.createElement("OptObjectives");
//	root.appendChild(cOptObjectives);
//	QString strOptObjectives = "\n"+CSV::optObjectivesToLines(result->optObjectivesResults);
//	QDomText tOptObjectives = doc.createTextNode(strOptObjectives);
//	cOptObjectives.appendChild(tOptObjectives);
//
//
//	// Opt variables
//	QDomElement cOptVariables = doc.createElement("OptVariables");
//	root.appendChild(cOptVariables);
//	QString strOptVariables = "\n"+CSV::optVariablesToLines(result->optVariablesResults);
//	QDomText tOptVariables = doc.createTextNode(strOptVariables);
//	cOptVariables.appendChild(tOptVariables);
//
//	// EA Parameters
//	QDomElement cEAParams = doc.createElement("AlgoParameters");
//	root.appendChild(cEAParams);
//	QString strEAParams = "\n"+result->eaConfig->parameters->toSavingString();
//	QDomText tEAParams = doc.createTextNode(strEAParams);
//	cEAParams.appendChild(tEAParams);
//
//	// Front File (containing values on pareto curve)
//	QDomElement cFrontFile = doc.createElement("FrontFile");
//	cFrontFile.setAttribute("path", result->frontFileName);
//	root.appendChild(cFrontFile);
//
//	// Writing points in Front File
//	QFileInfo tempInfo = QFileInfo(result->getSavePath());
//	QString folder = tempInfo.absoluteDir().absolutePath();
//	QFile frontFile(folder + QDir::separator() + result->frontFileName);
//	if(frontFile.exists())
//	{
//		frontFile.remove();
//	}
//	frontFile.open(QIODevice::WriteOnly);
//	QTextStream tsfront( &frontFile );
//	tsfront << result->buildFrontCSV();
//	frontFile.close();
//
//
//
//	// Writing Result file
//	QFile file(result->getSavePath());
//	if(file.exists())
//	{
//		file.remove();
//	}
//	file.open(QIODevice::WriteOnly);
//	QTextStream ts( &file );
//	ts << doc.toString();
//	file.close();
//
//}



void Save::saveProblem(Problem* problem)
{
    // Root element
    QDomDocument doc("OMCase");
    QDomElement caseRoot = doc.createElement("OMCase");
    doc.appendChild(caseRoot);

    QDomElement problemRoot = doc.createElement("OMProblem");
    QDomElement problemEl = problem->toXmlData(doc);
    caseRoot.appendChild(problemRoot);
    problemRoot.appendChild(problemEl);

    // Writing to file
    if(!problem->saveFileName().isEmpty())
    {
	QFile file(problem->entireSavePath());
	QFileInfo fileInfo(problem->entireSavePath());
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
}

void Save::saveResult(Result* result)
{
    switch (result->problemType())
    {
    case Problem::OPTIMIZATIONTYPE:
        saveOptimResult((OptimResult*) result);
        break;
    default :
        saveStdResult(result);
        break;

    }
}
void Save::saveStdResult(Result* result)
{
    // Root element
    QDomDocument doc("OMCase");
    QDomElement caseRoot = doc.createElement("OMCase");
    doc.appendChild(caseRoot);

    //***********************
    // Problem definition
    //***********************
    // Root element
    QDomElement problemRoot = doc.createElement("OMProblem");
    QDomElement problemEl;

    if(result->problem())
    {
        problemEl = result->problem()->toXmlData(doc);
        problemRoot.appendChild(problemEl);
        caseRoot.appendChild(problemRoot);
    }
    //*********************
    // Result definition
    //*********************
    QDomElement resultRoot = doc.createElement("OMResult");
    QDomElement resultEl = result->toXmlData(doc);
    resultRoot.appendChild(resultEl);
    caseRoot.appendChild(resultRoot);

    // Writing to file
    if(!result->saveFileName().isEmpty())
    {
        QFile file(result->entireSavePath());
        QFileInfo fileInfo(result->entireSavePath());
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

}



void Save::saveOptimResult(OptimResult* result)
{
    // Root element
    QDomDocument doc("OMCase");
    QDomElement caseRoot = doc.createElement("OMCase");
    doc.appendChild(caseRoot);

    //***********************
    // Problem definition
    //***********************
    QDomElement problemRoot = doc.createElement("OMProblem");
    QDomElement problemEl;
    if(result->problem())
    {
        problemEl= result->problem()->toXmlData(doc);
        caseRoot.appendChild(problemRoot);
        problemRoot.appendChild(problemEl);
    }

    //*********************
    // Result definition
    //*********************
    QDomElement resultRoot = doc.createElement("OMResult");
    QDomElement resultEl = result->toXmlData(doc);
    resultRoot.appendChild(resultEl);
    caseRoot.appendChild(resultRoot);

    // Writing to file
    QFile file(result->entireSavePath());
    QFileInfo fileInfo(result->entireSavePath());
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

    //*********************************
    // Writing points in Front File
    //*********************************

    QString optVarsfrontFileName = result->_optVarsFrontFileName;
    QString allVarsfrontFileName = result->_allVarsFrontFileName;

    result->exportFrontCSV(dir.absoluteFilePath(optVarsfrontFileName), false);
    result->exportFrontCSV(dir.absoluteFilePath(allVarsfrontFileName), true);
}




void Save::saveProject(Project* project)
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
    QStringList moFilesPath = project->moFiles();
    if(moFilesPath.size()>0)
    {
        QDomElement cMoFiles = doc.createElement("MoFiles");
        for(int i=0;i<moFilesPath.size();i++)
        {
            QDomElement cMoFile = doc.createElement("MoFile");
            cMoFile.setAttribute("path",moFilesPath.at(i));
            cMoFiles.appendChild(cMoFile);
        }
        root.appendChild(cMoFiles);
    }


    // Mmo files
    QStringList mmoFilesPath = project->mmoFiles();
    if(mmoFilesPath.size()>0)
    {
        QDomElement cMmoFiles = doc.createElement("MmoFiles");
        for(int i=0;i<mmoFilesPath.size();i++)
        {
            QDomElement cMmoFile = doc.createElement("MmoFile");
            relPath = projectDir.relativeFilePath(mmoFilesPath.at(i));
            cMmoFile.setAttribute("path",relPath);
            cMmoFiles.appendChild(cMmoFile);
        }
        root.appendChild(cMmoFiles);
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

    // Saving solved OMCases
    for (int nr=0;nr<project->results()->size();nr++)
    {
        Save::saveResult(project->results()->at(nr));
    }

    // Saving OMCases
    for (int nr=0;nr<project->problems()->size();nr++)
    {
        Save::saveProblem(project->problems()->at(nr));
    }

    // Saving ModModelPlus
    QList<ModModelPlus*> allModModelPlus = project->allModModelPlus();
    for (int m=0;m<allModModelPlus.size();m++)
    {
        Save::saveModModelPlus(allModModelPlus.at(m));
    }
}


void Save::saveModModelPlus(ModModelPlus* modModelPlus)
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

    // OtherFiles
    QDomElement cOtherFiles = doc.createElement( "OtherFiles" );
    QString strOtherFiles;
    for (int nof=0;nof<modModelPlus->otherFiles().size();nof++)
    {
        strOtherFiles.append(modModelPlus->otherFiles().at(nof)+";");
    }
    cOtherFiles.setAttribute("list",strOtherFiles);
    root.appendChild(cOtherFiles);

    // Infos
    QDomElement cInfos = doc.createElement( "Infos" );
    cInfos.setAttribute("text",modModelPlus->infos());
    root.appendChild(cInfos);

    // Variables
    QDomElement cVariables = modModelPlus->variables()->toXmlData(doc,"Variables");
    root.appendChild(cVariables);

    // Controlers
    QDomElement cControlers = doc.createElement("Controlers");
    cControlers.setAttribute("curType",(int)modModelPlus->ctrlType());


    // Controler parameters
    QList<QDomElement> cCtrls;
    QList<QDomElement> cCtrlsParams;
    QDomElement ccurCtrl;
    QDomElement ccurParams;
    ModPlusCtrl* curCtrl;
    for(int iCtrl=0;iCtrl<modModelPlus->ctrls()->values().size();iCtrl++)
    {
        curCtrl = modModelPlus->ctrls()->values().at(iCtrl);
        ccurCtrl = doc.createElement("Controler");
        ccurCtrl.setAttribute("type",(int)curCtrl->type());
        ccurParams = curCtrl->parameters()->toXmlData(doc,"parameters");

        ccurCtrl.appendChild(ccurParams);
        cControlers.appendChild(ccurCtrl);

        cCtrls.push_back(ccurCtrl);
        cCtrlsParams.push_back(ccurParams);
    }

    root.appendChild(cControlers);

    // .mo dependencies
    QDomElement cMoDeps = doc.createElement( "moDependencies" );
    QString strMoDeps;
    for (int nof=0;nof<modModelPlus->moDependencies().size();nof++)
    {
        strMoDeps.append(modModelPlus->moDependencies().at(nof)+";");
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

