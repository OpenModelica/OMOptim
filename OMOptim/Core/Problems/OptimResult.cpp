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
#include "VariablesManip.h"
#include "LowTools.h"

OptimResult::OptimResult():Result()
{
    _recomputedVariables = new MOOptVector(true,true,true);
    _optObjectivesResults = new MOOptVector(true,false,true); //objectives are constant for one scan
    _optVariablesResults= new MOOptVector(true,false,true); //optimized parameters are constant for one scan

    // files to copy
    _filesToCopy.push_back(QFileInfo("iteration*.sav"));

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
    _filesToCopy.push_back(QFileInfo("iteration*.sav"));

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
    _filesToCopy.push_back(QFileInfo("iteration*.sav"));

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
    int savVersion = domResInfos.attribute("version",0).toInt();


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

    loadOptimValuesFromFrontFile(savVersion);


//    // Filling recomputed values from folder point_ (csv)
//    this->updateRecomputedPointsFromFolder();
}

void OptimResult::loadOptimValuesFromFrontFile(int version)
{
    switch(version)
    {
    case 0:
        return loadOptimValuesFromFrontFilev0();
    case 1:
        return loadOptimValuesFromFrontFilev1();
    default:
        return;
    }
}

void OptimResult::loadOptimValuesFromFrontFilev0()
{
    // Filling and Sizing recomputed variables (without values)
    ModelPlus* curModel;
    Optimization* problem = dynamic_cast<Optimization*>(_problem);
    for(int iM=0;iM<_models.size();iM++)
    {
        curModel = omProject()->modelPlus(_models.at(iM));

        if(curModel->variables()->isEmpty() && curModel->isCompiled(problem->ctrl(_models.at(iM))))
            curModel->readVariables(problem->ctrl(_models.at(iM)),problem->filesToCopy());

        for (int i=0;i<curModel->variables()->size();i++)
        {
            this->recomputedVariables()->addItem(new VariableResult(*curModel->variables()->at(i)));
        }
    }

    // Filling final values from frontFile (csv)
    QFileInfo frontFileInfo(saveFolder(),this->_optVarsFrontFileName);
    if(frontFileInfo.exists())
        updateOptimValuesFromFrontFile(frontFileInfo.absoluteFilePath());
}

void OptimResult::loadOptimValuesFromFrontFilev1()
{
    // this time, values are directly created from csv file
    // Filling final values from frontFile (csv)

    // read csv file
    QFileInfo frontFileInfo(saveFolder(),this->_allVarsFrontFileName);
    QFile frontFile(frontFileInfo.absoluteFilePath());
    if(!frontFile.exists())
        return;

    frontFile.open(QIODevice::ReadOnly);
    QTextStream tsfront( &frontFile );
    QString text = tsfront.readAll();
    frontFile.close();

    // set recomputedvariables values from it
    bool useScan = (dynamic_cast<Optimization*>(this->problem())->nbScans()>1);
    this->recomputedVariables()->setUseScan(useScan);
    this->recomputedVariables()->setFromCsv(text);


    // update objectives and variables from it
    // Clearing previous values
    QString modelName,varName;
    VariableResult* corrVar;
    for (int i=0; i<this->optObjectivesResults()->size(); i++)
    {
        modelName = this->optObjectivesResults()->at(i)->model();
        varName = this->optObjectivesResults()->at(i)->name(Variable::SHORT);
        corrVar = this->recomputedVariables()->findVariable(modelName,varName);
        if(corrVar)
            this->optObjectivesResults()->replaceAt(i,corrVar->clone());
    }

    for (int i=0; i<this->optVariablesResults()->size(); i++)
    {
        modelName = this->optVariablesResults()->at(i)->model();
        varName = this->optVariablesResults()->at(i)->name(Variable::SHORT);
        corrVar = this->recomputedVariables()->findVariable(modelName,varName);
        if(corrVar)
            this->optVariablesResults()->replaceAt(i,corrVar->clone());
    }
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

void OptimResult::updateOptimValuesFromFrontFile(QString fileName)
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

    QList<VariableResult *> objs;
    QList<VariableResult*> optVars;
    QList<VariableResult*> recompVars;

    bool useScan = (dynamic_cast<Optimization*>(this->problem())->nbScans()>1);
    this->recomputedVariables()->setUseScan(useScan);

    // Filling index tables
    for(int i=0; i<nbCols; i++)
    {
        objs.push_back(this->optObjectivesResults()->findItem(firstLine.at(i)));
        optVars.push_back(this->optVariablesResults()->findItem(firstLine.at(i)));
        if(!useScan)
            recompVars.push_back(this->recomputedVariables()->findItem(firstLine.at(i)));
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
                if (objs[iCol])
                {
                    objs[iCol]->setFinalValue(0,iPoint,curLine[iCol].toDouble());
                }
                if (optVars.at(iCol))
                {
                    optVars.at(iCol)->setFinalValue(0,iPoint,curLine[iCol].toDouble());
                }
                if ((!useScan) && recompVars.at(iCol))
                {
                    recompVars.at(iCol)->setFinalValue(0,iPoint,curLine[iCol].toDouble());
                }
            }

            if(iiSubBlock>-1)
                this->_iSubModels.push_back(curLine[iiSubBlock].toInt());

            curIndex ++;
            iPoint++;
        }
    }
 }

