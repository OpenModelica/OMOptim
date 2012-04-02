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

        @file OptimResult.cpp
        @brief Comments for file documentation.
        @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
        Company : CEP - ARMINES (France)
        http://www-cep.ensmp.fr/english/
  @version

  */
#include "OptimResult.h"
#include "Optimization.h"
#include "CSVBase.h"
#include "Project.h"

OptimResult::OptimResult():Result()
{
    _recomputedVariables = new MOOptVector(true,true,true);
    _optObjectivesResults = new MOOptVector(true,false,true); //objectives are constant for one scan
    _optVariablesResults= new MOOptVector(true,false,true); //optimized parameters are constant for one scan

    // files to copy
    _filesToCopy << "iteration*.sav";

    _optVarsFrontFileName = "optVarsFront.csv";
    _allVarsFrontFileName = "allVarsFront.csv";

    _algo = NULL;
    _curPoint = -1;
    _curScan = -1;
}

OptimResult::OptimResult(Project* project, const Optimization & problem)
    :Result((ProjectBase*)project,problem)
{
    _omProject = project;
    _models = problem.models();

    _recomputedVariables = new MOOptVector(true,true,true);
    _optObjectivesResults = new MOOptVector(true,false,true); //objectives are constant for one scan
    _optVariablesResults= new MOOptVector(true,false,true); //optimized parameters are constant for one scan

    // files to copy
    _filesToCopy << "iteration*.sav";

    _optVarsFrontFileName = "optVarsFront.csv";
    _allVarsFrontFileName = "allVarsFront.csv";

    _curPoint = -1;
    _curScan = -1;

    _algo = problem.getCurAlgo()->clone();

}

OptimResult::OptimResult(Project* project,const QDomElement & domResult,const Optimization & problem,QDir resultDir,bool &ok)
    :Result((ProjectBase*)project,(const Problem&)problem)
{
    ok = true;
    _omProject = project;
    _models = problem.models();
    this->setSaveFolder(resultDir.absolutePath());


    _recomputedVariables = new MOOptVector(true,true,true);
    _optObjectivesResults = new MOOptVector(true,false,true); //objectives are constant for one scan
    _optVariablesResults= new MOOptVector(true,false,true); //optimized parameters are constant for one scan
    _algo = NULL;

    if(domResult.isNull() || (domResult.tagName()!=OptimResult::className()))
    {
        ok = false;
        return;
    }

    // files to copy
    _filesToCopy << "iteration*.sav";

    _optVarsFrontFileName = "optVarsFront.csv";
    _allVarsFrontFileName = "allVarsFront.csv";

    _curPoint = -1;
    _curScan = -1;

    _algo = problem.getCurAlgo()->clone();

    setSuccess(true);

    //**********
    // Result
    //**********
    // OptVarResult from optVar, OptObjResult from OptObj...
    this->optVariablesResults()->clear();
    for(int i=0;i<problem.optimizedVariables()->size();i++)
    {
        this->optVariablesResults()->addItem(new VariableResult(*problem.optimizedVariables()->at(i)));
    }

    this->optObjectivesResults()->clear();
    for(int i=0;i<problem.objectives()->size();i++)
    {
        this->optObjectivesResults()->addItem(new VariableResult(*problem.objectives()->at(i)));
    }

    //Infos
    QDomElement domResInfos = domResult.firstChildElement("Infos");
    this->setName(domResInfos.attribute("name", "" ));
    QString date = domResInfos.attribute("date");
    this->_date = QDateTime::fromString(date);
    QString duration = domResInfos.attribute("duration");
    this->_duration = QTime::fromString(duration);


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

            this->_subBlocks.push_back(new BlockSubstitutions(project,domBlock));
        }
        domBlock = domBlock.nextSiblingElement();
    }

    // Filling and Sizing recomputed variables (without values)
    ModModelPlus* curModel;
    for(int iM=0;iM<_models.size();iM++)
    {
        curModel = _omProject->modModelPlus(_models.at(iM));

        if(curModel->variables()->items.isEmpty())
            curModel->readVariables(problem.ctrl(_models.at(iM)));

        for (int i=0;i<curModel->variables()->size();i++)
        {
            this->recomputedVariables()->addItem(new VariableResult(*curModel->variables()->at(i)));
        }
    }

    // Filling final values from frontFile (csv)
    QFileInfo frontFileInfo(resultDir,this->_optVarsFrontFileName);
    if(frontFileInfo.exists())
        loadOptimValuesFromFrontFile(frontFileInfo.absoluteFilePath());

    // Filling recomputed values from folder point_ (csv)
    this->updateRecomputedPointsFromFolder();

}

OptimResult::OptimResult(const OptimResult &res)
    :Result(res)
{
    _algo = res._algo->clone();

    _optVariablesResults = res._optVariablesResults->clone();
    _optObjectivesResults = res._optObjectivesResults->clone();
    _recomputedVariables = res._recomputedVariables->clone();

    _curScan = res._curScan;
    _curPoint = res._curPoint;

    _optVarsFrontFileName = res._optVarsFrontFileName;
    _allVarsFrontFileName = res._allVarsFrontFileName;
}

