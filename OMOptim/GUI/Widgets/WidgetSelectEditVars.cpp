// $Id: WidgetSelectEditVars.cpp 15963 2013-05-02 19:17:13Z sjoelund.se $
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

  @file WidgetSelectEditVars.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version
*/

#include "Widgets/WidgetSelectEditVars.h"
#include "ui_WidgetSelectEditVars.h"
#include "Optimization.h"
#include "Project.h"
#include "MOStyleSheet.h"
#include "ModExePlus.h"
#include "ModModelPlus.h"
#include <QToolBar>
#include "Dialogs/DlgSelectVars.h"

WidgetSelectEditVars::WidgetSelectEditVars(Project* project,bool isEditable,bool showAllVariablesOnFront,QWidget *parent):
    QWidget(parent),
    _ui(new Ui::WidgetSelectEditVarsClass)
{
    _ui->setupUi(this);
    _project = project;
    _showAllVariablesOnFront = showAllVariablesOnFront;


    _isEditable = isEditable;

    // concatenate model variables
    _allModelsVars = new Variables(true,this);
    for(int i=0;i<models().size();i++)
    {
        ModelPlus* modelPlus = _project->modelPlus(models().at(i));
        if(modelPlus)
        {
            Variables* modelVars = modelPlus->variables();
            connect(modelVars,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(refreshAllModelsVars()));
        }
    }

    // create permanent vars vector
    _permanentVars = new Variables(true,this);



    _tableSamplingVars = NULL;
    _tableOptimizedVars = NULL;
    _tableObjectives = NULL;
    _tableOverVars = NULL;
    _tableVariables = NULL;


    //buttons
    connect(_ui->pushAddVariables, SIGNAL(clicked()), this, SLOT(addOptVariables()));
    connect(_ui->pushRemoveVariables, SIGNAL(clicked()), this, SLOT(deleteOptVariables()));
    connect(_ui->pushAddObjectives, SIGNAL(clicked()), this, SLOT(addOptObjectives()));
    connect(_ui->pushRemoveObjectives, SIGNAL(clicked()), this, SLOT(deleteOptObjectives()));
    connect(_ui->pushAddSampling, SIGNAL(clicked()), this, SLOT(addSamplingVariables()));
    connect(_ui->pushRemoveSampling, SIGNAL(clicked()), this, SLOT(deleteSamplingVariables()));
    connect(_ui->pushAddParam, SIGNAL(clicked()), this, SLOT(addOverVariables()));
    connect(_ui->pushRemoveParam, SIGNAL(clicked()), this, SLOT(deleteOverVariables()));




}

///**
//  * Used only for inheriting purposes
//  */
//WidgetSelectEditVars::WidgetSelectEditVars(QWidget *parent):
//    QWidget(parent)
//{

//}

