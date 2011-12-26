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

 	@file GuiTools.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 
*/

#include "Tools/GuiTools.h"


        GuiTools::GuiTools(void)
{
}

GuiTools::~GuiTools(void)
{
}

void GuiTools::ModelToView(QAbstractItemModel *model, QAbstractItemView *view)
{
    view->reset();
    view->setModel(model);

    QTableView* tableView = dynamic_cast<QTableView*>(view);
    if(tableView && model)
    {
        tableView->horizontalHeader()->setStretchLastSection(true);
        tableView->resizeColumnsToContents();
        tableView->resizeRowsToContents();

        // set maximum height
        int count = model->rowCount(QModelIndex());
        int h=0;
        for (int i = 0; i < count; i++)
            h += tableView->rowHeight(i);
        int maxH =
                (h +                                                      // total row height
                 count +                                                  // to account for the pixel(s) used in the grid
                 tableView->horizontalHeader()->height() +
                 tableView->horizontalScrollBar()->height());   // Need room for the horizontal scrollbar

        tableView->setMaximumHeight(maxH);
        tableView->resize(tableView->width(),maxH);

    }

    QTreeView* treeView = dynamic_cast<QTreeView*>(view);
    if(treeView && model)
    {
        for(int i=0;i<model->columnCount();i++)
            treeView->resizeColumnToContents(i);
        treeView->setSortingEnabled(true);
    }

    MyTreeView* myTreeView = dynamic_cast<MyTreeView*>(view);
    if(myTreeView && model)
    {
        connect(model,SIGNAL(dataChanged(const QModelIndex&,const QModelIndex&)),myTreeView,SLOT(onDataChanged(const QModelIndex&,const QModelIndex&)));
    }
}


ModClassTree* GuiTools::ModClassToTreeView(ModReader* modReader ,MOomc* moomc,const ModClass & modClass,QTreeView* treeView,bool showComponent)
{
    ModClassTree* newTree = new ModClassTree(modReader,moomc/*,treeView*/);
    newTree->setShowComponent(showComponent);
    ModClass* root = modClass.clone();
    root->clearDescendants(); // reset root : useful if showcomponents changed.
    newTree->addChild(newTree->rootElement(),root);

    treeView->reset();
    treeView->setModel(newTree);
    return newTree;
}


QSortFilterProxyModel * GuiTools::ModelToViewWithFilter(QAbstractItemModel *model, QAbstractItemView *view,QLineEdit* lineEdit)
{

    view->reset();
    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel((QObject*)lineEdit);
    proxyModel->setSourceModel(model);
    view->setModel(proxyModel);
    //view->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    QTableView* tableView = dynamic_cast<QTableView*>(view);
    if(tableView)
    {
        tableView->horizontalHeader()->setStretchLastSection(true);
        tableView->resizeColumnsToContents();
        tableView->resizeRowsToContents();
        tableView->setSortingEnabled(true);
        tableView->verticalHeader()->hide();
    }

    QTreeView* treeView = dynamic_cast<QTreeView*>(view);
    if(treeView)
    {
        for(int i=0;i<model->columnCount();i++)
            treeView->resizeColumnToContents(i);
        treeView->setSortingEnabled(true);
    }


    // views' filter
    proxyModel->setFilterKeyColumn(0);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    connect(lineEdit,SIGNAL(textChanged(const QString&)),
            proxyModel,SLOT(setFilterWildcard(const QString&)),Qt::AutoConnection);

    return proxyModel;

}




