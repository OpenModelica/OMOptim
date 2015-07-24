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
#include "Problem.h"
#include "MyTreeView.h"
#include "ProjectBase.h"
#include "Result.h"
#include "Variable.h"


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

        //        // set maximum height
        //        int count = model->rowCount(QModelIndex());
        //        int h=0;
        //        for (int i = 0; i < count; i++)
        //            h += tableView->rowHeight(i);
        //        int maxH =
        //                (h +                                                      // total row height
        //                 count +                                                  // to account for the pixel(s) used in the grid
        //                 tableView->horizontalHeader()->height() +
        //                 tableView->horizontalScrollBar()->height());   // Need room for the horizontal scrollbar

        //        tableView->setMaximumHeight(maxH);
        //        tableView->resize(tableView->width(),maxH);

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

QFrame* GuiTools::buildLine(QWidget* parent)
{
    QFrame*line = new QFrame(parent);
    line->setObjectName(QString::fromUtf8("line"));
    line->setGeometry(QRect(320, 150, 118, 3));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    return line;
}

QSortFilterProxyModel * GuiTools::ModelToViewWithFilter(QAbstractItemModel *model, QAbstractItemView *view,QLineEdit* lineEdit,QSortFilterProxyModel *proxyModel)
{
    view->reset();
    if(!proxyModel)
        proxyModel = new QSortFilterProxyModel((QObject*)lineEdit);

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



QMenu* GuiTools::createResultPopupMenu(ProjectBase* project, QWidget* mainWindow, const QPoint & iPoint,Result* selectedResult,int numResult)
{
    QMenu *menu = new QMenu();

    //Open folder
    QAction *openFolderAct = new QAction("Open folder",menu);
    openFolderAct->setData(selectedResult->name());
    connect(openFolderAct,SIGNAL(triggered()),project,SLOT(openOMCaseFolder()));
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

QMenu* GuiTools::createProblemPopupMenu(ProjectBase* project, QWidget* mainWindow, const QPoint & iPoint,Problem* selectedProblem,int numProblem)
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

VariableSortFilter::VariableSortFilter(QObject *parent)
    :QSortFilterProxyModel(parent)
{
    _causalities << OMUNKNOWN << OMINPUT << OMOUTPUT;
}

VariableSortFilter::VariableSortFilter(QList<VariableCausality> causalities, QObject *parent)
    :QSortFilterProxyModel(parent)
{
    _causalities = causalities;
}

bool VariableSortFilter::filterAcceptsRow(int sourceRow, const
                                          QModelIndex &sourceParent) const
{
    QModelIndex index = this->sourceModel()->index(sourceRow, 0, sourceParent);
    MOItem *item = static_cast<MOItem*>(index.internalPointer());
    Variable* variable = dynamic_cast<Variable*>(item);

    if(variable)
    {
        if(!_causalities.contains(variable->causality()))
            return false;
    }

    return QSortFilterProxyModel::filterAcceptsRow(sourceRow,sourceParent);
}

qint64 TextEditIoDevice::writeData(const char *data, qint64 maxSize)
{
    if(textBrowser)
    {
        QString msg(data);
        msg.remove(QRegExp("\\n$"));
        if(!msg.isEmpty())
            textBrowser->append(msg);
    }
    return maxSize;
}