//void OptimResult::updateRecomputedPointsFromFolder()
//{
//    _recomputedPoints.clear();

//    // Filling final values from frontFile (csv)
//    QDir dir(this->saveFolder());

//    // Looking for recomputed points
//    QString pointFolder;

//    VariableResult *newVariableResult;
//    bool curPointIsRecomputed;
//    int nbPoints = _optObjectivesResults->nbPoints();

//    for(int iPoint=0;iPoint<nbPoints;iPoint++)
//    {
//        pointFolder = dir.absolutePath()+ QDir::separator()+"point_"+QString::number(iPoint);
//        curPointIsRecomputed=dir.exists(pointFolder);


//        if(curPointIsRecomputed)
//        {
//            _recomputedPoints.push_back(iPoint);
//            _recomputedVariables->updateFromCsv(_recomputedVariables,iPoint,pointFolder+QDir::separator()+"resultVar.csv");
//        }
//    }
//}



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
    return variables->toCSV(separator,listPoints);
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
    cInfos.setAttribute("version", 1);
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

void OptimResult::recomputePoints(QList<int> iPoints,bool forceRecompute)
{

    Optimization* problem = dynamic_cast<Optimization*>(_problem);

    int iPoint;
    int nbPoints = iPoints.size();

    //Info
    InfoSender::instance()->send( Info(ListInfo::RECOMPUTINGPOINTS,QString::number(nbPoints)));

    //Execution
    QString resultFolder = this->saveFolder();
    QString pointSaveFolder;

    for(int i=0;i<nbPoints;i++)
    {
        iPoint = iPoints.at(i);


        if (!this->recomputedPoints().contains(iPoint) || forceRecompute)
        {

            //****************************************************
            //Deleting point dir
            //****************************************************
            pointSaveFolder = resultFolder+QDir::separator()+"point_"+QString::number(iPoint);
            QDir dir = QDir(pointSaveFolder);
            // if dir already exists, deleting it
            if (dir.exists())
            {
                bool removed,tempBool;

                QStringList files = dir.entryList();
                QString provFile;
                for (int indf=0;indf<files.size();indf++)
                {
                    provFile = files[indf];
                    tempBool = dir.remove(provFile);
                }
                bool temp = dir.cdUp();
                dir.refresh();
                removed = dir.rmdir(pointSaveFolder);
                if (!removed)
                {
                    InfoSender::instance()->send (Info(ListInfo::FOLDERUNREMOVABLE,pointSaveFolder));
                }
            }

            for(int iM=0;iM<problem->models().size();iM++)
            {
                //*************************************************************
                //Creating a new OneSimulation problem based on same model
                //and specifying overwrited variables from optimized variable values
                //*************************************************************
                ModelPlus* modelPlus = omProject()->modelPlus(problem->models().at(iM));
                OneSimulation *oneSim = new OneSimulation(omProject(),modelPlus);
                oneSim->setCtrls(*problem->ctrls(problem->models().at(iM)));
                Variable* overVar;
                for(int iOverVar=0;iOverVar < this->optVariablesResults()->size();iOverVar++)
                {
                    overVar = new Variable(*optVariablesResults()->at(iOverVar));
//                    for(int iField=0;iField<Variable::nbFields;iField++)
//                    {
//                        overVar->setFieldValue(iField,this->optVariablesResults()->at(iOverVar)->getFieldValue(iField));
//                    }
                    overVar->setFieldValue(Variable::VALUE,this->optVariablesResults()->at(iOverVar)->finalValue(0,iPoint));
                    oneSim->overwritedVariables()->addItem(overVar);
                }

                // Add scannedVariables
                oneSim->scannedVariables()->cloneFromOtherVector(problem->scannedVariables());

                // Add overwrited variables
                for(int iV=0;iV<problem->overwritedVariables()->size();iV++)
                    oneSim->overwritedVariables()->addItem(problem->overwritedVariables()->at(iV)->clone());

                //****************************************************
                // Launch simulation
                //****************************************************
                ProblemConfig config;
                OneSimResult *oneSimRes = dynamic_cast<OneSimResult*>(oneSim->launch(config));

                if(!oneSimRes->isSuccess())
                {
                    InfoSender::instance()->send( Info(ListInfo::RECOMPUTINGPOINTFAILED,QString::number(iPoint)));
                }
                else
                {
                    InfoSender::instance()->send( Info(ListInfo::RECOMPUTINGPOINTSUCCESS,QString::number(iPoint)));

                    //****************************************************
                    // Filing recomputedVariables values
                    //****************************************************
                    VariableResult* newVariableResult;
                    VariableResult* curFinalVariable;
                    VariableResult* recVar;
                    for(int iVar=0;iVar<oneSimRes->finalVariables()->size();iVar++)
                    {
                        curFinalVariable = oneSimRes->finalVariables()->at(iVar);
                        // look in recomputed variables
                        recVar = this->recomputedVariables()->findItem(curFinalVariable->name());
                        if(recVar)
                        {
                            recVar->setFinalValuesAtPoint(iPoint,curFinalVariable->finalValuesAtPoint(0));
                        }
                        else
                        {
                            // copy from a Variable cast to avoid finalValues copying
                            newVariableResult =  new VariableResult(*dynamic_cast<Variable*>(curFinalVariable));
                            newVariableResult->setFinalValuesAtPoint(iPoint,
                                                                     curFinalVariable->finalValuesAtPoint(0));
                            this->recomputedVariables()->addItem(newVariableResult);

                        }
                        // update objective value if necessary (should'nt be the case, but if model has changed)
                        VariableResult* objVar= this->optObjectivesResults()->findItem(curFinalVariable->name());
                        if(objVar)
                        {
                            bool objOk=false;
                            OptObjective* optObj = problem->objectives()->findItem(objVar->name());
                            if(optObj)
                            {
                            double objValue = VariablesManip::calculateObjValue(optObj,oneSimRes->finalVariables(),objOk);
                            objVar->setFinalValue(0,iPoint,objValue,objOk);
                            }
                        }
                    }
//                    //*****************************
//                    //Saving results into csv file
//                    //*****************************
//                    //update scan folders
//                    LowTools::mkpath(pointSaveFolder,true);
//                    QFile file(pointSaveFolder+QDir::separator()+"resultVar.csv");
//                    file.open(QIODevice::WriteOnly);
//                    QTextStream ts( &file );
//                    ts << CSVBase::variableResultToValueLines(this->recomputedVariables(),iPoint);
//                    file.close();

                    delete oneSimRes;
                }
            }
        }
        else
        {
            QString msg = "Point " + QString::number(iPoint)+" already computed. Won't be recomputed";
            InfoSender::instance()->send(Info(msg,ListInfo::NORMAL2));
        }
    }

    emit finishedRecomputingPoints(iPoints);
}

int OptimResult::nbPoints()
{
    return _optVariablesResults->nbPoints();
}

