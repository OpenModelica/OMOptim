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
 	@version 0.9 

  */
#include "OptimResult.h"

OptimResult::OptimResult():Result()
{
	_recomputedVariables = new MOOptVector(true,true);
	_optObjectivesResults = new MOOptVector(false,true); //objectives are constant for one scan
	_optVariablesResults= new MOOptVector(false,true); //optimized parameters are constant for one scan

	// files to copy
	_filesToCopyNames << "iteration*.sav";

	_optVarsFrontFileName = "optVarsFront.csv";
	_allVarsFrontFileName = "allVarsFront.csv";

	_algo = NULL; 
	_curPoint = -1;
	_curScan = -1;
}
OptimResult::OptimResult(Project* project, ModModelPlus* modModelPlus, Optimization* problem, ModReader* modReader,
						 ModPlusCtrl* modPlusReader)
:Result(project,modModelPlus,problem,modReader,modPlusReader)
{
	_recomputedVariables = new MOOptVector(true,true);
	_optObjectivesResults = new MOOptVector(false,true); //objectives are constant for one scan
	_optVariablesResults= new MOOptVector(false,true); //optimized parameters are constant for one scan

	// files to copy
	_filesToCopyNames << "iteration*.sav";

	_optVarsFrontFileName = "optVarsFront.csv";
	_allVarsFrontFileName = "allVarsFront.csv";

	_curPoint = -1;
	_curScan = -1;

	_algo = problem->getCurAlgo()->clone();
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
}

OptimResult::~OptimResult(void)
{
	delete _optVariablesResults;
	delete _optObjectivesResults;
	delete _recomputedVariables;
}

void OptimResult::updateRecomputedPointsFromFolder()
{
    _recomputedPoints.clear();

    // Filling final values from frontFile (csv)
    QDir dir(_problem->saveFolder());

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

            CSV::FileToVariableResult(_recomputedVariables,iPoint,pointFolder+QDir::separator()+"resultVar.csv");
            //int iRecVar;
//            for(int iCurVar=0;iCurVar<pointVariables->items.size();iCurVar++)
//            {
//                //set value
//                iRecVar = _recomputedVariables->findItem(pointVariables->items.at(iCurVar)->getFieldValue(Variable::NAME).toString());
//                if(iRecVar>-1)
//                    _recomputedVariables->items.at(iRecVar)->setFinalValueAtPoint(
//                            pointVariables->items.at(iCurVar)->getFieldValue(Variable::VALUE).toDouble(),iPoint);
//                else
//                {
//                    // variable found in file does not exist in _recomputedVariables
//                    // add it
//                    newVariableResult =  new VariableResult(*pointVariables->items.at(iCurVar));
//                    newVariableResult->setFinalValueAtPoint(iPoint,
//                                                            pointVariables->items.at(iCurVar)->getFieldValue(Variable::VALUE).toDouble());
//                    _recomputedVariables->addItem(newVariableResult);

//                    // display info
//                    QString msg;
//                    msg.sprintf("Variable %s added in recomputed variables list",
//                                newVariableResult->getFieldValue(VariableResult::NAME).toString().utf16());
//                    infoSender.send(Info(msg,ListInfo::NORMAL2));
//                }
//            }
        }
    }
}