WidgetSelectEditVars::~WidgetSelectEditVars()
{
    delete _ui;
    delete _allModelsVars;
    delete _permanentVars;
}
/** init GUI : to be called in inheriting class ctor!*/
void WidgetSelectEditVars::initialize()
{

    // create tables
    if(_showAllVariablesOnFront)
    {
        _tableVariables = new MOTableView(this);
        _tableVariables->setSelectionBehavior(QAbstractItemView::SelectRows);
        _tableVariables->setSelectionMode(QAbstractItemView::ExtendedSelection);
        _ui->layoutTableVariables->addWidget(_tableVariables);
        _variableProxyModel = GuiTools::ModelToViewWithFilter(_allModelsVars,
                                                              _tableVariables,_ui->lineVariableFilter);
        _tableVariables->resizeColumnsToContents();
    }
    else
        _ui->widgetAllVars->deleteLater();

    if(scannedVariables())
    {
        _tableSamplingVars = new MOTableView(this);
        _tableSamplingVars->setSelectionBehavior(QAbstractItemView::SelectRows);
        _ui->layoutSamplingVars->addWidget(_tableSamplingVars);
        _samplingProxyModel = GuiTools::ModelToViewWithFilter(scannedVariables(),
                                                              _tableSamplingVars,NULL);
        _tableSamplingVars->resizeColumnsToContents();
    }
    else
        _ui->groupSampling->deleteLater();//->hide();

    if(optimizedVariables())
    {
        _tableOptimizedVars = new MOTableView(this);
        _tableOptimizedVars->setSelectionBehavior(QAbstractItemView::SelectRows);
        _ui->layoutOptimizedVars->addWidget(_tableOptimizedVars);
        _optVariableProxyModel = GuiTools::ModelToViewWithFilter(optimizedVariables(),
                                                                 _tableOptimizedVars,NULL);
        _tableOptimizedVars->resizeColumnsToContents();
    }
    else
        _ui->groupOptimized->deleteLater();//->hide();

    if(objectives())
    {
        _tableObjectives = new MOTableView(this);
        _tableObjectives->setSelectionBehavior(QAbstractItemView::SelectRows);
        _ui->layoutObjectives->addWidget(_tableObjectives);
        _objectiveProxyModel = GuiTools::ModelToViewWithFilter(objectives(),
                                                               _tableObjectives,NULL);
        _tableObjectives->resizeColumnsToContents();

    }
    else
        _ui->groupObjectives->deleteLater();

    if(overwritedVariables())
    {
        _tableOverVars = new MOTableView(this);
        _tableOverVars->setSelectionBehavior(QAbstractItemView::SelectRows);
        _ui->layoutOvervars->addWidget(_tableOverVars);
        _overVarsProxyModel = GuiTools::ModelToViewWithFilter(overwritedVariables(),
                                                              _tableOverVars,NULL);
        _tableOverVars->resizeColumnsToContents();

    }
    else
        _ui->groupParameters->deleteLater();


    setShownColumns();
    setDelegates();
    refreshAllModelsVars();
    initToolBar();
}

void WidgetSelectEditVars::initToolBar()
{
    // create a toolbar
    _actionReadVariables = new QAction( QIcon(":/Refresh"),"Read variables",this);
    connect(_actionReadVariables,SIGNAL(triggered()),this,SLOT(readVariables()));

    _actionShowParameters = new QAction( "Parameters",this);
    connect(_actionShowParameters,SIGNAL(toggled(bool)),_ui->groupParameters,SLOT(setShown(bool)));
    _actionShowParameters->setCheckable(true);
    _actionShowParameters->setChecked(true);
    _actionShowParameters->setVisible(overwritedVariables());

    _actionShowObjectives = new QAction( "Objectives",this);
    connect(_actionShowObjectives,SIGNAL(toggled(bool)),_ui->groupObjectives,SLOT(setShown(bool)));
    _actionShowObjectives->setCheckable(true);
    _actionShowObjectives->setChecked(true);
    _actionShowObjectives->setVisible(objectives());

    _actionShowOptimized = new QAction( "Optimized",this);
    connect(_actionShowOptimized,SIGNAL(toggled(bool)),_ui->groupOptimized,SLOT(setShown(bool)));
    _actionShowOptimized->setCheckable(true);
    _actionShowOptimized->setChecked(true);
    _actionShowOptimized->setVisible(optimizedVariables());


    _actionShowSampling = new QAction( "Sampling variables",this);
    connect(_actionShowSampling,SIGNAL(toggled(bool)),_ui->groupSampling,SLOT(setShown(bool)));
    _actionShowSampling->setCheckable(true);
    _actionShowSampling->setChecked(true);
    _actionShowSampling->setVisible(scannedVariables());

    QToolBar* toolBar = new QToolBar(this);
    this->setStyleSheet("QToolButton { color:white;} QToolButton:checked {background-color: gray; color : white;border-color: palette(highlight)}");
    toolBar->setStyleSheet(MOStyleSheet::toolBarStyleSheet());

    toolBar->addAction(_actionReadVariables);
    toolBar->addSeparator();
    toolBar->addAction(_actionShowParameters);
    toolBar->addAction(_actionShowObjectives);
    toolBar->addAction(_actionShowOptimized);
    toolBar->addAction(_actionShowSampling);
    _ui->layoutToolBar->addWidget(toolBar);
    toolBar->setProperty("frameShape",QFrame::NoFrame);


}

