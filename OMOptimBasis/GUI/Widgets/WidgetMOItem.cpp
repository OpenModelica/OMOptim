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

  @file WidgetMOItem.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version
*/

#include "Widgets/WidgetMOItem.h"
#include "ui_WidgetMOItem.h"
#include "qsciencespinbox.h"

WidgetMOItem::WidgetMOItem(MOItem *_item,QWidget *parent,bool editable):
    QWidget(parent),
    ui(new Ui::WidgetMOItemClass)
{
    this->setLocale(QLocale::C);
    ui->setupUi(this);

    item = _item;
    _editable = editable;
    itemWidget = NULL;

    ui->groupItem->setLayout(new QGridLayout(this));
}

WidgetMOItem::~WidgetMOItem()
{
    delete ui;
}

void WidgetMOItem::clearGui()
{
    if(itemWidget)
    {
        ui->groupItem->layout()->removeWidget(itemWidget);
        delete itemWidget;
        fieldEditWidget.clear();
    }
    itemWidget = new QWidget(this);
    ui->groupItem->layout()->addWidget(itemWidget);

    QGridLayout* itemLayout = new QGridLayout(itemWidget);
    itemWidget->setLayout(itemLayout);


}

void WidgetMOItem::initializeGui()
{
    clearGui();
    QGridLayout* itemLayout = dynamic_cast<QGridLayout*>(itemWidget->layout());
    QLabel* label;
    if(item)
    {
        ui->groupItem->setTitle(item->name());
        QWidget* curWidget;

        for(int iF=0;iF<item->getNbFields();iF++)
        {
            curWidget = createEditWidget(item, iF, _editable);
            if(curWidget)
            {
                curWidget->setParent(this);
                fieldEditWidget.insert(iF,curWidget);


                // add it to gui
                label = new QLabel(item->getFieldName(iF),this);
                label->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
                itemLayout->addWidget(label,iF,0);
                itemLayout->addWidget(curWidget,iF,1);

                // connect change
                QComboBox* combo = dynamic_cast<QComboBox*>(curWidget);
                if(combo)
                    connect(combo,SIGNAL(thiseditTextChanged(const QString &)),
                            this,SLOT(onValueChanged(const QString &)));

                QSpinBox* spinBox = dynamic_cast<QSpinBox*>(curWidget);
                if(spinBox)
                    connect(spinBox,SIGNAL(valueChanged( const QString & )),
                            this,SLOT(onValueChanged(const QString &)));

                QScienceSpinBox* doubleSpinBox = dynamic_cast<QScienceSpinBox*>(curWidget);
                if(doubleSpinBox)
                    connect(doubleSpinBox,SIGNAL(valueChanged( const QString & )),
                            this,SLOT(onValueChanged(const QString &)));

                QCheckBox* checkBox = dynamic_cast<QCheckBox*>(curWidget);
                if(checkBox)
                    connect(checkBox,SIGNAL(stateChanged(int)),
                            this,SLOT(onCheckChanged(int)));

                QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(curWidget);
                if(lineEdit)
                    connect(lineEdit,SIGNAL(editingFinished()),
                            this,SLOT(onLineEditValueChanged()));
            }
        }
    }
}

void WidgetMOItem::setItem(MOItem* _item)
{
    item = _item;

    initializeGui();
    actualizeValues();
    emit valueChanged();

}

void WidgetMOItem::setEditable(bool editable)
{
    _editable = editable;
}
void WidgetMOItem::changeValue(QWidget* editor, const QVariant &value)
{
    int iF = fieldEditWidget.key(editor,-1);
    if(iF>-1)
    {
        item->setFieldValue(iF,value);
        updateWidgetValue(editor,item->getFieldValue(iF));
        emit valueChanged();
    }
}

void WidgetMOItem::onValueChanged(const QString & txt)
{
    QWidget *editor = qobject_cast<QWidget *>(sender());
    changeValue(editor,QVariant(txt));
}

void WidgetMOItem::onLineEditValueChanged()
{
    QLineEdit *editor = qobject_cast<QLineEdit*>(sender());
    changeValue(editor,QVariant(editor->text()));
}

void WidgetMOItem::onCheckChanged(int state)
{
    QWidget *editor = qobject_cast<QWidget *>(sender());
    switch(state)
    {
    case Qt::Unchecked:
    case Qt::PartiallyChecked    :
        changeValue(editor,QVariant(false));
        break;
    case Qt::Checked :
        changeValue(editor,QVariant(true));
        break;
    }
}

void WidgetMOItem::updateWidgetValue(QWidget* curWidget, QVariant value)
{
    QComboBox* combo = dynamic_cast<QComboBox*>(curWidget);
    if(combo)
        combo->setEditText(value.toString());

    QSpinBox* spinBox = dynamic_cast<QSpinBox*>(curWidget);
    if(spinBox)
        spinBox->setValue(value.toInt());

    QScienceSpinBox* doubleSpinBox = dynamic_cast<QScienceSpinBox*>(curWidget);
    if(doubleSpinBox)
        doubleSpinBox->setValue(value.toDouble());

    QCheckBox* checkBox = dynamic_cast<QCheckBox*>(curWidget);
    if(checkBox)
    {
        if(value.toBool())
            checkBox->setCheckState(Qt::Checked);
        else
            checkBox->setCheckState(Qt::Unchecked);
    }

    QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(curWidget);
    if(lineEdit)
        lineEdit->setText(value.toString());
}




void WidgetMOItem::actualizeValues()
{
    int iField;
    for(int i=0;i<fieldEditWidget.values().size();i++)
    {
        iField = fieldEditWidget.keys().at(i);

        if(item)
            updateWidgetValue(fieldEditWidget.values().at(i), item->getFieldValue(iField));
    }
}
