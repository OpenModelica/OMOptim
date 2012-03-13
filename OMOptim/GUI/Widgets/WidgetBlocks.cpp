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

     @file WidgetBlocks.cpp
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version 
*/

#include "Widgets/WidgetBlocks.h"
#include "ui_WidgetBlocks.h"
#include <QtGui/QErrorMessage>



WidgetBlocks::WidgetBlocks(Project* project,OptimResult* result,QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::WidgetBlocksClass)
{
    _ui->setupUi(this);
    

    _result = result;
    _project = project;
    connect(_result,SIGNAL(curPointChanged()),this,SLOT(onResultCurPointChanged()));

    _blockScene = new BlockSubsScene(((Optimization*)_result->problem())->blockSubstitutions(),_result->modModel(),
                _project->modItemsTree(),true);
    _ui->graphBlocks->setScene(_blockScene);
    _ui->graphBlocks->setRenderHint(QPainter::Antialiasing);
    connect(_blockScene, SIGNAL(zoomRect(QRectF)),
            this, SLOT(doZoomRect(QRectF)));
    _blockScene->zoomFit();

}

WidgetBlocks::~WidgetBlocks()
{
    delete _ui;
}


void WidgetBlocks::onResultCurPointChanged()
{
    int curPoint = _result->curPoint();

    if (curPoint==-1)
    {
        _blockScene->deselectAll();
    }
    else
    {
        if(curPoint<_result->_iSubModels.size())
        {
            int iSubModel = _result->_iSubModels.at(curPoint);
            if(iSubModel< _result->_subBlocks.size())
            {
                QStringList subBlocks;
                QStringList orgBlocks;

                BlockSubstitutions* selectedBlocks  = _result->_subBlocks.at(iSubModel);

                for(int i=0;i<selectedBlocks->size();i++)
                {
                                        orgBlocks << selectedBlocks->at(i)->_orgComponent;
                                        subBlocks << selectedBlocks->at(i)->_subComponent;
                }
                _blockScene->selectSubs(orgBlocks,subBlocks,true);    
            }
            else
            {
                _blockScene->deselectAll();
            }
        }
    }
}
