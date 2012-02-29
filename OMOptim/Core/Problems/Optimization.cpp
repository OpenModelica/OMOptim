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

  @file Optimization.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
        @version

  */
#include "Optimization.h"
#include "EA"
#include "OptimAlgoUtils.h"
#include "CSVBase.h"
#include "LowTools.h"


Optimization::Optimization(Project* project,ModModelPlus* modModelPlus)
    :Problem((ProjectBase*)project)
{
    _omProject = project;
    _modModelPlus = modModelPlus;

    _name="Optimization";
    _optimizedVariables = new OptVariables(true,modModelPlus->modModelName());
    _scannedVariables = new ScannedVariables(true,modModelPlus->modModelName());
    _objectives = new OptObjectives(true,modModelPlus->modModelName());
    _blockSubstitutions = new BlockSubstitutions();

    _algos = OptimAlgoUtils::getNewAlgos(this);

    _iCurAlgo=0;

    // ctrls
    _ctrls = new ModPlusCtrls(project,modModelPlus);

}

Optimization::Optimization(const Optimization &optim)
    :Problem(optim)
{

    _modModelPlus = optim._modModelPlus;

    _optimizedVariables = optim._optimizedVariables->clone();
    _scannedVariables = optim._scannedVariables->clone();
    _objectives = optim._objectives->clone();
    _blockSubstitutions = optim._blockSubstitutions->clone();
    _ctrls = optim._ctrls->clone();


    //cloning algos
    for(int i=0;i<optim._algos.size();i++)
    {
        _algos.push_back(optim._algos.at(i)->clone());
        _algos.at(i)->setProblem(this);
    }

    _iCurAlgo = optim._iCurAlgo;

}

Problem* Optimization::clone() const
{
    return new Optimization(*this);
}

Optimization::~Optimization()
{
    for(int i=0;i<_algos.size();i++)
        delete _algos.at(i);

    delete _optimizedVariables;
    delete _objectives;
    delete _blockSubstitutions;
    delete _ctrls;

}


Optimization::Optimization(QDomElement domProblem,Project* project,bool &ok)
    :Problem((ProjectBase*)project)
{
    // initialization
    _omProject = project;
    _modModelPlus = NULL;
    _algos = OptimAlgoUtils::getNewAlgos(this);

    // look for modmodelplus
    QDomElement domInfos;
    ok = !domProblem.isNull();
    ok = ok && (domProblem.tagName()==Optimization::className());
    if(ok)
    {
        domInfos = domProblem.firstChildElement("Infos");
        QString modelName = domInfos.attribute("model");

        // Find modModelPlus
        ModModel* modModel = _omProject->findModModel(modelName);
        if(modModel == NULL)
        {
            InfoSender::instance()->sendWarning("Unable to find model "+modelName);
            ok = false;

        }
        _modModelPlus = project->modModelPlus(modelName);
    }

    if(!_modModelPlus)
    {
        ok = false;
        //initialize default(otherwise seg fault in destructor)
        _optimizedVariables = new OptVariables(true);
        _scannedVariables = new ScannedVariables(true);
        _objectives = new OptObjectives(true);
        _blockSubstitutions = new BlockSubstitutions();
        _ctrls = new ModPlusCtrls(project,NULL);
    }
    else
    {

        // finishing initialization
        _optimizedVariables = new OptVariables(true,_modModelPlus->modModelName());
        _scannedVariables = new ScannedVariables(true,_modModelPlus->modModelName());
        _objectives = new OptObjectives(true,_modModelPlus->modModelName());
        _blockSubstitutions = new BlockSubstitutions();

        _iCurAlgo=0;


        // Infos
        this->setName(domInfos.attribute("name", "" ));

        // Optimized Variables
        QDomElement domOptVars = domProblem.firstChildElement("OptimizedVariables");
        this->optimizedVariables()->setItems(domOptVars);

        // Objectives
        QDomElement domObj = domProblem.firstChildElement("Objectives");
        this->objectives()->setItems(domObj);

        // Scanned Variables
        QDomElement domScann = domProblem.firstChildElement("ScannedVariables");
        this->scannedVariables()->setItems(domScann);

        // Files to copy
        QDomElement cFilesToCopy = domProblem.firstChildElement("FilesToCopy");
        QString text = cFilesToCopy.text();
        this->_filesToCopy = text.split("\n",QString::SkipEmptyParts);

        // Controlers
        QDomElement cControlers = domProblem.firstChildElement("Controlers");
        _ctrls = new ModPlusCtrls(project,_modModelPlus,cControlers);

        // BlockSubstitutions
        QDomElement domBlockSubs = domProblem.firstChildElement("BlockSubstitutions");
        this->setBlockSubstitutions(new BlockSubstitutions(project,_modModelPlus,domBlockSubs));

        // EA
        QDomElement domEA = domProblem.firstChildElement("EA");
        QDomElement domEAInfos = domEA.firstChildElement("Infos");
        if(!domEAInfos.isNull())
        {
            this->setiCurAlgo(domEAInfos.attribute("num", "" ).toInt());
        }
        QDomElement domEAParameters = domEA.firstChildElement("Parameters");
        if(!domEAParameters.isNull())
        {
            this->getCurAlgo()->_parameters->update(domEAParameters);
        }
    }
}


