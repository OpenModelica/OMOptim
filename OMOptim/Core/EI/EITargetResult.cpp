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

 	@file EITargetResult.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "EITargetResult.h"

EITargetResult::EITargetResult(void)
:Result()
{
        _eiTree = new EITree();
        _eiConns = new EIConns();

        _name = "EITargetResult";
}
EITargetResult::EITargetResult(Project* project, ModClassTree* modClassTree,Problem* problem)
:Result(project,modClassTree,problem)
{

        _eiTree = new EITree();
        _eiConns = new EIConns();

}

EITargetResult::EITargetResult(Project* project, ModClassTree* modClassTree,QDomElement domResult,Problem* problem)
    :Result(project,modClassTree,problem)
{

    _eiTree = new EITree();
    _eiConns = new EIConns();



    //**************
    // Result
    //**************
    if(!domResult.isNull())
    {
        this->setSuccess(true);

        //Infos
        QDomElement domInfos = domResult.firstChildElement("Infos");
        QString resultName = domInfos.attribute("name");
        setName(resultName);

        // EI
        QDomElement domEI = domResult.firstChildElement("EIItem");
        EIControler::setItems(domEI,_eiTree->rootElement());

        // Values
        QDomElement cValues = domResult.firstChildElement("Values");
        QString strTotalCost = cValues.attribute("TotalCost");
        _totalCost = strTotalCost.toDouble();

        // EIConns
        QDomElement domEIConns = domResult.firstChildElement("EIConns");
        _eiConns->setItems(domEIConns,_eiTree);

        QDomElement cFiles = domResult.firstChildElement("Files");
        _logFileName = cFiles.attribute("LogFile");
        _resFileName = cFiles.attribute("ResFile");
        _sensFileName = cFiles.attribute("SensFile");

    }
}

EITargetResult::~EITargetResult(void)
{
    delete _eiTree;
    delete _eiConns;
}

QDomElement EITargetResult::toXmlData(QDomDocument & doc)
{
        // Result element
        QDomElement cRoot = doc.createElement("OMResult");
        QDomElement cResult = doc.createElement(this->getClassName());
        cRoot.appendChild(cResult);

	// Problem definition
	QDomElement cInfos = doc.createElement("Infos");
	cInfos.setAttribute("name", _name);
	cInfos.setAttribute("type", problemType());
	cResult.appendChild(cInfos);

        // Root EI
        QDomElement cEI = _eiTree->toXmlData(doc);
        cResult.appendChild(cEI);

        // Total cost
        QDomElement cValues = doc.createElement("Values");
        cValues.setAttribute("TotalCost",_totalCost);
        cResult.appendChild(cValues);

        // files
        QDomElement cFiles = doc.createElement("Files");
        cFiles.setAttribute("LogFile",_logFileName);
        cFiles.setAttribute("ResFile",_resFileName);
        cFiles.setAttribute("SensFile",_sensFileName);
        cResult.appendChild(cFiles);

        // HLD
        QDomElement cEIConns = _eiConns->toXmlData(doc,"EIConns");
        cResult.appendChild(cEIConns);

	return cResult;
}

EITree* EITargetResult::eiTree()
{
    return _eiTree;
}


void EITargetResult::setEITree(const EITree & eiTree)
{
    if(_eiTree )
        delete _eiTree;
    _eiTree = new EITree(eiTree);
}


EIConns* EITargetResult::eiConns()
{
    return _eiConns;
}

void EITargetResult::setEIConns(EIConns * eiConns)
{
    if(_eiConns && (_eiConns!=eiConns))
        delete _eiConns;
    _eiConns = eiConns;
}