QMenu* GuiTools::createResultPopupMenu(Project* project, QWidget* mainWindow, const QPoint & iPoint,Result* selectedResult,int numResult)
{
    QMenu *menu = new QMenu();

    //Open folder
    QAction *openFolderAct = new QAction("Open folder",menu);
    connect(openFolderAct,SIGNAL(triggered()),selectedResult,SLOT(openFolder()));
    QIcon icon;
    icon.addPixmap(QPixmap(QString::fromUtf8(":/icons/folder")), QIcon::Normal, QIcon::Off);
    openFolderAct->setIcon(icon);
    //openFolderAct->setIconSize(QSize(20, 20));
    menu->addAction(openFolderAct);


    //Rename problem
    QAction *renameAct = new QAction("Rename result...",menu);
    renameAct->setData(numResult);
    connect(renameAct,SIGNAL(triggered()),mainWindow,SLOT(renameResult()));
    menu->addAction(renameAct);




    //Remove problem
    QAction *removeAct = new QAction("Remove result",menu);
    removeAct->setData(numResult);
    connect(removeAct,SIGNAL(triggered()),mainWindow,SLOT(removeResult()));
    QIcon iconRem;
    iconRem.addPixmap(QPixmap(QString::fromUtf8(":/icons/Remove")), QIcon::Normal, QIcon::Off);
    removeAct->setIcon(iconRem);
    menu->addAction(removeAct);

    return menu;
}

QMenu* GuiTools::createProblemPopupMenu(Project* project, QWidget* mainWindow, const QPoint & iPoint,Problem* selectedProblem,int numProblem)
{
    QMenu *menu = new QMenu();

    //Open folder
    //Open folder
    QAction *openFolderAct = new QAction("Open folder",menu);
    connect(openFolderAct,SIGNAL(triggered()),selectedProblem,SLOT(openFolder()));
    QIcon icon;
    icon.addPixmap(QPixmap(QString::fromUtf8(":/icons/folder")), QIcon::Normal, QIcon::Off);
    openFolderAct->setIcon(icon);
    //openFolderAct->setIconSize(QSize(20, 20));
    menu->addAction(openFolderAct);

    //Rename problem
    QAction *renameAct = new QAction("Rename problem...",menu);
    renameAct->setData(numProblem);
    connect(renameAct,SIGNAL(triggered()),mainWindow,SLOT(renameProblem()));
    menu->addAction(renameAct);

    //Remove problem
    QAction *removeAct = new QAction("Remove problem",menu);
    removeAct->setData(numProblem);
    connect(removeAct,SIGNAL(triggered()),mainWindow,SLOT(removeProblem()));
    QIcon iconRem;
    iconRem.addPixmap(QPixmap(QString::fromUtf8(":/icons/Remove")), QIcon::Normal, QIcon::Off);
    removeAct->setIcon(iconRem);
    menu->addAction(removeAct);

    return menu;
}


QMenu* GuiTools::newModClassPopupMenu(Project* project, const QPoint & iPoint,ModClass* selectedClass)
{
    QMenu *menu = new QMenu();
    project->setCurModClass(selectedClass);

    switch(selectedClass->getClassRestr())
    {
    case Modelica::MODEL :
        addModModelActions(menu,project,iPoint,(ModModel*)selectedClass);
    default :
            NULL;
}

    addCommonActions(menu,project,iPoint,selectedClass);

    return menu;
}

