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

    @file Result.cpp
    @brief Comments for file documentation.
    @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
    Company : CEP - ARMINES (France)
    http://www-cep.ensmp.fr/english/
    @version

  */
#include "Result.h"
#include "LowTools.h"

Result::Result(ProjectBase * project)
    :OMCase(project)
{
    _success = false;
    _problem = NULL;
}
Result::Result(ProjectBase* project,const Problem & problem)
    :OMCase(project)
{
    _problem = problem.clone();
    _success = false;

    if(_problem)
        _name = _problem->name()+"_Result";
}

Result::Result(const Result &result)
    :OMCase(result)
{
    // indication pointers
    _project = result._project;

    // filled pointers
    if(result._problem)
        _problem = result._problem->clone();
    else
        _problem = NULL;

    // information
    _success = result._success;

    //_name = result._name;
    //_filesToCopyNames = result._filesToCopyNames;

    _duration = result._duration;
    _date = result._date;
}

Result::~Result(void)
{
    if(_problem)
        delete _problem;
}

void Result::setDefaultSaveFileName()
{
    if(_name.isEmpty())
        _saveFileName= "result.mpb";
    else
        _saveFileName = _name + ".mpb";
}

QString Result::getFieldName(int field,int role) const
{
    return "name";
}
unsigned Result::getNbFields() const
{
    return 1;
}

Problem* Result::problem() const
{
    return _problem;
}

void Result::setProblem(Problem & problem)
{
    if(_problem)
        delete _problem;

    _problem = problem.clone();
}

bool Result::isSuccess()
{
    return _success;
}

void Result::setSuccess(bool success)
{
    _success = success;
}