/** Launch optimization procedure. checkBeforeComp() is not called in this function.
*   Be sure it has been called previously.
*/
bool Optimization::checkBeforeComp(QString & error)
{
    bool ok = true;
    int nbErrors = 0;
    QString curError;

    // check scanned variables
    for(int i=0;i<_scannedVariables->size();i++)
    {
        if(!_scannedVariables->at(i)->check(curError))
        {
            ok = false;
            curError.sprintf("Error concerning scan variable %s : %s",
                             _scannedVariables->at(i)->name().toLatin1().data(),
                             curError.toLatin1().data());
            error += curError + "\n";
            nbErrors++;
        }
    }

    // check otpimized variables
    for(int i=0;i<_optimizedVariables->size();i++)
    {
        if(!_optimizedVariables->at(i)->check(curError))
        {
            ok = false;
            curError.sprintf("Error concerning optimized variable %s : %s",
                             _optimizedVariables->at(i)->name().toLatin1().data(),
                             curError.toLatin1().data());
            error += curError + "\n";
            nbErrors++;
        }
    }

    // check objectives
    for(int i=0;i<_objectives->size();i++)
    {
        if(!_objectives->at(i)->check(curError))
        {
            ok = false;
            curError.sprintf("Error concerning objective %s : %s",
                             _objectives->at(i)->name().toLatin1().data(),
                             curError.toLatin1().data());
            error += curError + "\n";
            nbErrors++;
        }
    }

    // check number of optimized variables and objectives
    if((_optimizedVariables->size()==0) && (_blockSubstitutions->size()==0))
    {
        ok = false;
        curError = "Should specify at least one optimized variable";
        error += curError + "\n";
        nbErrors++;
    }
    if(_objectives->size()==0)
    {
        ok = false;
        curError = "Should specify at least one objective";
        error += curError + "\n";
        nbErrors++;
    }

    // check if optimization algorithm compatible with objectives number
    if((_objectives->size()>1) && ! getCurAlgo()->acceptMultiObjectives())
    {
        ok = false;
        curError = "Current alogrithm does not support multi-objectives";
        error += curError + "\n";
        nbErrors++;
    }


    // check if objectives have scan function
    if(_scannedVariables->size()>0)
    {
        bool okObj;
        for(int i=0;i<_objectives->size();i++)
        {
            if(_objectives->at(i)->scanFunction()==OptObjective::NONE)
            {
                ok = false;
                curError.sprintf("Objective %s should have a scanFunction defined",
                                 _objectives->at(i)->name().toLatin1().data());
                error += curError + "\n";
                nbErrors++;
            }
        }
    }

    if(nbErrors>0)
    {
        if(nbErrors==1)
            error.insert(0,"1 error : \n");
        else
            error.insert(0,QString::number(nbErrors)+" errors : \n");
    }
    return ok;
}