OptimResult::~OptimResult(void)
{
    delete _optVariablesResults;
    delete _optObjectivesResults;
    delete _recomputedVariables;
    if(_algo)
        delete _algo;
}

void OptimResult::loadOptimValuesFromFrontFile(QString fileName)
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
    for (int i=0; i<this->optObjectivesResults()->size(); i++)
    {
        this->optObjectivesResults()->at(i)->clearFinalValues();
    }
    for (int i=0; i<this->optVariablesResults()->size(); i++)
    {
        this->optVariablesResults()->at(i)->clearFinalValues();
    }
    for (int i=0; i<this->recomputedVariables()->size(); i++)
    {
        this->recomputedVariables()->at(i)->clearFinalValues();
    }

    QStringList lines = text.split("\n",QString::KeepEmptyParts);

    QStringList firstLine = lines[0].split("\t",QString::SkipEmptyParts);
    int nbCols = firstLine.size();

    int *objIndex = new int[nbCols];
    int *optVarIndex = new int[nbCols];
    int *recompVarIndex = new int[nbCols];

    bool useScan = (dynamic_cast<Optimization*>(this->problem())->nbScans()>1);
    this->recomputedVariables()->setUseScan(useScan);

    // Filling index tables
    for(int i=0; i<nbCols; i++)
    {
        objIndex[i] = this->optObjectivesResults()->findItem(firstLine.at(i));
        optVarIndex[i] = this->optVariablesResults()->findItem(firstLine.at(i));
        if(!useScan)
            recompVarIndex[i] = this->recomputedVariables()->findItem(firstLine.at(i));
    }

    int iiSubBlock = firstLine.indexOf("subBlocksIndex");

    // Filling variables and objectives values
    QStringList curLine;
    int iPoint=0;
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
                    this->optObjectivesResults()->at(objIndex[iCol])->setFinalValue(0,iPoint,curLine[iCol].toDouble());
                }
                if (optVarIndex[iCol]>-1)
                {
                    this->optVariablesResults()->at(optVarIndex[iCol])->setFinalValue(0,iPoint,curLine[iCol].toDouble());
                }
                if ((recompVarIndex[iCol]>-1)&&(!useScan))
                {
                    this->recomputedVariables()->at(recompVarIndex[iCol])->setFinalValue(0,iPoint,curLine[iCol].toDouble());
                }
            }

            if(iiSubBlock>-1)
                this->_iSubModels.push_back(curLine[iiSubBlock].toInt());

            curIndex ++;
            iPoint++;
        }
    }
}

void OptimResult::updateRecomputedPointsFromFolder()
{
    _recomputedPoints.clear();

    // Filling final values from frontFile (csv)
    QDir dir(this->saveFolder());

    // Looking for recomputed points
    QString pointFolder;

    VariableResult *newVariableResult;
    bool curPointIsRecomputed;
    int nbPoints = _optObjectivesResults->nbPoints();

    for(int iPoint=0;iPoint<nbPoints;iPoint++)
    {
        pointFolder = dir.absolutePath()+ QDir::separator()+"point_"+QString::number(iPoint);
        curPointIsRecomputed=dir.exists(pointFolder);


        if(curPointIsRecomputed)
        {
            _recomputedPoints.push_back(iPoint);

            CSVBase::FileToVariableResult(_recomputedVariables,iPoint,pointFolder+QDir::separator()+"resultVar.csv");
            //int iRecVar;
            //            for(int iCurVar=0;iCurVar<pointVariables->size();iCurVar++)
            //            {
            //                //set value
            //                iRecVar = _recomputedVariables->findItem(pointVariables->at(iCurVar)->getFieldValue(Variable::NAME).toString());
            //                if(iRecVar>-1)
            //                    _recomputedVariables->at(iRecVar)->setFinalValueAtPoint(
            //                            pointVariables->at(iCurVar)->getFieldValue(Variable::VALUE).toDouble(),iPoint);
            //                else
            //                {
            //                    // variable found in file does not exist in _recomputedVariables
            //                    // add it
            //                    newVariableResult =  new VariableResult(*pointVariables->at(iCurVar));
            //                    newVariableResult->setFinalValueAtPoint(iPoint,
            //                                                            pointVariables->at(iCurVar)->getFieldValue(Variable::VALUE).toDouble());
            //                    _recomputedVariables->addItem(newVariableResult);

            //                    // display info
            //                    QString msg;
            //                    msg.sprintf("Variable %s added in recomputed variables list",
            //                                newVariableResult->getFieldValue(VariableResult::NAME).toString().utf16());
            //                    InfoSender::instance()->send(Info(msg,ListInfo::NORMAL2));
            //                }
            //            }
        }
    }
}



