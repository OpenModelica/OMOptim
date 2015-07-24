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

     @file MyDelegates.cpp
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version
*/

#include "MyDelegates.h"
#include "Variable.h"

GenericDelegate::GenericDelegate(QList<int> _values ,QStringList _titles,QObject *parent)
    : QStyledItemDelegate(parent)
{
    values = _values;
    titles = _titles;
}

QWidget *GenericDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem &/* option */,
                                       const QModelIndex &/* index */) const
{
    QComboBox *editor = new QComboBox(parent);
    //editor->setAutoCompletion(true);

    editor->addItems(titles);

    connect(editor, SIGNAL(activated(int)), this, SLOT(emitCommitData()));

    return editor;
}

void GenericDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    QString curTitle = index.model()->data(index, Qt::DisplayRole).toString();

    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    comboBox->setCurrentIndex(comboBox->findText(curTitle));
}

void GenericDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);

    QString selectedText = comboBox->currentText();
    int iValue = titles.indexOf(selectedText);

    if(iValue>-1)
    {
        int curValue = values.at(iValue);
        model->setData(index,QVariant(curValue));
    }

    return;
}

void GenericDelegate::emitCommitData()
{
    emit commitData(qobject_cast<QWidget *>(sender()));
}


//***********************************
// Fuzzy status
//***********************************
//
//FuzzyStatusDelegate::FuzzyStatusDelegate(QObject *parent)
//: QStyledItemDelegate(parent)
//{
//}
//
//QWidget *FuzzyStatusDelegate::createEditor(QWidget *parent,
//                                         const QStyleOptionViewItem &/* option */,
//                                         const QModelIndex &/* index */) const
//{
//    QComboBox *editor = new QComboBox(parent);
//    //editor->setAutoCompletion(true);
//    QStringList items;
//
//    items << "Point dependent" << "Point independent";
//    editor->addItems(items);
//
//    connect(editor, SIGNAL(activated(int)), this, SLOT(emitCommitData()));
//
//    return editor;
//}
//
//void FuzzyStatusDelegate::setEditorData(QWidget *editor,
//                                      const QModelIndex &index) const
//{
//    QString status = index.model()->data(index, Qt::DisplayRole).toString();
//
//    QComboBox *comboBox = static_cast<QComboBox*>(editor);
//    comboBox->setCurrentIndex(comboBox->findText(status));
//}
//
//void FuzzyStatusDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
//                                     const QModelIndex &index) const
//{
//    QComboBox *comboBox = static_cast<QComboBox*>(editor);
//
//    QString selectedText = comboBox->currentText();
//
//    if (selectedText=="Point dependent")
//    {
//        model->setData(index, FuzzyVariable::POINTDEP);
//        return;
//    }
//
//    if (selectedText=="Point independent")
//    {
//        model->setData(index, FuzzyVariable::POINTINDEP);
//        return;
//    }
//    return;
//}
//
//void FuzzyStatusDelegate::emitCommitData()
// {
//     emit commitData(qobject_cast<QWidget *>(sender()));
// }
//
//
////***********************************
//// Fuzzy prec status
////***********************************
//FuzzyPrecStatusDelegate::FuzzyPrecStatusDelegate(QObject *parent)
//: QStyledItemDelegate(parent)
//{
//}
//
//QWidget *FuzzyPrecStatusDelegate::createEditor(QWidget *parent,
//                                         const QStyleOptionViewItem &/* option */,
//                                         const QModelIndex &/* index */) const
//{
//    QComboBox *editor = new QComboBox(parent);
//    //editor->setAutoCompletion(true);
//
//
//    editor->addItem("Fixed",FuzzyVariable::FIXED);
//    editor->addItem("Relative prec.",FuzzyVariable::PRECREL);
//    editor->addItem("Absolute prec.",FuzzyVariable::PRECABS);
//
//    connect(editor, SIGNAL(activated(int)), this, SLOT(emitCommitData()));
//
//    return editor;
//}
//
//void FuzzyPrecStatusDelegate::setEditorData(QWidget *editor,
//                                      const QModelIndex &index) const
//{
//    int precStatus = index.model()->data(index,Qt::UserRole).toInt();
//    QComboBox *comboBox = static_cast<QComboBox*>(editor);
//    comboBox->setCurrentIndex(comboBox->findData(precStatus));
//
//}
//
//void FuzzyPrecStatusDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
//                                     const QModelIndex &index) const
//{
//    QComboBox *comboBox = static_cast<QComboBox*>(editor);
//    model->setData(index,comboBox->itemData(comboBox->currentIndex()));
//
//    return;
//}
//
//void FuzzyPrecStatusDelegate::commitAndCloseEditor()
//{
//   QComboBox *editor = qobject_cast<QComboBox *>(sender());
//   emit commitData(editor);
//   emit closeEditor(editor);
//}

