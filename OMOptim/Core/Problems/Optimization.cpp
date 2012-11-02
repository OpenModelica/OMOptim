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

  @file Optimization.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
        @version

  */
#include "Optimization.h"
#include "EA"
#include "CSVBase.h"
#include "LowTools.h"
#include "ModPlusCtrls.h"
#include "InfoSender.h"



Optimization::Optimization(Project* project,QStringList models)
    :Problem((ProjectBase*)project)

{
    _name="Optimization";

    _optimizedVariables = new OptVariables(true);
    _overwritedVariables = new Variables(true);
    _scannedVariables = new ScannedVariables(true);
    _objectives = new OptObjectives(true);
    _blockSubstitutions = new BlockSubstitutions();

    _algos = new OptimAlgos(project,this);
    _algos->setCurrentAlgo(SPEA2Adapt().name());

    // models and ctrls
    for(int i=0;i<models.size();i++)
    {
        this->addModel(models.at(i));
    }
}

Optimization::Optimization(const Optimization &optim)
    :Problem(optim)
{
    _optimizedVariables = optim._optimizedVariables->clone();
    _scannedVariables = optim._scannedVariables->clone();
    _overwritedVariables = optim._overwritedVariables->clone();
    _objectives = optim._objectives->clone();
    _blockSubstitutions = optim._blockSubstitutions->clone();

    QString curModel;
    for(int i=0;i<optim._models.size();i++)
    {
        curModel = optim._models.at(i);
        this->addModel(curModel,optim._ctrls.value(curModel)->clone());
    }

    _algos = optim._algos->clone();
    _algos->setProblem(this);
}

Problem* Optimization::clone() const
{
    return new Optimization(*this);
}

Optimization::~Optimization()
{
    delete _algos;

    delete _optimizedVariables;
    delete _objectives;
    delete _blockSubstitutions;
    delete _overwritedVariables;
    delete _scannedVariables;

    for(int i=0;i<_ctrls.values().size();i++)
    {
        delete _ctrls.values().at(i);
    }
}

Project *Optimization::omProject()
{
    return dynamic_cast<Project*>(_project);
}

/**
  * This function is used to evaluate a single configuration. It is called within optimization process, in fitness evaluation function.
  * @sa EAStdOptimizationEval
  * Returns a vector of final variables.
  * @param ok : is set to true if evaluation is successful, to false otherwise.
  */
MOOptVector *Optimization::evaluate(QList<ModelPlus*> models, Variables *optimVariables, ScannedVariables *scannedVariables, bool &ok)
{
    ok = true;
    bool useScan = (scannedVariables->size()>0);
    bool usePoints = false;
    MOOptVector* resultVariables = new MOOptVector(true,useScan,usePoints);

    for(int iM=0;iM<models.size();iM++)
    {
        if(ok)
        {
            /************************************
            Creating a new OneSimulation
            ************************************/
            QString modelName = models.at(iM)->modelName();
            OneSimulation *oneSim = new OneSimulation(omProject(),models.at(iM));
            oneSim->_filesToCopy = this->_filesToCopy;
            oneSim->setCtrls(*this->ctrls(modelName));


            int nbVar = optimVariables->size();
            Variable* curVar;

            // set optim variables in one simulation
            for(int i=0;i<nbVar;i++)
            {
                curVar = optimVariables->at(i);
                if(curVar->model()==modelName)
                {
                    oneSim->overwritedVariables()->addItem(curVar->clone());
                }
            }

            // add overwrited variables of optimization in simulation
            for(int i=0;i<overwritedVariables()->size();i++)
            {
                curVar = overwritedVariables()->at(i);
                if(curVar->model()==modelName)
                {
                    oneSim->overwritedVariables()->addItem(curVar->clone());
                }
            }

            // copying relevant scanned variables
            for(int iS=0;iS<scannedVariables->size();iS++)
            {
                if(scannedVariables->at(iS)->model()==modelName)
                {
                    oneSim->scannedVariables()->addItem(scannedVariables->at(iS)->clone());
                }
            }


            /************************************
            Launch OneSimulation
            ************************************/
            ProblemConfig config();
            OneSimResult *result =  dynamic_cast<OneSimResult*>(oneSim->launch(config));
            ok = ok && result->isSuccess();
            resultVariables->addItems(result->finalVariables(),true);

            // free memory
            delete oneSim;
            delete result;
        }
    }

    return resultVariables;
}