/** Description : Launch optimization procedure. checkBeforeComp() is not called in this function.
*   Be sure it has been called previously.
*/
Result* Optimization::launch(ProblemConfig _config)
{

    // first create temp dir
    QDir dir;
    if(dir.exists(_project->tempPath()))
        dir.rmdir(_project->tempPath());
    dir.mkdir(_project->tempPath());

#ifdef USEBLOCKSUB
    //create different dymosim executable for blocksubstitutions
    QList<ModModelPlus*> _subModels;
    QList<BlockSubstitutions*> _subBlocks;
    createSubExecs(_subModels,_subBlocks);
#endif

    OptimResult* result = dynamic_cast<OptimResult*>(((EABase*)getCurAlgo())->launch(_project->tempPath()));

    result->setName(this->name()+" result");

    return result;
}


void Optimization::recomputePoints(OptimResult* result, vector<int> iPoints,bool forceRecompute)
{

    int iPoint;
    int nbPoints = iPoints.size();

    //Info
    InfoSender::instance()->send( Info(ListInfo::RECOMPUTINGPOINTS,QString::number(nbPoints)));

    //Execution
    QString resultFolder = result->saveFolder();
    QString pointSaveFolder;

    for(int i=0;i<nbPoints;i++)
    {
        iPoint = iPoints.at(i);


        if (!result->recomputedPoints().contains(iPoint) || forceRecompute)
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

            //*************************************************************
            //Creating a new OneSimulation problem based on same model
            //and specifying overwrited variables from optimized variable values
            //*************************************************************
            OneSimulation *oneSim = new OneSimulation(_omProject,result->modModelPlus());
            Variable* overVar;
            for(int iOverVar=0;iOverVar < result->optVariablesResults()->size();iOverVar++)
            {
                overVar = new Variable();
                for(int iField=0;iField<Variable::nbFields;iField++)
                {
                    overVar->setFieldValue(iField,result->optVariablesResults()->at(iOverVar)->getFieldValue(iField));
                }
                overVar->setFieldValue(Variable::VALUE,result->optVariablesResults()->at(iOverVar)->finalValue(0,iPoint));
                oneSim->overwritedVariables()->items.push_back(overVar);
            }

            // Add scannedVariables
            oneSim->scannedVariables()->cloneFromOtherVector(this->scannedVariables());

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
                int iRecVar;
                for(int iVar=0;iVar<oneSimRes->finalVariables()->size();iVar++)
                {
                    curFinalVariable = oneSimRes->finalVariables()->at(iVar);
                    // look in recomputed variables
                    iRecVar = result->recomputedVariables()->findItem(curFinalVariable->name());
                    if(iRecVar>-1)
                    {
                        result->recomputedVariables()->at(iRecVar)->setFinalValuesAtPoint(iPoint,
                                                                                          curFinalVariable->finalValuesAtPoint(0));
                    }
                    else
                    {
                        // copy from a Variable cast to avoid finalValues copying
                        newVariableResult =  new VariableResult(*dynamic_cast<Variable*>(curFinalVariable));
                        newVariableResult->setFinalValuesAtPoint(iPoint,
                                                                 curFinalVariable->finalValuesAtPoint(0));
                        result->recomputedVariables()->addItem(newVariableResult);

                        QString msg;
                        msg.sprintf("Variable %s added in recomputed variables list",
                                    newVariableResult->name().toLatin1().data());
                        InfoSender::instance()->debug(msg);
                    }
                    // update objective value if necessary (should'nt be the case, but if model has been changed)
                    int iObj = result->optObjectivesResults()->findItem(curFinalVariable->name());
                    if(iObj>-1)
                    {
                        bool objOk=false;
                        double objValue = VariablesManip::calculateObjValue(this->objectives()->at(iObj),oneSimRes->finalVariables(),objOk);
                        result->optObjectivesResults()->at(iObj)->setFinalValue(0,iPoint,objValue,objOk);
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
                ts << CSVBase::variableResultToValueLines(result->recomputedVariables(),iPoint);
                file.close();

                delete oneSimRes;
            }
        }
        else
        {
            QString msg = "Point " + QString::number(iPoint)+" already computed. Won't be recomputed";
            InfoSender::instance()->send(Info(msg,ListInfo::NORMAL2));
        }
    }
}


