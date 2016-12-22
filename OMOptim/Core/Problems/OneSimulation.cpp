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

  @file OneSimulation.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version

  */
#include "OneSimulation.h"
#include "Problem.h"
#include "Variable.h"
#include "LowTools.h"
#include "CSV.h"
#include "reportingHook.h"
#include "Project.h"
#include "ModPlusCtrls.h"
#include "VariablesManip.h"
#include "OneSimResult.h"
#include "ModModelPlus.h"
#include "ProblemConfig.h"


OneSimulation::OneSimulation(Project* project, ModelPlus* ModPlus)
    :Problem((ProjectBase*)project)
{

    _ModelPlus = ModPlus;
    if(!ModPlus)
        _name="One Simulation";
    else
        _name = "Simulation "+ModPlus->modelName().section(".",-1);
    _omProject = project;


    _overwritedVariables = new Variables(true,this);
    _scannedVariables = new ScannedVariables(true,this);


    // ctrls
    _ctrls = new ModPlusCtrls(project,ModPlus,this);

    // qDebug(QString("New OneSimulation").toLatin1().data());
}

OneSimulation::OneSimulation(const OneSimulation &oneSim)
    :Problem(oneSim)
{
    _omProject = oneSim._omProject;
    _ModelPlus = oneSim._ModelPlus;
    _neededFiles = oneSim._neededFiles;
    _filesToCopy = oneSim._filesToCopy;

    _overwritedVariables = oneSim._overwritedVariables->clone();
    _scannedVariables = oneSim._scannedVariables->clone();

    _ctrls = oneSim._ctrls->clone();

    // qDebug(QString("New OneSimulation").toLatin1().data());
}

OneSimulation::OneSimulation(QDomElement domProblem,Project* project,bool &ok)
    :Problem((ProjectBase*)project)
{
    _omProject = project;

    // look for modelPlus
    ok = (domProblem.tagName()==OneSimulation::className());
    ok = ok && !domProblem.isNull();

    QDomElement domInfos = domProblem.firstChildElement("Infos");
    QString modelName = domInfos.attribute("model");

    // Find model
    ModItem* modModel = ((Project*)_project)->findModItem(modelName);
    if(modModel == NULL)
    {
        InfoSender::instance()->sendWarning("Unable to find model "+modelName);
        ok = false;
    }

    _ModelPlus = project->modelPlus(modelName);
    if(!_ModelPlus)
    {
        ok = false;
        // create vector to avoid segfault when deleted
        _overwritedVariables = new Variables(true,this);
        _scannedVariables = new ScannedVariables(true,this);
        _ctrls = new ModPlusCtrls(project,_ModelPlus,this);
    }
    else
    {
        // finishing initialization
        _overwritedVariables = new Variables(true,this);
        _scannedVariables = new ScannedVariables(true,this);
        // Infos
        this->setName(domInfos.attribute("name", "" ));

        // Scanned Variables
        QDomElement domScanVars = domProblem.firstChildElement("ScannedVariables");
        this->scannedVariables()->setItems(domScanVars);

        // Overwrited Variables
        QDomElement domOverVars = domProblem.firstChildElement("OverwritedVariables");
        this->overwritedVariables()->setItems(domOverVars);

        // Files to copy
        QDomElement cFilesToCopy = domProblem.firstChildElement("FilesToCopy");
        QStringList strList = cFilesToCopy.text().split("\n",QString::SkipEmptyParts);
        for(int i=0;i<strList.size();i++)
            this->_filesToCopy.push_back(QFileInfo(strList.at(i)));

        // addOverWritedCVariables
        // make their value editable
//        for(int iV=0;iV<this->overwritedVariables()->size();iV++)
//            this->overwritedVariables()->at(iV)->setIsEditableField(Variable::VALUE,true);

        // Controler type
        QDomElement cControlers = domProblem.firstChildElement("Controlers");
        _ctrls = new ModPlusCtrls(project,_ModelPlus,cControlers,this);
    }

    // qDebug(QString("New OneSimulation").toLatin1().data());
}

Problem* OneSimulation::clone() const
{
    return new OneSimulation(*this);
}



OneSimulation::~OneSimulation(void)
{
    delete _overwritedVariables;
    delete _scannedVariables;
    delete _ctrls;

    // qDebug(QString("Remove OneSimulation").toLatin1().data());
}


bool OneSimulation::checkBeforeComp(QString & error)
{
    bool ok = true;
    return ok;
}