Optimization::Optimization(QDomElement domProblem,Project* project,bool &ok)
    :Problem((ProjectBase*)project)
{
    // initialization
    // look for modmodelplus
    ok = !domProblem.isNull();
    ok = ok && (domProblem.tagName()==Optimization::className());
    if(ok)
    {

        QDomElement  domInfos = domProblem.firstChildElement("Infos");
        this->setName(domInfos.attribute("name", "" ));

        // compatibility with older saving format (one model, saved in Infos node)
        if(!domInfos.attribute("model").isEmpty())
        {
            QString modelName = domInfos.attribute("model");
            ModelPlus* modelPlus = project->modelPlus(modelName);

            if(modelPlus)
            {
                // read corresponding controlers
                QDomElement domCtrls = domProblem.firstChildElement(ModPlusCtrls::className());
                ModPlusCtrls* ctrls = new ModPlusCtrls(project,modelPlus,domCtrls);

                // add model
                this->addModel(domInfos.attribute("model"));
            }
        }

        // new format
        QDomElement domModels = domProblem.firstChildElement("Models");
        QDomElement domModel = domModels.firstChildElement("Model");
        while(!domModel.isNull())
        {
            QString modelName = domModel.attribute("name");
            ModelPlus* modelPlus = project->modelPlus(modelName);

            if(modelPlus) // add it only if found in project
                          // otherwise, may lead to segfaults
            {
                // read corresponding controlers
                QDomElement domCtrls = domModel.firstChildElement(ModPlusCtrls::className());
                ModPlusCtrls* ctrls = new ModPlusCtrls(project,modelPlus,domCtrls);

                // add model and controlers
                this->addModel(modelName,ctrls);
            }


            domModel = domModel.nextSiblingElement("Model");
        }

    }

    //initialize default(otherwise seg fault in destructor)
    _optimizedVariables = new OptVariables(true);
    _scannedVariables = new ScannedVariables(true);
    _overwritedVariables = new Variables(true);
    _objectives = new OptObjectives(true);

    // Parameters
    QDomElement domParameters = domProblem.firstChildElement("Parameters");
    this->parameters()->setItems(domParameters);

    // Optimized Variables
    QDomElement domOptVars = domProblem.firstChildElement("OptimizedVariables");
    this->optimizedVariables()->setItems(domOptVars);

    // Objectives
    QDomElement domObj = domProblem.firstChildElement("Objectives");
    this->objectives()->setItems(domObj);

    // Scanned Variables
    QDomElement domScann = domProblem.firstChildElement("ScannedVariables");
    this->scannedVariables()->setItems(domScann);

    // Overvars Variables
    QDomElement domOverVars = domProblem.firstChildElement("OverwritedVariables");
    this->overwritedVariables()->setItems(domOverVars);
    for(int i=0;i<overwritedVariables()->size();i++)
        overwritedVariables()->at(i)->setIsEditableField(Variable::VALUE,true);

    // Files to copy
    QDomElement cFilesToCopy = domProblem.firstChildElement("FilesToCopy");
    QString text = cFilesToCopy.text();
    QStringList strList = text.split("\n",QString::SkipEmptyParts);
    for(int i=0;i<strList.size();i++)
        this->_filesToCopy.push_back(QFileInfo(strList.at(i)));

    // BlockSubstitutions
    QDomElement domBlockSubs = domProblem.firstChildElement("BlockSubstitutions");
    _blockSubstitutions = new BlockSubstitutions(project,domBlockSubs);

    // Algos
    QDomElement cAlgos = domProblem.firstChildElement(OptimAlgos::className());
    if(!cAlgos.isNull())
    {
        _algos = new OptimAlgos(project,this,cAlgos);
    }
    else
    {
        _algos = new OptimAlgos(project,this);
        // older version
        QDomElement domEA = domProblem.firstChildElement("EA");
        QDomElement domEAInfos = domEA.firstChildElement("Infos");
        QString algoName;
        if(!domEAInfos.isNull())
        {
            int iAlgo = (OptimAlgosList::Type)domEAInfos.attribute("num", "" ).toInt();
            OptimAlgo* algo = OptimAlgosList::getNewAlgo(project,this,(OptimAlgosList::Type)iAlgo);
            if(algo)
            {
                algoName = algo->name();
                setCurAlgo(algoName);
                delete algo;
            }
        }
        QDomElement domEAParameters = domEA.firstChildElement("Parameters");
        if(!domEAParameters.isNull())
        {
            this->getCurAlgo()->parameters()->update(domEAParameters);
        }
    }
}


