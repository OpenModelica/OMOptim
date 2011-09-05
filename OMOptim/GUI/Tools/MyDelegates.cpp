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

#include "OptObjective.h"
#include <QtGui/QLineEdit>

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
//										 const QStyleOptionViewItem &/* option */,
//										 const QModelIndex &/* index */) const
//{
//	QComboBox *editor = new QComboBox(parent);
//	//editor->setAutoCompletion(true);
//	QStringList items;
//
//	items << "Point dependent" << "Point independent";
//	editor->addItems(items);
//
//	connect(editor, SIGNAL(activated(int)), this, SLOT(emitCommitData()));
//
//	return editor;
//}
//
//void FuzzyStatusDelegate::setEditorData(QWidget *editor,
//									  const QModelIndex &index) const
//{
//	QString status = index.model()->data(index, Qt::DisplayRole).toString();
//
//	QComboBox *comboBox = static_cast<QComboBox*>(editor);
//	comboBox->setCurrentIndex(comboBox->findText(status));
//}
//
//void FuzzyStatusDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
//									 const QModelIndex &index) const
//{
//	QComboBox *comboBox = static_cast<QComboBox*>(editor);
//
//	QString selectedText = comboBox->currentText();
//
//	if (selectedText=="Point dependent")
//	{
//		model->setData(index, FuzzyVariable::POINTDEP);
//		return;
//	}
//
//	if (selectedText=="Point independent")
//	{
//		model->setData(index, FuzzyVariable::POINTINDEP);
//		return;
//	}
//	return;
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
//										 const QStyleOptionViewItem &/* option */,
//										 const QModelIndex &/* index */) const
//{
//	QComboBox *editor = new QComboBox(parent);
//	//editor->setAutoCompletion(true);
//
//
//	editor->addItem("Fixed",FuzzyVariable::FIXED);
//	editor->addItem("Relative prec.",FuzzyVariable::PRECREL);
//	editor->addItem("Absolute prec.",FuzzyVariable::PRECABS);
//	
//	connect(editor, SIGNAL(activated(int)), this, SLOT(emitCommitData()));
//
//	return editor;
//}
//
//void FuzzyPrecStatusDelegate::setEditorData(QWidget *editor,
//									  const QModelIndex &index) const
//{
//	int precStatus = index.model()->data(index,Qt::UserRole).toInt();
//	QComboBox *comboBox = static_cast<QComboBox*>(editor);
//	comboBox->setCurrentIndex(comboBox->findData(precStatus));
//	
//}
//
//void FuzzyPrecStatusDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
//									 const QModelIndex &index) const
//{
//	QComboBox *comboBox = static_cast<QComboBox*>(editor);
//	model->setData(index,comboBox->itemData(comboBox->currentIndex()));
//
//	return;
//}
//
//void FuzzyPrecStatusDelegate::commitAndCloseEditor()
//{
//   QComboBox *editor = qobject_cast<QComboBox *>(sender());
//   emit commitData(editor);
//   emit closeEditor(editor);
//}