Result* OneSimulation::launch(ProblemConfig config)
{
    // Creating a variables instance containing updated variables
    Variables updatedVariables(*_overwritedVariables);

    OneSimResult* result = new OneSimResult(_omProject,_ModelPlus,*this);
    result->setName(this->name()+" result");

    // check if has modependencies (only for modmodelplus)
    QFileInfoList moDeps;
    ModModelPlus* modModelPlus = dynamic_cast<ModModelPlus*>(_ModelPlus);
    if(modModelPlus)
        moDeps.append(modModelPlus->moDependencies());

    // loop indexes on scannVariables
    QList<int> indexes,maxIndexes;
    Variable* clonedVar;
    ScannedVariable *scannedVar;
    for(int iScanV=0; iScanV < _scannedVariables->size(); iScanV++)
    {
        indexes.push_back(0);
        scannedVar = _scannedVariables->at(iScanV);
        int index = scannedVar->nbScans()-1;
        maxIndexes.push_back(index);
        clonedVar = new Variable(*(dynamic_cast<Variable*>(scannedVar)));
        updatedVariables.addItem(clonedVar);
    }

    MOVector<Variable> curVariables(true);
    bool allSimSuccess=true;
    bool curSimSuccess;
    int iScan=0;
    do
    {
        // Update values
        VariablesManip::updateScanValues(&updatedVariables,_scannedVariables,indexes);

        // Simulate
        curVariables.clear();
        curSimSuccess = ctrl()->simulate(_project->tempPath(), &updatedVariables, &curVariables,
                                         QFileInfoList() << _ModelPlus->neededFiles() << _filesToCopy,moDeps);
        allSimSuccess = allSimSuccess && curSimSuccess;

        if(allSimSuccess)
        {
            // Add values
            double curValue;
            //if it is first scan, finalvariables is an empy vector -> fill with curVariables
            if(result->finalVariables()->isEmpty())
            {
                for(int i=0;i<curVariables.size();i++)
                {
                    result->finalVariables()->addItem(new VariableResult(*curVariables.at(i)));
                    curValue = curVariables.at(i)->getFieldValue(Variable::VALUE).toDouble();
                    result->finalVariables()->at(i)->setFinalValue(iScan,0,curValue,true);
                }
            }
            else
            {
                // append scan values
                Variable *curVar;
                for(int i=0;i<result->finalVariables()->size();i++)
                {
                    curVar = curVariables.findItem(result->finalVariables()->at(i)->name());
                    if(curVar)
                    {
                        curValue = curVar->doubleValue();
                        result->finalVariables()->at(i)->setFinalValue(iScan,0,curValue,true);
                    }
                    else
                        result->finalVariables()->at(i)->setFinalValue(iScan,0,-1,false);
                }
            }
        }


        indexes = LowTools::nextIndex(indexes,maxIndexes);
        iScan++;
    }
    while(!indexes.isEmpty() && allSimSuccess);

    curVariables.clear();

    //qDebug(result->finalVariables()->toCSV().toLatin1().data());

    result->setSuccess(allSimSuccess);

    return result;
}


bool OneSimulation::setOverwritedVariableValue(QString modelName, QString varName, double value)
{
    Variable* overVar = _overwritedVariables->findVariable(modelName,varName);

    if(!overVar)
    {
        overVar = new Variable(varName);
        overVar->setModel(modelName);
        overVar->setValue(value);
        _overwritedVariables->addItem(overVar);
        return true;
    }
    else
    {
        overVar->setValue(value);
        return true;
    }
}

void OneSimulation::stop()
{
    ctrl()->stopSimulation();
}

bool OneSimulation::canBeStoped()
{
    return ctrl()->canBeStoped();
}

void OneSimulation::store(QString destFolder, QString tempDir)
{
    Problem::store(destFolder,tempDir);
}

QDomElement OneSimulation::toXmlData(QDomDocument & doc)
{
    QDomElement cProblem = doc.createElement(getClassName());

    //***********************
    // Problem definition
    //***********************
    QDomElement cInfos = doc.createElement("Infos");
    cProblem.appendChild(cInfos);
    cInfos.setAttribute("name",name());
    cInfos.setAttribute("model", _ModelPlus->modelName());

    // overwrited variables
    QDomElement cOverVariables = _overwritedVariables->toXmlData(doc,"OverwritedVariables");
    cProblem.appendChild(cOverVariables);

    // Scanned Variables
    QDomElement cScanVars = _scannedVariables->toXmlData(doc,"ScannedVariables");
    cProblem.appendChild(cScanVars);

    // Controlers
    QDomElement cControlers = ctrls()->toXmlData(doc);
    cProblem.appendChild(cControlers);

    // Files to copy
    QDomElement cFilesToCopy = doc.createElement("FilesToCopy");
    QStringList strFilesToCopy;
    for(int i=0;i<_filesToCopy.size();i++)
        strFilesToCopy.push_back(_filesToCopy.at(i).absoluteFilePath());
    QDomText cFiles = doc.createTextNode(strFilesToCopy.join("\n"));
    cFilesToCopy.appendChild(cFiles);
    cProblem.appendChild(cFilesToCopy);

    return cProblem;
}

ModelPlus* OneSimulation::modelPlus() const
{
    return _ModelPlus;
}

QString OneSimulation::model() const
{
    return _ModelPlus->modelName();
}

ModPlusCtrl* OneSimulation::ctrl()
{
    return _ctrls->currentCtrl();
}

ModPlusCtrls* OneSimulation::ctrls()
{
    return _ctrls;
}

ModPlusCtrl::Type OneSimulation::ctrlType()
{
    return _ctrls->currentCtrlType();
}

void OneSimulation::setCtrlType(ModPlusCtrl::Type type)
{
    _ctrls->setCurrentCtrlType(type);
}

/**
  * Copy controls from other ones
*/
void OneSimulation::setCtrls(const ModPlusCtrls & ctrls)
{
    _ctrls->setFromOtherCtrls(ctrls);
}

