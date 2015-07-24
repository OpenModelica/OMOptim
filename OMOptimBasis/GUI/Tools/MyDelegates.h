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

     @file MyDelegates.h
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version
*/

#if !defined(_MYDELEGATES_H)
#define _MYDELEGATES_H

#include <limits>

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui/QStyledItemDelegate>
#include <QtGui/QCompleter>
#include <QtCore/QModelIndex>
#include <QtCore/QObject>
#include <QtCore/QSize>
#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QProgressBar>
#include <QSortFilterProxyModel>
#include <QtGui/QLineEdit>
#include <QCheckBox>
#endif

#include "qsciencespinbox.h"



class GenericDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    GenericDelegate(QList<int>,QStringList,QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;


private slots:
    void emitCommitData();

private:
    QList<int> values;
    QStringList titles;
};

class VarValueDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    VarValueDelegate(QSortFilterProxyModel* proxyModel = NULL, QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;
protected :
    QSortFilterProxyModel* _proxyModel;

private slots:
    void emitCommitData();
};

class VarCompleterDelegate : public QStyledItemDelegate
{
public:
    VarCompleterDelegate(QStringList _list,QObject* parent = 0) : QStyledItemDelegate(parent),list(_list) { }
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        /* QWidget* editor = QStyledItemDelegate::createEditor(parent, option, index);
        QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(editor);
        if (lineEdit)
        {
        QCompleter* completer = new QCompleter(list,lineEdit);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        lineEdit->setCompleter(completer);
        }
        return editor;*/

        QComboBox *comboBox = new QComboBox(parent);
        comboBox->setEditable(true);
        comboBox->setInsertPolicy(QComboBox::NoInsert);
        comboBox->addItems(list);

        connect(comboBox, SIGNAL(activated(int)), this, SLOT(emitCommitData()));
        connect(comboBox->lineEdit(), SIGNAL(editingFinished()), this, SLOT(emitCommitData()));

        QString currentText = index.model()->data(index, Qt::DisplayRole).toString();
        int selectedItem = comboBox->findText(currentText);
        if(selectedItem == -1)
            comboBox->setEditText(index.model()->data(index, Qt::DisplayRole).toString());
        else
            comboBox->setCurrentIndex(selectedItem);

        return comboBox;
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const
    {
        QComboBox *comboBox = qobject_cast<QComboBox*>(editor);

        comboBox->clear();
        comboBox->addItems(list);

        QString currentText = index.model()->data(index, Qt::DisplayRole).toString();
        int selectedItem = comboBox->findText(currentText);
        if(selectedItem == -1)
            comboBox->setEditText(index.model()->data(index, Qt::DisplayRole).toString());
        else
            comboBox->setCurrentIndex(selectedItem);
    }

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
    {
        QComboBox* comboBox = qobject_cast<QComboBox*>(editor);
        model->setData(index, comboBox->currentText(), Qt::EditRole);
    }

    void emitCommitData()
    {
        emit commitData(qobject_cast<QWidget *>(sender()));
    }
private:
    QStringList list;
};



class DoubleSpinBoxDelegate : public QStyledItemDelegate
{
public :
    DoubleSpinBoxDelegate(QObject *parent,int decimals, double min=-std::numeric_limits<double>::max(),
                          double max=std::numeric_limits<double>::max());

    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &/* option */,
                          const QModelIndex &/* index */) const
    ;

    double _min;
    double _max;
    int _decimals;
};



#endif