void OptimResult::recomputePoints(vector<int> iPoints,bool forceRecompute)
{

	int iPoint;
	int nbPoints = iPoints.size();

	//Info
	infoSender.send( Info(ListInfo::RECOMPUTINGPOINTS,QString::number(nbPoints)));

        //Execution
	QString resultFolder = _problem->saveFolder();
	QString pointSaveFolder;

	for(int i=0;i<nbPoints;i++)
	{
		iPoint = iPoints.at(i);


                if (!recomputedPoints().contains(iPoint) || forceRecompute)
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
					infoSender.send (Info(ListInfo::FOLDERUNREMOVABLE,pointSaveFolder));
				}
			}
			
			//*************************************************************
			//Creating a new OneSimulation problem based on same model
			//and specifying overwrited variables from optimized variable values
			//*************************************************************
			OneSimulation *oneSim = new OneSimulation(_project,_problem->rootModClass(),_problem->modReader(),_problem->modPlusCtrl(),_modModelPlus);
			Variable* overVar;
			for(int iOverVar=0;iOverVar < _optVariablesResults->items.size();iOverVar++)
			{
				overVar = new Variable();
				for(int iField=0;iField<Variable::nbFields;iField++)
				{
					overVar->setFieldValue(iField,_optVariablesResults->items.at(iOverVar)->getFieldValue(iField));
				}
				overVar->setFieldValue(Variable::VALUE,_optVariablesResults->items.at(iOverVar)->finalValue(0,iPoint));
				oneSim->overwritedVariables()->items.push_back(overVar);
			}

			// Add scannedVariables
			oneSim->scannedVariables()->cloneFromOtherVector(((Optimization*)_problem)->scannedVariables());
			
			//****************************************************
			// Launch simulation
			//****************************************************
			ProblemConfig config(_project->tempPath(),true);
			oneSim->launch(config);

			if(!oneSim->isSolved())
			{
				infoSender.send( Info(ListInfo::RECOMPUTINGPOINTFAILED,QString::number(iPoint)));
			}		
			else
			{
				infoSender.send( Info(ListInfo::RECOMPUTINGPOINTSUCCESS,QString::number(iPoint)));

				OneSimResult *oneSimRes = oneSim->result();

				//****************************************************
				// Filing recomputedVariables values
				//****************************************************
				VariableResult* newVariableResult;
				VariableResult* curFinalVariable;
				int iRecVar;
				for(int iVar=0;iVar<oneSimRes->finalVariables()->items.size();iVar++)
				{
					curFinalVariable = oneSimRes->finalVariables()->items.at(iVar);
					// look in recomputed variables
					iRecVar = _recomputedVariables->findItem(curFinalVariable->name());
					if(iRecVar>-1)
					{
						_recomputedVariables->items.at(iRecVar)->setFinalValuesAtPoint(iPoint,
							curFinalVariable->finalValuesAtPoint(0));
					}
					else
					{
						// copy from a Variable cast to avoid finalValues copying
						newVariableResult =  new VariableResult(*dynamic_cast<Variable*>(curFinalVariable));
						newVariableResult->setFinalValuesAtPoint(iPoint,
							curFinalVariable->finalValuesAtPoint(0));							
						_recomputedVariables->addItem(newVariableResult);

						QString msg;
						msg.sprintf("Variable %s added in recomputed variables list",
							newVariableResult->name().toLatin1().data());
						infoSender.debug(msg);
					}
					// update objective value if necessary (should'nt be the case, but if model has been changed)
					int iObj = _optObjectivesResults->findItem(curFinalVariable->name());
					if(iObj>-1)
					{
						bool objOk=false;			
						double objValue = VariablesManip::calculateObjValue(((Optimization*)_problem)->objectives()->items.at(iObj),oneSimRes->finalVariables(),objOk);
						_optObjectivesResults->items.at(iObj)->setFinalValue(0,iPoint,objValue,objOk);
					}
				}
				//*****************************
				//Saving results into csv file
				//*****************************
				//update scan folders
				dir.mkdir(pointSaveFolder);
				QFile file(pointSaveFolder+QDir::separator()+"resultVar.csv");
				file.open(QIODevice::WriteOnly);
				QTextStream ts( &file );
				ts << CSV::variableResultToValueLines(_recomputedVariables,iPoint);
				file.close();
			}
		}
		else
		{
			QString msg = "Point " + QString::number(iPoint)+" already computed. Won't be recomputed";
			infoSender.send(Info(msg,ListInfo::NORMAL2));				
		}
	}
}