void WidgetSelectEditVars::addPermanentVars(Variables * vars)
{
    _permanentVars->addItems(vars,true);
}

void WidgetSelectEditVars::addPermanentVar(const Variable & var)
{
    _permanentVars->addItem(var.clone());
}

void WidgetSelectEditVars::removePermanentVars(QString model)
{
    QList<int> toRemove;
    for(int i=0;i<_permanentVars->size();i++)
        if(_permanentVars->at(i)->model()==model)
            toRemove.push_back(i);

    _permanentVars->removeRows(toRemove);
}

QList<Variable*> WidgetSelectEditVars::getSelectedVariables()
{
    QList<Variable*> vars;

    if(_showAllVariablesOnFront)
    {
        QModelIndexList proxyIndexes = _tableVariables->selectionModel()->selectedRows();
        QModelIndex curProxyIndex;
        QModelIndex curSourceIndex;
        Variable* selVar;

        foreach(curProxyIndex, proxyIndexes)   // loop through and remove them
        {
            curSourceIndex = _variableProxyModel->mapToSource(curProxyIndex);

            selVar=_allModelsVars->at(curSourceIndex.row());
            vars.push_back(selVar);
        }
    }
    else
    {
        MOVector<Variable> * selectedVars = DlgSelectVars::getSelectedVars(_allModelsVars);
        Variable* var;

        if(selectedVars)
        {
            for(int i=0;i<selectedVars->size();i++)
            {
                var = selectedVars->at(i);
                vars.push_back(var);
            }
        }
    }
    return vars;
}


void WidgetSelectEditVars::addOptVariables()
{
    QList<Variable*> vars = getSelectedVariables();
    Variable* var;
    OptVariable* optVarProv;
    bool alreadyIn;
    for(int i=0;i<vars.size();i++)
    {
        var = vars.at(i);
        alreadyIn = optimizedVariables()->alreadyIn(var->name());
        //        if ((!alreadyIn && !selVar->output()))
        if (!alreadyIn)
        {
            optVarProv = new OptVariable(*var);
            optimizedVariables()->addItem(optVarProv);
        }
    }
    _tableOptimizedVars->resizeColumnsToContents();
}

void WidgetSelectEditVars::deleteOptVariables()
{
    QModelIndexList indexList = _tableOptimizedVars->selectionModel()->selectedRows();
    QModelIndex curSourceIndex;

    QList<int> rows;
    for(int i=0;i<indexList.size();i++)
    {
        curSourceIndex = _optVariableProxyModel->mapToSource(indexList.at(i));
        rows.push_back(curSourceIndex.row());
    }
    optimizedVariables()->removeRows(rows);

    _tableOptimizedVars->resizeColumnsToContents();

}

void WidgetSelectEditVars::addSamplingVariables()
{
    QList<Variable*> vars = getSelectedVariables();
    Variable* var;
    ScannedVariable* scannedVarProv;
    bool alreadyIn;
    for(int i=0;i<vars.size();i++)
    {
        var = vars.at(i);
        alreadyIn = scannedVariables()->alreadyIn(var->name());
        //        if ((!alreadyIn && !selVar->output()))
        if (!alreadyIn)
        {
            scannedVarProv = new ScannedVariable(*var);
            scannedVariables()->addItem(scannedVarProv);
        }
    }
    _tableSamplingVars->resizeColumnsToContents();

}

void WidgetSelectEditVars::deleteSamplingVariables()
{
    QModelIndexList indexList = _tableSamplingVars->selectionModel()->selectedRows();
    QModelIndex curSourceIndex;

    QList<int> rows;
    for(int i=0;i<indexList.size();i++)
    {
        curSourceIndex = _samplingProxyModel->mapToSource(indexList.at(i));
        rows.push_back(curSourceIndex.row());
    }
    scannedVariables()->removeRows(rows);

    _tableSamplingVars->resizeColumnsToContents();
}

