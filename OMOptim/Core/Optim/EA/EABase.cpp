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

    @file EABase.cpp
    @brief Comments for file documentation.
    @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
    Company : CEP - ARMINES (France)
    http://www-cep.ensmp.fr/english/
    @version

  */

#include "EABase.h"
#include "Project.h"

EABase::EABase(void)
    :OptimAlgo()
{

}


EABase::EABase(Project* project,Problem* problem)
    :OptimAlgo()
{
    _project = project;
    _problem = problem;
    if(project)
        _modItemsTree = project->modItemsTree();
    else
        _modItemsTree = NULL;
    _quickEnd = false;
    _keepResults = true;
}

EABase::EABase(const EABase & eaBase)
    :OptimAlgo(eaBase)
{
    _modItemsTree = eaBase._modItemsTree;
 //   _subModels = eaBase._subModels;
  //  _subBlocks = eaBase._subBlocks;
    _index = eaBase._index;
    _quickEnd = eaBase._quickEnd;
    _keepResults = eaBase._keepResults;
}

EABase::~EABase(void)
{

}

//void EABase::setSubModels(QList<QList<ModelPlus*> > subModels,QList<BlockSubstitutions*> subBlocks)
//{
//    _subModels = subModels;
//    _subBlocks = subBlocks;

//    if(_subModels.size()>1)
//    {
//        _useSubModels = true;
//    }
//}

void EABase::quickEnd()
{
    _quickEnd = true;
}

void EABase::stop()
{
    _quickEnd = true;
    _keepResults = false;
}