DoubleSpinBoxDelegate::DoubleSpinBoxDelegate(QObject *parent,int decimals, double min, double max)
    : QStyledItemDelegate(parent),_min(min),_max(max),_decimals(decimals)
{

}

QWidget *DoubleSpinBoxDelegate::createEditor(QWidget *parent,
                                             const QStyleOptionViewItem &/* option */,
                                             const QModelIndex &/* index */) const
{
    QScienceSpinBox *editor = new QScienceSpinBox(parent);

    editor->setMinimum(_min);
    editor->setMaximum(_max);
    editor->setDecimals(_decimals);

    return editor;
}

QWidget *VarValueDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(!index.isValid())
        return QStyledItemDelegate::createEditor(parent,option,index);

    QModelIndex realIndex = index;
    if(_proxyModel)
        realIndex = _proxyModel->mapToSource(index);

    Variable* var = static_cast<Variable*>(realIndex.internalPointer());
    VariableType type = var->getDataType();

    QWidget* result;
    QComboBox* combo;
    QSpinBox* spinBox;
    QScienceSpinBox *doubleSpinBox;
     int comboIndex;
     bool varValue;
    switch(type)
    {
    case OMBOOLEAN :
        combo = new QComboBox(parent);
        combo->addItem("true",true);
        combo->addItem("false",false);
        varValue = var->value().toBool();
        comboIndex = combo->findData(varValue);
        combo->setCurrentIndex(comboIndex);
        connect(combo, SIGNAL(activated(int)), this, SLOT(emitCommitData()));
        return combo;
        break;
//    case OMINTEGER :
//        spinBox = new QSpinBox(parent);
//        spinBox->setMinimum(-std::numeric_limits<int>::max()/2);
//        spinBox->setMaximum(std::numeric_limits<int>::max()/2);
//        connect(spinBox,SIGNAL(valueChanged(int)),this,SLOT(emitCommitData()));
//        return spinBox;
//        break;
//    case OMREAL :
//        doubleSpinBox = new QScienceSpinBox(parent);
//        doubleSpinBox->setMinimum(-std::numeric_limits<double>::max()/2);
//        doubleSpinBox->setMaximum(std::numeric_limits<double>::max()/2);
//        connect(doubleSpinBox,SIGNAL(valueChanged(int)),this,SLOT(emitCommitData()));
//        return doubleSpinBox;
//        break;
    default:
        return QStyledItemDelegate::createEditor(parent,option,index);
    }
}

void VarValueDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = dynamic_cast<QComboBox*>(editor);
//    QModelIndex realIndex = index;
//    if(_proxyModel)
//        realIndex = _proxyModel->mapToSource(index);
    if(comboBox)
    {
        QVariant value = comboBox->itemData(comboBox->currentIndex());
        model->setData(index,QVariant(value));
    }
    else
        QStyledItemDelegate::setModelData(editor,model,index);
}

void VarValueDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    bool curValue = index.model()->data(index, Qt::DisplayRole).toBool();

    QComboBox *comboBox = dynamic_cast<QComboBox*>(editor);
    if(comboBox)
        comboBox->setCurrentIndex(comboBox->findData(curValue));
    else
        QStyledItemDelegate::setEditorData(editor,index);
}

void VarValueDelegate::emitCommitData()
{
    emit commitData(qobject_cast<QWidget *>(sender()));
}

VarValueDelegate::VarValueDelegate(QSortFilterProxyModel *proxyModel, QObject *parent)
{
    _proxyModel = proxyModel;
}