void WidgetSelectEditVars::addOverVariables()
{
    QList<Variable*> vars = getSelectedVariables();
    Variable* var;
    Variable* varProv;
    bool alreadyIn;
    for(int i=0;i<vars.size();i++)
    {
        var = vars.at(i);
        alreadyIn = overwritedVariables()->alreadyIn(var->name());
        //        if ((!alreadyIn && !selVar->output()))
        if (!alreadyIn)
        {
            varProv = new Variable(*var);
            overwritedVariables()->addItem(varProv);
        }
    }
    _tableOverVars->resizeColumnsToContents();
}

void WidgetSelectEditVars::deleteOverVariables()
{
    QModelIndexList indexList = _tableOverVars->selectionModel()->selectedRows();
    QModelIndex curSourceIndex;

    QList<int> rows;
    for(int i=0;i<indexList.size();i++)
    {
        curSourceIndex = _overVarsProxyModel->mapToSource(indexList.at(i));
        rows.push_back(curSourceIndex.row());
    }
    overwritedVariables()->removeRows(rows);

    _tableOverVars->resizeColumnsToContents();
}



void WidgetSelectEditVars::addOptObjectives()
{
    QList<Variable*> vars = getSelectedVariables();
    Variable* var;
    OptObjective* newObj;
    bool alreadyIn;
    for(int i=0;i<vars.size();i++)
    {
        var = vars.at(i);
        alreadyIn = objectives()->alreadyIn(var->name());
        //        if ((!alreadyIn && !selVar->output()))
        if (!alreadyIn)
        {
            newObj = new OptObjective(*var);
            objectives()->addItem(newObj);
        }
    }
    _tableObjectives->resizeColumnsToContents();
}

void WidgetSelectEditVars::deleteOptObjectives()
{
    QModelIndexList indexList = _tableObjectives->selectionModel()->selectedRows();
    QList<int> rows;
    QModelIndex curSourceIndex;

    for(int i=0;i<indexList.size();i++)
    {
        curSourceIndex = _objectiveProxyModel->mapToSource(indexList.at(i));
        rows.push_back(curSourceIndex.row());
    }
    objectives()->removeRows(rows);
    _tableObjectives->resizeColumnsToContents();
}




