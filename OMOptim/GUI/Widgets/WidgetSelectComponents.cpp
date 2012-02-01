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

  @file WidgetSelectComponents.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version
*/

#include "Widgets/WidgetSelectComponents.h"
#include "ui_WidgetSelectComponents.h"
#include <QtGui/QErrorMessage>


WidgetSelectComponents::WidgetSelectComponents(Project* project,Optimization* problem,bool isResult,QWidget *parent):
    QWidget(parent),
    _ui(new Ui::WidgetSelectComponentsClass)
{
    _ui->setupUi(this);

    _project = project;
    _problem = problem;
    _isResult = isResult;


    //Tree components
    _ui->treeLibrary->setModel(project->modItemsTree());

    ModItemsTree* modelTree = GuiTools::ModItemToTreeView(_project->modLoader(),_project->moomc(),*_problem->modModelPlus()->modModel(),
                                                           _ui->treeComponents,true);

    // Diagram of blocks
    _blockScene = new BlockSubsScene(_problem->blockSubstitutions(),_problem->modModelPlus()->modModel(),
                                     _project->modItemsTree(),true);
    _ui->graphBlocks->setScene(_blockScene);
    _ui->graphBlocks->setRenderHint(QPainter::Antialiasing);
    connect(_blockScene, SIGNAL(zoomRect(QRectF)),
            this, SLOT(doZoomRect(QRectF)));
    _blockScene->zoomFit();

    connect(_ui->pushAddReplaced,SIGNAL(clicked()), this, SLOT(addReplacedComponent()));
    connect(_ui->pushAddReplacing,SIGNAL(clicked()), this, SLOT(addReplacingComponent()));
}

WidgetSelectComponents::~WidgetSelectComponents()
{
    delete _ui;
}


void WidgetSelectComponents::addReplacedComponent()
{
    QModelIndex index = _ui->treeComponents->currentIndex();
    ModItem* curComponent = static_cast<ModItem*>(index.internalPointer());


    QString replacedCompName = curComponent->name(Modelica::FULL);

    QStringList alreadyReplaced = _problem->blockSubstitutions()->getReplacedComponentsNames();

    bool ok=true;

    if(!alreadyReplaced.contains(replacedCompName))
    {
        BlockSubstitution *newBlockSub = new BlockSubstitution(_project,_problem->modModelPlus(),_problem->modModelPlus()->connections(),
                                                               replacedCompName,QString(),true,ok);

        if(ok)
            _problem->blockSubstitutions()->add(newBlockSub);
        else
            delete newBlockSub;
    }

    if(ok)
        _blockScene->selectOrg(replacedCompName,true);
}


void WidgetSelectComponents::addReplacingComponent()
{

    // looking for selected component to replace
    QStringList replacedCompNames = _blockScene->getSelectedOrg();

    if(replacedCompNames.size()==1)
    {
        // replacing component
        QModelIndex index = _ui->treeLibrary->currentIndex();
        ModItem* curReplacingComponent = static_cast<ModItem*>(index.internalPointer());
        QString replacingCompName = curReplacingComponent->name(Modelica::FULL);

        // look for an existing but empty block
        bool ok;
        BlockSubstitution *newBlockSub = new BlockSubstitution(_project,_problem->modModelPlus(),_problem->modModelPlus()->connections(),
                                                               replacedCompNames.at(0),replacingCompName,true,ok);

        if(!ok)
            delete newBlockSub;
        else
        {
            if(openConnections(newBlockSub))
            {
                _problem->blockSubstitutions()->add(newBlockSub);

                // refreshing gui
                //actualizeReplacingComponentList();
                _blockScene->selectSub(replacedCompNames.at(0),replacingCompName,true);
            }
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Please select one and only one component to replace.");
        msgBox.exec();
    }
}

bool WidgetSelectComponents::openConnections(BlockSubstitution* blockSub)
{
    // creating form for specifying connections
    BlockSubstituteConnDlg *form = new BlockSubstituteConnDlg(blockSub,this);
    return (form->exec() == QDialog::Accepted);
}

void WidgetSelectComponents::doZoomRect(QRectF _rect)
{
    _ui->graphBlocks->setSceneRect(_rect);

}


void WidgetSelectComponents::actualizeGui()
{
    // list of widgets to hide when problem is solved
    QWidgetList unsolvedWidgets;
    unsolvedWidgets << _ui->pushAddReplaced << _ui->pushAddReplacing ;

    // list of widgets to hide when problem is unsolved
    QWidgetList solvedWidgets;

    QList<QTableView*> tables;


    // if problem is solved
    if(_isResult)
    {
        for(int i=0; i < unsolvedWidgets.size(); i++)
            unsolvedWidgets.at(i)->hide();
	
        for(int i=0; i < solvedWidgets.size(); i++)
            solvedWidgets.at(i)->show();
	
        for(int i=0; i< tables.size(); i++)
            tables.at(i)->setEditTriggers(QAbstractItemView::NoEditTriggers);

    }
    else
    {
        for(int i=0; i < unsolvedWidgets.size(); i++)
            unsolvedWidgets.at(i)->show();
	
        for(int i=0; i < solvedWidgets.size(); i++)
            solvedWidgets.at(i)->hide();
	
        for(int i=0; i< tables.size(); i++)
            tables.at(i)->setEditTriggers(QAbstractItemView::DoubleClicked);

    }
}