void Optimization::createSubExecs(QList<ModModelPlus*> & subModels, QList<BlockSubstitutions*> & subBlocks)
{

    subModels.clear();
    subBlocks.clear();

    QMultiMap<QString,QString> map;
    // fill map
    for(int i=0; i < _blockSubstitutions->getSize();i++)
    {
        BlockSubstitution *curBlockSub = _blockSubstitutions->getAt(i);
        if(!curBlockSub->_subComponent.isEmpty())
            map.insert(curBlockSub->_orgComponent,curBlockSub->_subComponent);
    }

    int nbOrgs = map.uniqueKeys().size();
    //adding non-moving cases for each orgComponent
    for(int i = 0; i<nbOrgs; i ++)
    {
        map.insert(map.uniqueKeys().at(i),map.uniqueKeys().at(i));
    }

    //build first index and maximum index
    QList<int> index, maxIndex;
    nbOrgs = map.uniqueKeys().size();
    for(int i = 0; i<nbOrgs; i ++)
    {
        index.push_back(0);
        QList<QString> subs = map.values(map.uniqueKeys().at(i));
        maxIndex.push_back(subs.size()-1);
    }

    // genuine mo file information
    QString oldMoFilePath = _modModelPlus->modModel()->filePath();
    QFile oldMoFile(oldMoFilePath);
    QFileInfo oldMoFileInfo(oldMoFile);



    int iCase=0;
    bool oneChange;
    while(!index.isEmpty())
    {
        // Display case (for debug)
        QString msg = "CASE " + QString::number(iCase) + "\n";
        for(int i=0; i < index.size(); i++)
        {
            msg += map.uniqueKeys().at(i);
            msg += " -> ";
            msg += map.values(map.uniqueKeys().at(i)).at(index.at(i));
            msg+=",";
        }
        msg.remove(msg.size()-1,1);
        msg +="\n \n";
        InfoSender::instance()->debug(msg);


        // create folder
        QString newName = _modModelPlus->modModelName() + "_case_"+QString::number(iCase);
        QString newFolder = saveFolder()+ QDir::separator() + "SubModels" + QDir::separator() + newName;
        QDir dir(saveFolder());
        dir.mkpath(newFolder);
        QDir newDir(newFolder);
        QString newMmoFilePath = newDir.absoluteFilePath(newName+".mmo");

        // clone mo file.
        QString newMoPath = newDir.filePath(oldMoFileInfo.fileName());
        bool removeExistingMo = newDir.remove(newMoPath);
        oldMoFile.copy(newMoPath);

        // load file (! will replace previously loaded)
        _omProject->loadMoFile(newMoPath,false,true);

        // create new modModel
        ModModel* newModModel = dynamic_cast<ModModel*>(_omProject->modItemsTree()->findItem(_modModelPlus->modModelName()));

        if(newModModel)
        {
            // create new modModelPlus
            ModModelPlus* newModModelPlus = new ModModelPlus(_omProject,newModModel->name());
            newModModelPlus->setMmoFilePath(newMmoFilePath);

            // apply blocksubs
            BlockSubstitutions *curSubBlocks = new BlockSubstitutions();

            oneChange = false;
            for(int i=0; i<index.size();i++)
            {
                QString replacedComp = map.uniqueKeys().at(i);
                QString replacingComp = map.values(map.uniqueKeys().at(i)).at(index.at(i));

                if(replacedComp != replacingComp)
                {
                    BlockSubstitution* blockSub = _blockSubstitutions->find(replacedComp,replacingComp);
                    if(blockSub)
                    {
                        oneChange =  newModModelPlus->applyBlockSub(blockSub,true) || oneChange ;
                        curSubBlocks->push_back(blockSub);
                    }
                }

            }

            if(oneChange)
            {
                bool compiledOk = newModModelPlus->compile(ctrl());

                if(compiledOk)
                {
                    // store subModel and subBlocks
                    subModels.push_back(newModModelPlus);
                    subBlocks.push_back(curSubBlocks);
                    _foldersToCopy << newFolder;

                    InfoSender::instance()->send( Info(ListInfo::SUBMODELADDED,newName));
                }
                else
                {
                    InfoSender::instance()->send( Info(ListInfo::SUBMODELNOTADDED,newName));
                }
            }
        }
        iCase++;
        index = LowTools::nextIndex(index,maxIndex);
    }

    // reload genuine mo file
    if(iCase>0)
        _omProject->loadMoFile(oldMoFilePath,false,true);
}