void WidgetSelectEditVars::actualizeGui()
{
    // list of widgets to hide when problem is solved
    QWidgetList unsolvedWidgets;
    unsolvedWidgets << _ui->pushAddObjectives << _ui->pushAddVariables << _ui->pushAddSampling;
    unsolvedWidgets << _ui->pushRemoveObjectives << _ui->pushRemoveVariables << _ui->pushRemoveSampling;

    // list of widgets to hide when problem is unsolved
    QWidgetList solvedWidgets;

    QList<QTableView*> tables;
    tables << _tableObjectives << _tableOptimizedVars << _tableVariables ;
    tables.removeAll(NULL);

    // if problem is solved
    if(!_isEditable)
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


void WidgetSelectEditVars::readVariables()
{

    _allModelsVars->clear();

    for(int i=0;i<models().size();i++)
    {
        ModelPlus* curModelPlus = _project->modelPlus(models().at(i));

        // is compiled
        bool isCompiled;

        ModPlusCtrl* modCtrl = ctrl(models().at(i));
        if(modCtrl)
        {

            if(curModelPlus->modelType() == ModelPlus::MODELICA)
                isCompiled = ((ModModelPlus *) curModelPlus)->isCompiled(modCtrl);

            if( curModelPlus->modelType() == ModelPlus::MODELICA)
            {
                bool shouldForceRecompile = false;
                if(isCompiled)
                {
                    // already compiled, ask user if we should only read init file or recompile model
                    QMessageBox msgBox;
                    msgBox.setText("Model "+models().at(i)+" is already compiled.");
                    msgBox.setInformativeText("Would you like to compile it again ?");
                    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                    msgBox.setDefaultButton(QMessageBox::Yes);
                    int ret = msgBox.exec();
                    switch (ret)
                    {
                    case QMessageBox::Yes:
                        shouldForceRecompile = true;
                        break;
                    case QMessageBox::No:
                        shouldForceRecompile = false;
                        break;
                    }
                }
                ((ModModelPlus *)curModelPlus)->readVariables(modCtrl,QFileInfoList(),shouldForceRecompile);
            }
            if(curModelPlus->modelType() == ModelPlus::EXECUTABLE)
                ((ModExePlus *) curModelPlus)->readVariables(modCtrl);
        }
    }

    refreshAllModelsVars();

    if(_tableVariables)
        _tableVariables->resizeColumnsToContents();
}


void WidgetSelectEditVars::refreshAllModelsVars()
{
    _allModelsVars->clear();
    ModelPlus* curModelPlus;
    Variables* curVariables;
    for(int i=0;i<models().size();i++)
    {
        curModelPlus = _project->modelPlus(models().at(i));
        if(curModelPlus)
        {
            //            if(compileIfNeeded && !curModelPlus->isCompiled(_problem->ctrl(models().at(i))))
            //                curModelPlus->compile(_problem->ctrl(models().at(i)));
            curVariables = curModelPlus->variables();
            _allModelsVars->addItems(curVariables,true);
        }
    }

    _allModelsVars->addItems(_permanentVars,true);

}


void WidgetSelectEditVars::setShownColumns()
{
    // Hide columns
    QList<int> varsColsToShow;

    //varsColsToHide;
    varsColsToShow << Variable::NAME << Variable::DESCRIPTION;
    QList<int> colsHidden;

    if(_tableVariables)
    {
        for(int i=0;i<_tableVariables->horizontalHeader()->count();i++)
        {
            _tableVariables->setColumnHidden(i,!varsColsToShow.contains(i));
            if(!varsColsToShow.contains(i))
                colsHidden.push_back(i);
        }
        _hiddenFields.insert(_tableVariables,colsHidden);
    }

    //    QList<int> optColsToHide;
    //    optColsToHide << OptVariable::DESCRIPTION;
    //    for(int i=0;i<optColsToHide.size();i++)
    //        _tableOptimizedVars->setColumnHidden(optColsToHide.at(i),true);

    //    QList<int> scannedColsToHide;
    //    scannedColsToHide << ScannedVariable::DESCRIPTION;
    //    for(int i=0;i<scannedColsToHide.size();i++)
    //        _tableSamplingVars->setColumnHidden(scannedColsToHide.at(i),true);

    //    QList<int> objColsToHide;
    //    objColsToHide << OptObjective::DESCRIPTION;
    //    for(int i=0;i<objColsToHide.size();i++)
    //        _tableObjectives->setColumnHidden(objColsToHide.at(i),true);

}

void WidgetSelectEditVars::clearDelegates()
{
    for(int i=0;i<_delegates.size();i++)
        delete _delegates.at(i);

    _delegates.clear();
}


void WidgetSelectEditVars::setDelegates()
{
    clearDelegates();

    //tables' delegates
    QList<int> values;
    QStringList titles;
    values << OptObjective::MINIMIZE    << OptObjective::MAXIMIZE;
    titles << "Minimize"                << "Maximize";
    GenericDelegate *directionDelegate = new GenericDelegate(values,titles,this);

    if(_tableObjectives)
        _tableObjectives->setItemDelegateForColumn(iColumn(OptObjective::DIRECTION,_tableObjectives),directionDelegate);

    _delegates.push_back(directionDelegate);


    VarValueDelegate* varValueDelegate = new VarValueDelegate(_overVarsProxyModel,this);
    if(_tableOverVars)
        _tableOverVars->setItemDelegateForColumn(iColumn(Variable::VALUE,_tableOverVars),varValueDelegate);

    values.clear();
    titles.clear();
    values << OptObjective::NONE
           << OptObjective::SUM
           << OptObjective::AVERAGE
           << OptObjective::DEVIATION
           << OptObjective::MINIMUM
           << OptObjective::MAXIMUM ;

    titles << "None"
           << "Sum"
           << "Average"
           << "Standard deviation"
           << "Minimum"
           << "Maximum";

    GenericDelegate *scanFunctionDelegate = new GenericDelegate(values,titles,this);
    if(_tableObjectives)
        _tableObjectives->setItemDelegateForColumn(iColumn(OptObjective::SAMPLINGFUNCTION,_tableObjectives),scanFunctionDelegate);
    _delegates.push_back(scanFunctionDelegate);
    if(_tableOptimizedVars)
    {
        values.clear();
        titles.clear();
        values << OMREAL << OMINTEGER << OMBOOLEAN << OMSTRING ;
        titles << "Real" << "Integer" << "Boolean" << "String";
        GenericDelegate *dataTypeDelegate = new GenericDelegate(values,titles,this);
        _tableOptimizedVars->setItemDelegateForColumn(iColumn(OptVariable::DATATYPE,_tableOptimizedVars),dataTypeDelegate);
        _delegates.push_back(dataTypeDelegate);


        DoubleSpinBoxDelegate* minDelegate = new DoubleSpinBoxDelegate(this,30);
        DoubleSpinBoxDelegate* maxDelegate = new DoubleSpinBoxDelegate(this,30);
        _tableOptimizedVars->setItemDelegateForColumn(iColumn(OptVariable::OPTMIN,_tableOptimizedVars),minDelegate);
        _tableOptimizedVars->setItemDelegateForColumn(iColumn(OptVariable::OPTMAX,_tableOptimizedVars),maxDelegate);
        _delegates.push_back(minDelegate);
        _delegates.push_back(maxDelegate);
    }

    if(_tableObjectives)
    {
        DoubleSpinBoxDelegate* minObjDelegate = new DoubleSpinBoxDelegate(this,30);
        DoubleSpinBoxDelegate* maxObjDelegate = new DoubleSpinBoxDelegate(this,30);
        _tableObjectives->setItemDelegateForColumn(iColumn(OptObjective::MIN,_tableObjectives),minObjDelegate);
        _tableObjectives->setItemDelegateForColumn(iColumn(OptObjective::MAX,_tableObjectives),maxObjDelegate);
        _delegates.push_back(minObjDelegate);
        _delegates.push_back(maxObjDelegate);
    }
    if(_tableSamplingVars)
    {
        DoubleSpinBoxDelegate* minScanDelegate = new DoubleSpinBoxDelegate(this,30);
        DoubleSpinBoxDelegate* maxScanDelegate = new DoubleSpinBoxDelegate(this,30);
        DoubleSpinBoxDelegate* valueScanDelegate = new DoubleSpinBoxDelegate(this,30);
        DoubleSpinBoxDelegate* stepScanDelegate = new DoubleSpinBoxDelegate(this,30);
        _tableSamplingVars->setItemDelegateForColumn(iColumn(ScannedVariable::SCANMIN,_tableSamplingVars),minScanDelegate);
        _tableSamplingVars->setItemDelegateForColumn(iColumn(ScannedVariable::SCANMAX,_tableSamplingVars),maxScanDelegate);
        _tableSamplingVars->setItemDelegateForColumn(iColumn(ScannedVariable::VALUE,_tableSamplingVars),valueScanDelegate);
        _tableSamplingVars->setItemDelegateForColumn(iColumn(ScannedVariable::SCANSTEP,_tableSamplingVars),stepScanDelegate);
        _delegates.push_back(minScanDelegate);
        _delegates.push_back(maxScanDelegate);
        _delegates.push_back(valueScanDelegate);
        _delegates.push_back(stepScanDelegate);
    }

}

int WidgetSelectEditVars::iColumn(int iField,MOTableView* table) const
{
    QList<int> hiddenFields = _hiddenFields.value(table);

    if(hiddenFields.contains(iField))
        return -1;

    int iRow = 0;
    for(int i=0;i<iField;i++)
    {
        if(!hiddenFields.contains(i))
            iRow++;
    }
    return iRow;
}