QString OptimResult::buildOptVarsFrontCSV(QString separator)
{
	QString csv;
	int iObj,iVar,nbPoints,iPoint;

	// avoid identical columns
	QStringList objNames;
	QStringList varNames;
	vector<int> varKeeped; // contains var index that are not in commun with objectives
	vector<int> objKeeped;

	for(iObj=0;iObj<_optObjectivesResults->items.size();iObj++)
	{
		objNames << _optObjectivesResults->items.at(iObj)->name();
		objKeeped.push_back(iObj);
	}
	for(iVar=0;iVar<_optVariablesResults->items.size();iVar++)
	{
		varNames << _optVariablesResults->items.at(iVar)->name();
	}

	int iIdenticalObj;
	for(iVar=0;iVar<varNames.size();iVar++)
	{
		iIdenticalObj = objNames.indexOf(varNames.at(iVar));

		if(iIdenticalObj==-1)
		{
			varKeeped.push_back(iVar);
		}
	}

	
	// writing names
	for(iObj=0;iObj<_optObjectivesResults->items.size();iObj++)
	{
		csv += _optObjectivesResults->items.at(iObj)->name();
		csv += separator;
	}

	for(iVar=0;iVar<varKeeped.size();iVar++)
	{
		csv += _optVariablesResults->items.at(varKeeped.at(iVar))->name();
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
	nbPoints = _optObjectivesResults->nbPoints();

	for(iPoint = 0; iPoint < nbPoints; iPoint++)
	{
		for(iObj=0;iObj<_optObjectivesResults->items.size();iObj++)
		{
			csv += QString::number(_optObjectivesResults->items.at(iObj)->finalValue(0,iPoint));
			csv += separator;
		}
		for(iVar=0;iVar<varKeeped.size();iVar++)
		{
			csv += QString::number(_optVariablesResults->items.at(varKeeped.at(iVar))->finalValue(0,iPoint));
			csv += separator;
		}
		// adding subModel index if necessary
		if(_subBlocks.size()>1)
		{
			csv += QString::number(_iSubModels.at(iPoint));
			csv += separator;
		}
		csv += "\n";
	}

	return csv;
}

QString OptimResult::buildAllVarsFrontCSV(QString separator)
{
	int nbVars = _recomputedVariables->items.size();
	QList<int> listVars;
	for(int i=0;i<nbVars;i++)
		listVars.push_back(i);

	if(nbVars==0)
		return QString();
	
	int nbPoints = _recomputedVariables->items.at(0)->nbPoints();
	QList<int> listPoints;
	for(int i=0;i<nbPoints;i++)
		listPoints.push_back(i);

	return buildAllVarsFrontCSV(listVars,listPoints,separator);
}

QString OptimResult::buildAllVarsFrontCSV(QList<int> listPoints,QString separator)
{
	int nbVars = _recomputedVariables->items.size();
	QList<int> listVars;
	for(int i=0;i<nbVars;i++)
		listVars.push_back(i);

	return buildAllVarsFrontCSV(listVars,listPoints,separator);
}

QString OptimResult::buildAllVarsFrontCSV(QList<int> listVars, QList<int> listPoints,QString separator)
{
	QString csv;
	int iVar,nbPoints,iPoint;

	// avoid identical columns
	QStringList varNames;
	

	if(_recomputedVariables->items.size()>0)
	{
		// writing names
		for(iVar=0;iVar<listVars.size();iVar++)
		{
			csv += _recomputedVariables->items.at(iVar)->name();
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
			for(iVar=0;iVar<listVars.size();iVar++)
			{
				if(listVars.at(iVar)<_recomputedVariables->items.size())
				{
					csv += QString::number(_recomputedVariables->items.at(listVars.at(iVar))->finalValue(0,listPoints.at(iPoint)));
					csv += separator;
				}
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

	// Root element
	QDomElement cResult = doc.createElement("Result");
	
	// Result definition
	QDomElement cInfos = doc.createElement("Infos");
	cInfos.setAttribute("name", name());
	cInfos.setAttribute("type", problemType());
	cInfos.setAttribute("date", _date.toString());
	cInfos.setAttribute("hour", _hour.toString());
	cInfos.setAttribute("comptime", _computationTime.toString());
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

int OptimResult::nbScans()
{
	return static_cast<Optimization*>(_problem)->nbScans();
}
