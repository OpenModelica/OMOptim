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

     @file MOTableView.cpp
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version

  */
#include "MOTableView.h"
#include <QDrag>
#include <QDragEnterEvent>
#include "InfoSender.h"
#include <QDebug>


MOTableView::MOTableView(QWidget* parent):QTableView(parent)
{

    //_view->installEventFilter(this);
    installEventFilter(this);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
#else
    horizontalHeader()->setResizeMode(QHeaderView::Interactive);
#endif
    verticalHeader()->hide();

    //editable ?
    setEditable(true);

    // context menu
    _actionCopy = new QAction("Copy", this);
    connect(_actionCopy,SIGNAL(triggered()),this, SLOT(onCopyAsked()));
    _actionCopy->setShortcut(QKeySequence::Copy);
    _actionCopy->setShortcutContext(Qt::WidgetShortcut);
    this->addAction(_actionCopy);
    _actionPaste = new QAction("Paste", this);
    connect(_actionPaste,SIGNAL(triggered()),this, SLOT(onPasteAsked()));
    _actionPaste->setShortcut(QKeySequence::Paste);
    _actionPaste->setShortcutContext(Qt::WidgetShortcut);
    this->addAction(_actionPaste);

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,SIGNAL(customContextMenuRequested(const QPoint &)),
            this,SLOT(contextualMenu(const QPoint &)));


}

MOTableView::~MOTableView()
{
}

void MOTableView::setEditable(bool editable)
{

    // drag and drop
    //    this->setDragEnabled(editable);
    //    this->setAcceptDrops(editable);
    //    this->setDropIndicatorShown(editable);
    //    this->viewport()->setAcceptDrops(editable);

    if(editable)
    {
        this->setDragDropMode(QAbstractItemView::DragDrop);
        this->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::AnyKeyPressed);

    }
    else
    {
        /// \todo find a way to avoid checking/unchecking
        this->setDragDropMode(QAbstractItemView::NoDragDrop);
        this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
}

void MOTableView::adjustViewSize()
{
    //        // Resize columns and rows.
    //        resizeRowsToContents();
    //        resizeColumnsToContents();

    //        // Complete width of view port.
    //        int totalWidth = viewport()->width();
    //        int availableWidth = totalWidth;

    //        // Iterate all columns and give them the required size.
    //        // The last column should receive the left size.
    //        for( int i = 0; i < horizontalHeader()->count(); i++ )
    //        {
    //                        int reqColumnSize = columnWidth(i);
    //                        availableWidth -= reqColumnSize;
    //        }

    //        if( availableWidth > 0 && availableWidth<totalWidth )
    //        {
    //                float ratio = (float)totalWidth/(float)(totalWidth-availableWidth);


    //                for( int i = 0; i < horizontalHeader()->count(); i++ )
    //                {
    //                        setColumnWidth(i,(int)((float)columnWidth(i)*ratio));
    //                }

    //        }

}

void MOTableView::setModel(QAbstractItemModel *_model)
{
    QTableView::setModel(_model);

    connect(_model, SIGNAL(layoutChanged()), this, SLOT(adjustViewSize()));

    adjustViewSize();
}


bool MOTableView::eventFilter( QObject *obj, QEvent *ev )
{
    if( obj == this )
    {
        if( ev->type() == QEvent::Resize )
        {
            adjustViewSize();
            return false;
        }
    }
    return false;
}

void MOTableView::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug() << "MOTableView::dragEnterEvent; begin";
    if (children().contains(event->source())|| (event->source()==this)) {
        InfoSender::instance()->debug("drag from same widget");
        event->setDropAction(Qt::MoveAction);
        event->accept();
    } else {
        InfoSender::instance()->debug("drag to another widget");
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
    qDebug() << "MOTableView::dragEnterEvent; end";
}


void MOTableView::startDrag(Qt::DropActions supportedActions)
{
    qDebug() << "MOTableView::startDrag; begin";
    QModelIndexList indexes = selectedIndexes();
    QList<QPersistentModelIndex> persistentIndexes;

    if (indexes.count() > 0) {
        QMimeData *data = model()->mimeData(indexes);
        if (!data)
            return;
        for (int i = 0; i<indexes.count(); i++){
            QModelIndex idx = indexes.at(i);
            persistentIndexes.append(QPersistentModelIndex(idx));
        }

        QPixmap pixmap = indexes.first().data(Qt::DecorationRole).value<QPixmap>();
        QDrag *drag = new QDrag(this);
        drag->setPixmap(pixmap);
        drag->setMimeData(data);
        drag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));

        Qt::DropAction defaultDropAction = Qt::IgnoreAction;
        if (supportedActions & Qt::MoveAction && dragDropMode() != QAbstractItemView::InternalMove)
            defaultDropAction = Qt::MoveAction; //was Qt::CopyAction THIS WAS THE CULPRIT!

        if ( drag->exec(supportedActions, defaultDropAction) == Qt::MoveAction ){
            //when we get here any copying done in dropMimeData has messed up our selected indexes
            //that's why we use persistent indexes
            for (int i = 0; i<indexes.count(); i++){
                QPersistentModelIndex idx = persistentIndexes.at(i);
                if (idx.isValid()){ //the item is not top level
                    model()->removeRow(idx.row(), idx.parent());
                }
                else{
                    model()->removeRow(idx.row(), QModelIndex());
                }
            }
        }
    }
    qDebug() << "MOTableView::startDrag; end";
}



void MOTableView::contextualMenu(const QPoint& point)
{
    QMenu *menu = new QMenu;
    menu->addAction(_actionCopy);
    menu->addAction(_actionPaste);
    menu->exec(this->mapToGlobal(point));
}


void MOTableView::onCopyAsked()
{
    QModelIndexList indexList = this->selectedIndexes();
    if(model())
    {
        QMimeData* mimeData = model()->mimeData(indexList);
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setMimeData(mimeData);
    }
}


void MOTableView::onPasteAsked()
{
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();


    QModelIndex index = this->currentIndex();

    model()->dropMimeData(mimeData,Qt::CopyAction,0,0,index);
}