ModModelPlus* Optimization::modModelPlus() const
{
    return _modModelPlus;
}

void Optimization::setBlockSubstitutions(BlockSubstitutions* blockSubstitutions)
{
    if(_blockSubstitutions)
        delete _blockSubstitutions;

    _blockSubstitutions = blockSubstitutions;
}

/**
* Description Save problem in a folder in XML form.
*/
void Optimization::store(QString destFolder, QString tempDir)
{
    Problem::store(destFolder,tempDir);
}

QDomElement Optimization::toXmlData(QDomDocument & doc)
{

    QDomElement cProblem = doc.createElement(getClassName());
    //***********************
    // Problem definition
    //***********************
    QDomElement cInfos = doc.createElement("Infos");
    cProblem.appendChild(cInfos);
    cInfos.setAttribute("name", name());
    cInfos.setAttribute("model", _modModelPlus->modModelName());

    // Optimized variables
    QDomElement cOptVariables = _optimizedVariables->toXmlData(doc,"OptimizedVariables");
    cProblem.appendChild(cOptVariables);

    // objectives
    QDomElement cObjectives = _objectives->toXmlData(doc,"Objectives");
    cProblem.appendChild(cObjectives);

    // ScannedVariables
    QDomElement cScanned = _scannedVariables->toXmlData(doc,"ScannedVariables");
    cProblem.appendChild(cScanned);

    //BlockSubstitutions
    QDomElement cBlocks = _blockSubstitutions->toXmlData(doc);
    cProblem.appendChild(cBlocks);

    // Controlers
    QDomElement cControlers = ctrls()->toXmlData(doc);
    cProblem.appendChild(cControlers);

    // Files to copy
    QDomElement cFilesToCopy = doc.createElement("FilesToCopy");
    QDomText cFiles = doc.createTextNode(_filesToCopy.join("\n"));
    cFilesToCopy.appendChild(cFiles);
    cProblem.appendChild(cFilesToCopy);

    // ea configuration
    OptimAlgo *curAlgo = getCurAlgo();

    if(curAlgo)
    {
        QDomElement cEA = doc.createElement("EA");
        cProblem.appendChild(cEA);

        QDomElement cEAInfos = doc.createElement("Infos");
        cEAInfos.setAttribute("num",getiCurAlgo());
        cEA.appendChild(cEAInfos);

        QDomElement cAlgoParameters = curAlgo->_parameters->toXmlData(doc,"Parameters");
        cEA.appendChild(cAlgoParameters);
    }

    return cProblem;
}

int Optimization::nbScans()
{
    if(_scannedVariables->size()==0)
        return 1;

    int nbScans = 1;
    for(int i=0;i<_scannedVariables->size();i++)
        nbScans = nbScans*_scannedVariables->at(i)->nbScans();

    return nbScans;
}


int Optimization::getiCurAlgo()
{
    return _iCurAlgo;
}

OptimAlgo* Optimization::getCurAlgo() const
{
    if((_iCurAlgo>-1)&&(_iCurAlgo<_algos.size()))
        return _algos.at(_iCurAlgo);
    else
        return NULL;
}


QStringList Optimization::getAlgoNames()
{
    QStringList names;
    for(int i=0;i<_algos.size();i++)
        names.push_back(_algos.at(i)->name());
    return names;
}

void Optimization::setiCurAlgo(int iCurAlgo)
{
    _iCurAlgo = iCurAlgo;
}

/**
  * This function allows to finish optimization the soonest but getting intermediary results.
  * e.g. in population algorithms, it stops at the end of the current generation. This allows
  * to keep results (thus differs from ProblemThread::onStopAsked() )
  */
void Optimization::onQuickEndAsked()
{
    getCurAlgo()->onQuickEndAsked();
}




ModPlusCtrl* Optimization::ctrl() const
{
    return _ctrls->currentCtrl();
}

ModPlusCtrls* Optimization::ctrls() const
{
    return _ctrls;
}

ModPlusCtrl::Type  Optimization::ctrlType() const
{
    return _ctrls->currentCtrlType();
}

void Optimization::setCtrlType(ModPlusCtrl::Type type)
{
    _ctrls->setCurrentCtrlType(type);
}
