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

 	@file EIProblem.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */

#include "EIProblem.h"

EIProblem::EIProblem(Project* project,ModClassTree* modClassTree,MOomc* moomc)
    :Problem(project,modClassTree)
{
    _type = Problem::EIPROBLEM;
    _name="EI";

    _eiTree = new EITree();

    _moomc = moomc;
    _connConstrs = new EIConnConstrs();
    _inputVars = new MOOptVector(false,false);
}

EIProblem::~EIProblem(void)
{
    delete _eiTree;
    delete _connConstrs;
    delete _inputVars;
}


EIProblem::EIProblem(const EIProblem &problem)
    :Problem(problem)
{

    if(problem._eiTree==NULL)
    {
        _eiTree=NULL;
    }
    else
        _eiTree = new EITree(*problem._eiTree);


    _moomc = problem._moomc;
    _modClassTree = problem._modClassTree;

    _connConstrs = problem._connConstrs->clone();
    _inputVars = problem._inputVars->clone();
    _parameters = problem._parameters->clone();
}

void EIProblem::loadModel(ModModel* loadedModel)
{
    bool eraseExisting=true;
    EIItem* modelRootEI = EIModelExtractor::extractFromModClass(loadedModel,_modClassTree,_moomc);

    bool unloadOk;
    if(eraseExisting)
        unloadModel(loadedModel,unloadOk);

    if(unloadOk)
        _eiTree->addChild(_eiTree->rootElement(),modelRootEI);
}

void EIProblem::unloadModel(ModModel* unloadedModel, bool &ok)
{

    EIItem*  eiItem = _eiTree->findItem(EI::MODELCONTAINER,unloadedModel->name(),EIModelContainer::MODEL);

    ok= true;


    if(eiItem)
    {
        ok = _eiTree->removeItem(eiItem);
    }
}

EIConnConstrs* EIProblem::connConstrs()
{
    return _connConstrs;
}

void EIProblem::setConnConstrs(EIConnConstrs* connConstr)
{
    if(connConstr!=_connConstrs)
        delete _connConstrs;

    _connConstrs = connConstr;
}

void EIProblem::clearInputVars()
{
    _inputVars->clear();
}

void EIProblem::updateInputVars(MOOptVector *addedVars)
{
    if(addedVars->getUsePoints())
        _inputVars->setUsePoints(true);

    if(addedVars->getUseScan())
        _inputVars->setUseScan(true);

    _inputVars->append(*addedVars,true);

    emit inputVarsModified();
}


void EIProblem::setInputVars(MOOptVector* variables)
{
    if(_inputVars!=variables)
        delete _inputVars;

    _inputVars = variables;
}

MOOptVector * EIProblem::inputVars()
{
    return _inputVars;
}

EITree* EIProblem::eiTree()
{
    return _eiTree;
}

void EIProblem::setEITree(EITree* eiTree)
{
    if(_eiTree != eiTree)
        delete _eiTree;

    _eiTree = eiTree;
}
