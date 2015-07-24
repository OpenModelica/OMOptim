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

  @file GuiTools.h
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version
*/

#ifndef GUITOOLS_H
#define GUITOOLS_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui/QMessageBox>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QTableView>
#include <QtGui/QTableWidget>
#include <QtGui/QListView>
#include <QtGui/QMenu>
#include <QtGui/QLineEdit>
#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QtGui/QTabWidget>
#include <QtGui/QLayout>
#include <QDesktopServices>
#include <QUrl>
#include <QtGui/QHeaderView>
#include <QtGui/QScrollBar>
#include <QtGui/QTextBrowser>
#include <QtGui/QTextEdit>
#include <QEvent>
#include <QKeyEvent>
#include <QSortFilterProxyModel>
#include <QGroupBox>
#include <QPointer>
#include <QFrame>
#endif

#include "VariableType.h"

class ProjectBase;
class Result;
class Problem;
class MyTreeView;

class GuiTools: public QObject
{
    Q_OBJECT

public:



    static QSortFilterProxyModel * ModelToViewWithFilter(QAbstractItemModel *, QAbstractItemView *,QLineEdit*,
                                                         QSortFilterProxyModel *proxyModel = NULL);
    static void ModelToView(QAbstractItemModel *, QAbstractItemView *);



    static QMenu* createResultPopupMenu(ProjectBase*,QWidget*,const QPoint &,Result*,int numResult);
    static QMenu* createProblemPopupMenu(ProjectBase*,QWidget*,const QPoint &,Problem*,int numProblem);


    static void minimizeTableSize(QTableView*);
    static void minimizeTableSize(QTableWidget*);

    static void resizeTable(QTableView*);

    static void resizeTreeViewColumns(MyTreeView*);
    static void resizeTableViewColumns(QTableView*);




    static QFrame *buildLine(QWidget *parent);
};

class VariableSortFilter : public QSortFilterProxyModel
{
public :
    VariableSortFilter(QObject * parent = 0);
    VariableSortFilter(QList<VariableCausality> causalities,QObject * parent = 0);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    QList<VariableCausality> _causalities;
};

class KeyPressFilter : public QObject
{
    Q_OBJECT

    KeyPressFilter(QObject* parent):QObject(parent){};

protected:
    bool eventFilter( QObject *o, QEvent *e )
    {
        if ( e->type() == QEvent::KeyPress ) {
            // special processing for key press
            QKeyEvent *k = (QKeyEvent *)e;

            switch(k->key())
            {
            case Qt::Key_Delete:
                emit deletePressed();
            }

        }
        return false;
    }

signals :
    void deletePressed();
};

class MyGroupBox : public QGroupBox
{
    Q_OBJECT
public :
    MyGroupBox(QWidget* parent=0)
        :QGroupBox(parent)
    {

    }

    MyGroupBox(const QString &title, QWidget* parent=0)
        :QGroupBox(title,parent)
    {

    }

public Q_SLOTS :

    void setChecked( bool checked )
    {
        QList<QWidget *> ch = findChildren<QWidget *>();
        for( int i = 0; i < ch.count(); i++ ) ch[i]->setVisible( checked );
        QGroupBox::setChecked( checked );
        setFlat( ! checked );
    }
};

class TextEditIoDevice : public QIODevice
{
    Q_OBJECT

public:
    TextEditIoDevice(QTextBrowser *const textBrowser, QObject *const parent)
        : QIODevice(parent)
        , textBrowser(textBrowser)
    {
        open(QIODevice::WriteOnly|QIODevice::Text);
    }

    //...

protected:
    qint64 readData(char *data, qint64 maxSize) { return 0; }
    qint64 writeData(const char *data, qint64 maxSize);

private:
    QPointer<QTextBrowser> textBrowser;
};


#endif
