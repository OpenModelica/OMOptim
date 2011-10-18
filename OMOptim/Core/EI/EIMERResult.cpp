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

 	@file EIMERResult.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 

  */
#include "EIMERResult.h"

EIMERResult::EIMERResult(void)
{
	curveHot = new MOCCCurve(MOCCCurve::CCHOT);
	curveCold = new MOCCCurve(MOCCCurve::CCCOLD);
	curveGcc = new MOCCCurve(MOCCCurve::GCC);
        curveIccProcess = new MOCCCurve(MOCCCurve::ICCPROCESS);
        curveIccUtilities= new MOCCCurve(MOCCCurve::ICCUTILITIES);

}

EIMERResult::EIMERResult(const EIMERResult &result)
{
	curveHot =  new MOCCCurve(*result.curveHot);
	curveCold = new MOCCCurve(*result.curveCold);
	curveGcc = new MOCCCurve(*result.curveGcc);
        curveIccProcess = new MOCCCurve(*result.curveIccProcess);
        curveIccUtilities= new MOCCCurve(*result.curveIccUtilities);


	_project = result._project;
	_problem = result._problem;

        TPinch = result.TPinch;
        MER = result.MER;
        MERCold = result.MERCold;
}

EIMERResult::EIMERResult(Project* project, EIProblem * clonedProblem)
{
	curveHot = new MOCCCurve(MOCCCurve::CCHOT);
	curveCold = new MOCCCurve(MOCCCurve::CCCOLD);
	curveGcc = new MOCCCurve(MOCCCurve::GCC);
        curveIccProcess = new MOCCCurve(MOCCCurve::ICCPROCESS);
        curveIccUtilities= new MOCCCurve(MOCCCurve::ICCUTILITIES);

	_project = project;
        _problem = clonedProblem;
}

EIMERResult::~EIMERResult(void)
{
    emit deleted();

    delete curveHot;
    delete curveCold;
    delete curveGcc;
    delete curveIccProcess;
    delete curveIccUtilities;
}


int EIMERResult::problemType()
{
        return Problem::EIPROBLEMTYPE;
}

QDomElement EIMERResult::toXmlData(QDomDocument &)
{
	return QDomElement();
}

void EIMERResult::emitUpdated()
{
	emit updated();
}