void GuiTools::addCommonActions(QMenu* menu,Project* project, const QPoint & iPoint,ModClass* selectedModClass)
{
    //Open folder
    QAction *openFolderAct = new QAction("Open folder",menu);
    connect(openFolderAct,SIGNAL(triggered()),selectedModClass,SLOT(openMoFolder()));
    QIcon icon;
    icon.addPixmap(QPixmap(QString::fromUtf8(":/icons/Folder")), QIcon::Normal, QIcon::Off);
    openFolderAct->setIcon(icon);
    menu->addAction(openFolderAct);

    // Reload mo file
    QAction *reload = new QAction("Reload .mo file",menu);
    connect(reload,SIGNAL(triggered()),selectedModClass,SLOT(reloadInOMC()));
    menu->addAction(reload);
}
void GuiTools::addModModelActions(QMenu* menu,Project* project, const QPoint & iPoint,ModModel* selectedModel)
{

    ModModelPlus* selectedModModelPlus = project->modModelPlus(selectedModel);

//    //Compile
//    QAction *compileModel = new QAction("Recompile model",menu);
//    connect(compileModel,SIGNAL(triggered()),selectedModModelPlus,SLOT(compile()));
//    menu->addAction(compileModel);


//    //Read variables
//    QAction *readVariables = new QAction("Read variables",menu);
//    connect(readVariables,SIGNAL(triggered()),selectedModModelPlus,SLOT(readVariables()));
//    menu->addAction(readVariables);

//    //Read connections
//    QAction *readConnections = new QAction("Read connections",menu);
//    connect(readConnections,SIGNAL(triggered()),selectedModModelPlus,SLOT(readConnections()));
//    menu->addAction(readConnections);

    //Add problem
    QMenu *addProblemMenu = menu->addMenu("Create problem");
    QAction *addOptimization = addProblemMenu->addAction("Optimization");
    connect(addOptimization,SIGNAL(triggered()),project,SLOT(addNewOptimization()));
    QAction *addOneSim = addProblemMenu->addAction("Simulation");
    connect(addOneSim,SIGNAL(triggered()),project,SLOT(addNewOneSimulation()));
    QAction *addEIProblem = addProblemMenu->addAction("EI");
    connect(addEIProblem,SIGNAL(triggered()),project,SLOT(addNewEIProblem()));


//    //Set parameters
//    QAction *setParameters = new QAction("Set parameters...",menu);
//    connect(setParameters,SIGNAL(triggered()),selectedModModelPlus,SLOT(openParametersDlg()));
//    menu->addAction(setParameters);

    // Set mo dependencies
    QAction *setMoDeps = new QAction("Set .mo dependencies ...",menu);
    connect(setMoDeps,SIGNAL(triggered()),selectedModModelPlus,SLOT(openDependenciesDlg()));
    menu->addAction(setMoDeps);

//    //Select simulator
//    QActionGroup *simulator = new QActionGroup(menu);
//    simulator->setExclusive(true);
//    menu->addSeparator()->setText(tr("Simulator"));

//    ModPlusCtrl* curCtrl;
//    ModPlusCtrl::Type curType;
//    QList<ModPlusCtrl*> ctrls = selectedModModelPlus->ctrls()->values();
//    for(int i=0;i<ctrls.size();i++)
//    {
//        curCtrl = ctrls.at(i);
//        curType = selectedModModelPlus->ctrls()->key(curCtrl);
//        QAction* simAction = simulator->addAction(curCtrl->name());
//        simAction->setCheckable(true);
//        simAction->setChecked(selectedModModelPlus->ctrlType()==curType);
//        simAction->setData((int)curType);
//        menu->addAction(simAction);
//        connect(simAction,SIGNAL(triggered()),selectedModModelPlus,SLOT(setCtrlType()));
//        simulator->addAction(simAction);
//    }
//    menu->addSeparator()->setText("");

}

void GuiTools::minimizeTableSize(QTableView* _table)
{
    // resizing window
    int colCount = _table->model()->columnCount();
    int rowCount = _table->model()->rowCount();

    _table->resizeColumnsToContents();
    _table->resizeRowsToContents();

    int w=0;
    for (int i = 0; i < colCount; i++)
        w += _table->columnWidth(i);

    int maxW =
            (w +                                                      // total column width
             colCount +                                                  // to account for the pixel(s) used in the grid
             _table->verticalHeader()->width() +
             _table->verticalScrollBar()->width());   // Need room for the vertical scrollbar


    int h=0;
    for (int i = 0; i < rowCount; i++)
        h += _table->rowHeight(i);

    int maxH =
            (h +                                                      // total column width
             rowCount +                                                  // to account for the pixel(s) used in the grid
             _table->horizontalHeader()->height() +
             _table->horizontalScrollBar()->height());   // Need room for the vertical scrollbar

    _table->resize(maxW+2,maxH+40);
}

