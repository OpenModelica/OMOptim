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

  @file OneSimResult.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version

  */
#include "OneSimResult.h"
#include "OneSimulation.h"

OneSimResult::OneSimResult(void)
{
    _inputVariables = new Variables(true);
    _finalVariables = new MOOptVector(true,true,false); //can have several scans but not several points

    // files to copy
    _filesToCopy << "dsin.txt";
}

OneSimResult::OneSimResult(Project* project, ModModelPlus* modelPlus, const OneSimulation &problem)
    :Result(project,(const Problem&)problem)
{
    _modModelPlus = modelPlus;

    _inputVariables = new Variables(true,modelPlus);
    _finalVariables = new MOOptVector(true,true,false); //can have several scans but not several points

    // files to copy
    _filesToCopy << "dsin.txt";
}

OneSimResult::OneSimResult(Project* project, const QDomElement & domResult,const OneSimulation &problem, bool &ok)
:Result(project,(const Problem&)problem)
{

    _modModelPlus = problem.modModelPlus();

    ok = (domResult.tagName()==OneSimResult::className());

    //Infos
    QDomElement domResInfos = domResult.firstChildElement("Infos");
    this->setName(domResInfos.attribute("name", "" ));

    // input variables
    _inputVariables = new Variables(true,_modModelPlus);

    //FinalVariables
    _finalVariables = new MOOptVector(true,true,false); //can have several scans but not several points
    QDomElement domFinalVars = domResult.firstChildElement("FinalVariables");
    this->finalVariables()->setItems(domFinalVars);
}

OneSimResult::~OneSimResult(void)
{
    _inputVariables->deleteLater();
    _finalVariables->deleteLater();
}

QDomElement OneSimResult::toXmlData(QDomDocument & doc)
{
    QDomElement cRoot = doc.createElement("OMResult");
    QDomElement cResult = doc.createElement(this->getClassName());
    cRoot.appendChild(cResult);

    // Problem definition
    QDomElement cInfos = doc.createElement("Infos");
    cInfos.setAttribute("name", _name);
 //   cInfos.setAttribute("type", problemType());
    cResult.appendChild(cInfos);

    // Final variables
    QDomElement cFinalVariables = _finalVariables->toXmlData(doc,"FinalVariables");
    cResult.appendChild(cFinalVariables);
    return cResult;
}

ModPlusCtrl* OneSimResult::modPlusCtrl()
{
    OneSimulation* oneSim = dynamic_cast<OneSimulation*>(_problem);
    if(oneSim)
    {
        return oneSim->ctrl();
    }
    else
        return NULL;
}

