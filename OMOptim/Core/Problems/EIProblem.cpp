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
        @version

  */

#include "EIProblem.h"
#include "OneSimulation.h"
#include "OneSimResult.h"
#include "Project.h"

EIProblem::EIProblem(Project* project,ModClassTree* modClassTree,MOomc* moomc)
    :Problem(project,modClassTree)
{
    _type = Problem::EIPROBLEMTYPE;
    _name="EI";

    _eiTree = new EITree(project);

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

}

EIProblem::EIProblem(Project* project,ModClassTree* modClassTree,MOomc* moomc,QDomElement domProblem)
    :Problem(project,modClassTree)
{
    _type = Problem::EIPROBLEMTYPE;
    _name="EI";

    _eiTree = new EITree(project);

    _moomc = moomc;
    _connConstrs = new EIConnConstrs();
    _inputVars = new MOOptVector(false,false);


    QDomElement domInfos = domProblem.firstChildElement("Infos");
    QString problemName = domInfos.attribute("name");

    // Infos
    setType((Problem::ProblemType)domInfos.attribute("type", "" ).toInt());
    setName(problemName);

    // EI
    QDomElement domEI = domProblem.firstChildElement("EIItem");
    EIControler::setItems(domEI,eiTree()->rootElement());

    // InputVars
    QDomElement domInputVars = domProblem.firstChildElement("InputVars");
    _inputVars->setItems(domInputVars);

    // Conn sontrs
    QDomElement domConnConstrs = domProblem.firstChildElement(EIConnConstrs::className());
    _connConstrs->setItems(domConnConstrs,eiTree());

}


QDomElement EIProblem::toXmlData(QDomDocument & doc)
{
    QDomElement cProblem = doc.createElement(getClassName());
    //***********************
    // Problem definition
    //***********************
    QDomElement cInfos = doc.createElement("Infos");
    cProblem.appendChild(cInfos);
    cInfos.setAttribute("name", name());
    cInfos.setAttribute("type", type());

    // EI
    QDomElement cEI = _eiTree->toXmlData(doc);
    cProblem.appendChild(cEI);

    // Variables
    QDomElement cResultVars = _inputVars->toXmlData(doc,"ResultVars");
    cProblem.appendChild(cResultVars);

    // Conn constr
    QDomElement cConnConstr = _connConstrs->toXmlData(doc,_connConstrs->getClassName());
    cProblem.appendChild(cConnConstr);

    return cProblem;
}


void EIProblem::loadModel(ModModel* loadedModel)
{
    _eiTree->loadModel(loadedModel);


   // delete simRes;
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


void EIProblem::setInputVars(MOOptVector *variables)
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

void EIProblem::setEITree(const EITree & eiTree)
{
    if(_eiTree)
        delete _eiTree;

    _eiTree = new EITree(eiTree);
}