/** Launch Optimization procedure. checkBeforeComp() is not called in this function.
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

    // check if Optimization algorithm compatible with objectives number
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



/** Description : Launch Optimization procedure. checkBeforeComp() is not called in this function.
*   Be sure it has been called previously.
*/
Result* Optimization::launch(ProblemConfig config)
{

    // first create temp dir
    LowTools::mkpath(_project->tempPath(),false);


    EABase* algo = ((EABase*)getCurAlgo());

#ifdef USEBLOCKSUB
    //create different dymosim executable for blocksubstitutions

    QList<ModModelPlus*> subModels;
    QList<BlockSubstitutions*> subBlocks;
    createSubExecs(subModels,subBlocks);
    algo->setSubModels(subModels,subBlocks);

#endif

    OptimResult* result = dynamic_cast<OptimResult*>(algo->launch(_project->tempPath()));

    result->setName(this->name()+" result");

    return result;
}





void Optimization::createSubExecs(QList<QList<ModelPlus*> > & subModels, QList<BlockSubstitutions*> & subBlocks)
{

    subModels.clear();
    subBlocks.clear();

    QMultiMap<QString,QString> map; // <orgComponent,subcomponent>
    QMap<QString,QString> mapModel; //<orgComponent,model>
    // fill map
    for(int i=0; i < _blockSubstitutions->getSize();i++)
    {
        BlockSubstitution *curBlockSub = _blockSubstitutions->getAt(i);
        if(!curBlockSub->_subComponent.isEmpty())
        {
            map.insert(curBlockSub->_orgComponent,curBlockSub->_subComponent);
            mapModel.insert(curBlockSub->_orgComponent,curBlockSub->_model);
        }
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



    QStringList models = mapModel.values();
    models.removeDuplicates();

    // storing genuine mo file paths
    QStringList oldMoFilePaths;
    for(int iM=0;iM<models.size();iM++)
    {
        ModModelPlus* modModelPlus = dynamic_cast<ModModelPlus*>(omProject()->modelPlus(models.at(iM)));
        oldMoFilePaths.push_back(modModelPlus->moFilePath());
    }

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
        QString newName = "case_"+QString::number(iCase);

        QString newFolder = saveFolder()+ QDir::separator() + "SubModels" + QDir::separator() + newName;
        QDir dir(saveFolder());
        dir.mkpath(newFolder);
        QDir newDir(newFolder);


        // clone mo files and load them
        // and create corresponding modmodelplus
        QStringList newMoPaths;
        QStringList newMmoPaths;
        QMap<QString,ModelPlus*> newModModels;
        for(int iM=0;iM<oldMoFilePaths.size();iM++)
        {
            QFileInfo oldMoFileInfo(oldMoFilePaths.at(iM));
            QFile oldMoFile(oldMoFilePaths.at(iM));

            QString newMoPath = newDir.filePath(oldMoFileInfo.fileName());
            QString newMmoPath = newMoPath;
            newMmoPath = newMmoPath.replace(".mo",".mmo");

            newDir.remove(newMoPath);
            oldMoFile.copy(newMoPath);

            newMoPaths.append(newMoPath);
            newMmoPaths.append(newMmoPath);


            // load file (! will replace previously loaded)
            omProject()->loadMoFile(newMoPath,false,true);

            // create new modModelPlus
            ModModelPlus* newModModelPlus = new ModModelPlus(omProject(),models.at(iM));
            newModModelPlus->setMmoFilePath(newMmoPath);
            newModModels.insert(models.at(iM),newModModelPlus);
        }


        // apply blocksubs
        BlockSubstitutions *curSubBlocks = new BlockSubstitutions();

        QMap<QString,bool> changes; // <model,hasChanged>
        changes.clear();
        for(int i=0; i<index.size();i++)
        {
            QString replacedComp = map.uniqueKeys().at(i);
            QString replacingComp = map.values(map.uniqueKeys().at(i)).at(index.at(i));

            if(replacedComp != replacingComp)
            {
                BlockSubstitution* blockSub = _blockSubstitutions->find(replacedComp,replacingComp);
                if(blockSub)
                {
                    ModelPlus* corrNewModModelPlus = newModModels.value(blockSub->_model);
                    oneChange =  ((ModModelPlus*) corrNewModModelPlus)->applyBlockSub(blockSub,true) || oneChange ;
                    curSubBlocks->push_back(blockSub);
                    changes.insert(blockSub->_model,true);
                }
            }
        }

        QStringList modelsToCompile = changes.keys(true);// those which have been modified
        bool compilationOk = true;
        for(int iM=0;iM<modelsToCompile.size();iM++)
        {
            ModelPlus* modelPlus =  newModModels.value(modelsToCompile.at(iM));
            compilationOk = ((ModModelPlus*) modelPlus)->compile(ctrl(modelsToCompile.at(iM))) && compilationOk;
        }

        if(compilationOk)
        {

            // store subModel and subBlocks
            subModels.push_back(newModModels.values());
            subBlocks.push_back(curSubBlocks);
            _foldersToCopy << newFolder;

            InfoSender::instance()->send( Info(ListInfo::SUBMODELADDED,newName));
        }
        else
        {
            InfoSender::instance()->send( Info(ListInfo::SUBMODELNOTADDED,newName));
        }


        iCase++;
        index = LowTools::nextIndex(index,maxIndex);
    }

    // reload genuine mo file
    if(iCase>0)
    {
        for(int i=0;i<oldMoFilePaths.size();i++)
            omProject()->loadMoFile(oldMoFilePaths.at(i),false,true);
    }
}