QString OptimResult::buildOptVarsFrontCSV(QString separator)
{

    if(_optVariablesResults->size()==0)
        return QString();

    int nbPoints = _optVariablesResults->at(0)->nbPoints();
    QList<int> listPoints;
    for(int i=0;i<nbPoints;i++)
        listPoints.push_back(i);

    // concatenate variables concerned
    MOOptVector variables(false,true,true);

    variables.append(*_optObjectivesResults,false);
    variables.append(*_optVariablesResults,false);

    return buildVarsFrontCSV(&variables,listPoints,separator);
}

QString OptimResult::buildAllVarsFrontCSV(QString separator)
{
    if(_recomputedVariables->size()==0)
        return QString();

    int nbPoints = _recomputedVariables->at(0)->nbPoints();
    QList<int> listPoints;
    for(int i=0;i<nbPoints;i++)
        listPoints.push_back(i);

    return buildVarsFrontCSV(_recomputedVariables,listPoints,separator);
}

QString OptimResult::buildAllVarsFrontCSV(QList<int> listPoints,QString separator)
{
    if(_recomputedVariables->size()==0)
        return QString();

    return buildVarsFrontCSV(_recomputedVariables,listPoints,separator);
}

QString OptimResult::buildVarsFrontCSV(MOOptVector* variables, QList<int> listPoints,QString separator)
{
    QString csv;
    int iVar,nbPoints,iPoint;

    // avoid identical columns
    QStringList varNames;


    if(variables->size()>0)
    {
        // writing names
        for(iVar=0;iVar<variables->size();iVar++)
        {
            csv += variables->at(iVar)->name();
            csv += separator;
        }

        // adding subModel index if necessary
        if(_subBlocks.size()>1)
        {
            csv += "subBlocksIndex";
            csv += separator;
        }
        csv += "\n";

        // writing values
        nbPoints = listPoints.size();

        for(iPoint = 0; iPoint < nbPoints; iPoint++)
        {
            for(iVar=0;iVar<variables->size();iVar++)
            {
                csv += QString::number(variables->at(iVar)->finalValue(0,listPoints.at(iPoint)));
                csv += separator;
            }
            // adding subModel index if necessary
            if(_subBlocks.size()>1)
            {
                csv += QString::number(_iSubModels.at(listPoints.at(iPoint)));
                csv += separator;
            }
            csv += "\n";
        }
    }
    return csv;
}

QDomElement OptimResult::toXmlData(QDomDocument & doc)
{

    // Result element
    QDomElement cRoot = doc.createElement("OMResult");
    QDomElement cResult = doc.createElement(OptimResult::className());
    cRoot.appendChild(cResult);


    // Result definition
    QDomElement cInfos = doc.createElement("Infos");
    cInfos.setAttribute("name", name());
    //cInfos.setAttribute("type", problemType());
    cInfos.setAttribute("date", _date.toString());
    cInfos.setAttribute("duration", _duration.toString());
    cResult.appendChild(cInfos);


    // SubModels blocks
    QDomElement cAllBlocks = doc.createElement("AllBlockSubstitutions");
    for(int i=0;i<_subBlocks.size();i++)
    {
        QDomElement cBlocks = _subBlocks.at(i)->toXmlData(doc);
        cBlocks.setTagName("BlockSubstitutions"+QString::number(i));
        cAllBlocks.appendChild(cBlocks);
    }
    cResult.appendChild(cAllBlocks);


    // Front File (containing values on pareto curve)
    QDomElement cOptFrontFile = doc.createElement("OptVarsFrontFile");
    cOptFrontFile.setAttribute("path", _optVarsFrontFileName);
    cResult.appendChild(cOptFrontFile);
    QDomElement cAllFrontFile = doc.createElement("AllVarsFrontFile");
    cAllFrontFile.setAttribute("path", _allVarsFrontFileName);
    cResult.appendChild(cAllFrontFile);


    return cResult;

}


void OptimResult::setCurPoint(int curPoint)
{
    _curPoint = curPoint;

    _recomputedVariables->setCurPoint(curPoint);
    _optVariablesResults->setCurPoint(curPoint);
    _optObjectivesResults->setCurPoint(curPoint);

    emit curPointChanged();
}

void OptimResult::setCurScan(int curScan)
{
    _curScan = curScan;

    _recomputedVariables->setCurScan(curScan );
    _optVariablesResults->setCurScan(curScan );
    _optObjectivesResults->setCurScan(curScan );

    emit curPointChanged();
}

int OptimResult::curPoint()
{
    return _curPoint;
}


void OptimResult::exportFrontCSV(QString fileName, bool allVars)
{
    QString csv;
    if(allVars)
        csv = buildAllVarsFrontCSV();
    else
        csv = buildOptVarsFrontCSV();

    QFile frontFile(fileName);
    if(frontFile.exists())
        frontFile.remove();

    frontFile.open(QIODevice::WriteOnly);
    QTextStream tsfront( &frontFile );
    tsfront << csv;
    frontFile.close();
}