void GuiTools::minimizeTableSize(QTableWidget* _table)
{
    // resizing window
    int colCount = _table->columnCount();
    int rowCount = _table->rowCount();

    _table->resizeColumnsToContents();
    _table->resizeRowsToContents();

    int w=0;
    for (int i = 0; i < colCount; i++)
        w += _table->columnWidth(i);

    int maxW =
            (w +                                                      // total column width
             colCount +                                                  // to account for the pixel(s) used in the grid
             _table->verticalHeader()->width() +
             _table->verticalScrollBar()->width());   // Need room for the vertical scrollbar


    int h=0;
    for (int i = 0; i < rowCount; i++)
        h += _table->rowHeight(i);

    int maxH =
            (h +                                                      // total column width
             rowCount +                                                  // to account for the pixel(s) used in the grid
             _table->horizontalHeader()->height() +
             _table->horizontalScrollBar()->height());   // Need room for the vertical scrollbar

    _table->resize(maxW+2,maxH+40);
}

void GuiTools::resizeTable(QTableView* table)
{
    // resizing window
    int count = table->model()->columnCount(QModelIndex());
    int w=0;
    for (int i = 0; i < count; i++)
        w += table->columnWidth(i);

    int maxW =
            (w +                                                      // total column width
             count +                                                  // to account for the pixel(s) used in the grid
             table->verticalHeader()->width() +
             table->verticalScrollBar()->width());   // Need room for the vertical scrollbar


    count = table->model()->rowCount(QModelIndex());
    int h=0;
    for (int i = 0; i < count; i++)
        h += table->rowHeight(i);

    int maxH =
            (h +                                                      // total column width
             count +                                                  // to account for the pixel(s) used in the grid
             table->horizontalHeader()->height() +
             table->horizontalScrollBar()->height());   // Need room for the vertical scrollbar

    table->resize(maxW+2,maxH+40);
}


void GuiTools::resizeTreeViewColumns(MyTreeView* treeView)
{

    //    // resizing window
    //    int count = treeView->model()->columnCount();

    //    QList<int> colsSize;
    //    int allColsSize=0;
    //    int curColSize;
    //    for (int i = 0; i < count; i++)
    //    {
    //        curColSize = treeView->getColHintSize(i);
    //        colsSize.push_back(curColSize);
    //        allColsSize += curColSize;
    //    }

    //    int contentWidth = treeView->contentsRect().width();
    //    double fact = 1;
    //    if(allColsSize<contentWidth)
    //        fact = (double)contentWidth/(double)allColsSize;

    //    for (int i = 0; i < count; i++)
    //    {
    //        treeView->setColumnWidth(i,colsSize.at(i)*fact);
    //    }

    int count = treeView->model()->columnCount();
    for (int i = 0; i < count; i++)
    {
        treeView->resizeColumnToContents(i);
    }

}

void GuiTools::resizeTableViewColumns(QTableView* tableView)
{

    // resizing window
    int nbCols = tableView->model()->columnCount();

    int colsWidth=0;
    for (int i = 0; i < nbCols; i++)
    {
        tableView->resizeColumnToContents(i);
        colsWidth += tableView->columnWidth(i);
    }

    int contentWidth = tableView->verticalHeader()->width();

    if(colsWidth<contentWidth)
    {
        double fact = (double)contentWidth/(double)colsWidth;
        int curColWidth;
        for (int i = 0; i < nbCols; i++)
        {
            curColWidth = tableView->columnWidth(i);
            tableView->setColumnWidth(i,curColWidth*fact);
        }
    }
}

QString GuiTools::launchBarStyleSheet()
{
    return "QFrame{background:qlineargradient(spread:repeat, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(65, 72, 89, 255), stop:0.466019 rgba(94, 105, 129, 255), stop:0.961165 rgba(65, 72, 89, 255));}";
}