OptimAlgos *Optimization::algos() const
{
    return _algos;
}

QStringList Optimization::models() const

{
    return _models;
}

bool Optimization::addModel(QString modelName,ModPlusCtrls* ctrls)
{
    if(_models.contains(modelName)&&(ctrls==NULL))
        return false;
    else if (omProject()->findModItem(modelName))
    {
        // create and add controlers
        _models.push_back(modelName);
        if(ctrls==NULL)
            _ctrls.insert(modelName,new ModPlusCtrls(omProject(),omProject()->modelPlus(modelName)));
        else
            _ctrls.insert(modelName,ctrls);


        emit addedModel(modelName);
        return true;
    }
}

bool Optimization::addModels(QStringList models)
{
    for(int i=0;i<models.size();i++)
        addModel(models.at(i));
}


bool Optimization::removeModel(QString model)
{
    if(!_models.contains(model))
        return false;

    delete _ctrls.value(model);
    _models.removeAll(model);
    _ctrls.remove(model);

    // remove optimized variables
    int varToRemove = _optimizedVariables->findItem(model,OptVariable::MODEL);
    while(varToRemove>-1)
    {
        _optimizedVariables->removeRow(varToRemove);
        varToRemove = _optimizedVariables->findItem(model,OptVariable::MODEL);
    }

    // removed scanned variables
    varToRemove = _scannedVariables->findItem(model,ScannedVariable::MODEL);
    while(varToRemove>-1)
    {
        _scannedVariables->removeRow(varToRemove);
        varToRemove = _scannedVariables->findItem(model,ScannedVariable::MODEL);
    }

    // removed overwrited variables
    varToRemove = _overwritedVariables->findItem(model,Variable::MODEL);
    while(varToRemove>-1)
    {
        _overwritedVariables->removeRow(varToRemove);
        varToRemove = _overwritedVariables->findItem(model,Variable::MODEL);
    }

    // removed objectives
    varToRemove = _objectives->findItem(model,OptObjective::MODEL);
    while(varToRemove>-1)
    {
        _objectives->removeRow(varToRemove);
        varToRemove = _objectives->findItem(model,OptObjective::MODEL);
    }

    emit removedModel(model);
    return true;
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

    QDomElement cProblem = doc.createElement(Optimization::className());
    //***********************
    // Problem definition
    //***********************
    QDomElement cInfos = doc.createElement("Infos");
    cProblem.appendChild(cInfos);
    cInfos.setAttribute("name", name());


    QDomElement cModels = doc.createElement("Models");
    QString curModel;
    for(int i=0;i<_models.size();i++)
    {
        curModel = _models.at(i);
        QDomElement cModel = doc.createElement("Model");
        cModel.setAttribute("name",curModel);

        QDomElement cCtrls = this->ctrls(curModel)->toXmlData(doc);
        cModel.appendChild(cCtrls);
        cModels.appendChild(cModel);
    }
    cProblem.appendChild(cModels);

    // parameters
    QDomElement cParameters = _parameters->toXmlData(doc,"Parameters");
    cProblem.appendChild(cParameters);

    // Optimized variables
    QDomElement cOptVariables = _optimizedVariables->toXmlData(doc,"OptimizedVariables");
    cProblem.appendChild(cOptVariables);

    // objectives
    QDomElement cObjectives = _objectives->toXmlData(doc,"Objectives");
    cProblem.appendChild(cObjectives);

    // ScannedVariables
    QDomElement cScanned = _scannedVariables->toXmlData(doc,"ScannedVariables");
    cProblem.appendChild(cScanned);

    // Overwrited variables
    QDomElement cOverVars = _overwritedVariables->toXmlData(doc,"OverwritedVariables");
    cProblem.appendChild(cOverVars);


    //BlockSubstitutions
    QDomElement cBlocks = _blockSubstitutions->toXmlData(doc);
    cProblem.appendChild(cBlocks);

    // Files to copy
    QDomElement cFilesToCopy = doc.createElement("FilesToCopy");
    QStringList strList;
    for(int  i=0;i<_filesToCopy.size();i++)
        strList.push_back(_filesToCopy.at(i).absoluteFilePath());
    QDomText cFiles = doc.createTextNode(strList.join("\n"));
    cFilesToCopy.appendChild(cFiles);
    cProblem.appendChild(cFilesToCopy);

    // Algos
    QDomElement cAlgos = _algos->toXmlData(doc);
    cProblem.appendChild(cAlgos);

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

OptimAlgo* Optimization::getCurAlgo() const
{
    return _algos->currentAlgo();
}


QStringList Optimization::getAlgoNames()
{
    return _algos->getNames();
}

void Optimization::setCurAlgo(QString curAlgoName)
{
    _algos->setCurrentAlgo(curAlgoName);
}

/**
  * This function allows to finish Optimization the soonest but getting intermediary results.
  * e.g. in population algorithms, it stops at the end of the current generation. This allows
  * to keep results (thus differs from ProblemThread::onStopAsked() )
  */
void Optimization::onQuickEndAsked()
{
    getCurAlgo()->onQuickEndAsked();
}




ModPlusCtrl* Optimization::ctrl(QString model) const
{
    ModPlusCtrls* ctrls = _ctrls.value(model,NULL);
    if(!ctrls)
        return NULL;
    else
        return ctrls->currentCtrl();
}

ModPlusCtrls* Optimization::ctrls(QString model) const
{
    return _ctrls.value(model);
}


QMap<QString,ModPlusCtrls*> Optimization::ctrls() const
{
    return _ctrls;
}



ModPlusCtrl::Type  Optimization::ctrlType(QString model) const
{
    ModPlusCtrls* ctrls = _ctrls.value(model,NULL);
    if(!ctrls)
        return ModPlusCtrl::OPENMODELICA;
    else
        return ctrls->currentCtrlType();
}

void Optimization::setCtrlType(QString model,ModPlusCtrl::Type type)
{
    ModPlusCtrls* ctrls = _ctrls.value(model,NULL);
    if(ctrls)
        ctrls->setCurrentCtrlType(type);
}
