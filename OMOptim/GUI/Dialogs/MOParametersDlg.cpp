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

  @file MOParametersDlg.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version 0.9
*/

#include "MOParametersDlg.h"
#include "MOTableView.h"
#include <QtGui/QScrollBar>

MOParametersDlg::MOParametersDlg(MOParameters *parameters, bool editable)
{
    this->setWindowTitle("Parameters");
    //    if(parameters->items.size()==0)
    //    {
    //        // simulate an accept() (doesn't work in ctor)
    //        // no need to wait for a click on OK !
    //        setResult(QDialog::Accepted);
    //        QApplication::postEvent( this, new QCloseEvent() );
    //    }




    _parameters = parameters;
    _editable = editable;
    QGridLayout *newLayout = buildLayoutFromParameters();
    this->setLayout(newLayout);

}


QGridLayout* MOParametersDlg::buildLayoutFromParameters()
{
    //Adding Layout
    QGridLayout *mainLayout = new QGridLayout(this);

    // get groups
    QStringList uniqueGroups;
    QStringList groups;

    QStringList paramNames;
    for(int i=0;i<_parameters->items.size();i++)
        paramNames.push_back(_parameters->items.at(i)->name());

    for(int i=0;i<paramNames.size();i++)
    {
        if(paramNames.at(i).contains("/"))
            groups.push_back(paramNames.at(i).section("/",0,0));
        else
            groups.push_back(QString());
    }
    uniqueGroups=groups;
    LowTools::removeDuplicates(uniqueGroups);

    MOParameter* parameter;
    MOParameterListed *paramList;

    // create group box
    for(int iG=0;iG<uniqueGroups.size();iG++)
    {
        int iRow=0;
        QGroupBox *box = new QGroupBox(uniqueGroups.at(iG),this);
        QGridLayout *boxLayout = new QGridLayout(box);

        int index0=0;
        int index1 = groups.indexOf(uniqueGroups.at(iG),index0);
        while(index1>-1)
        {
            parameter = _parameters->items.at(index1);
            // add setting
            QString dispName;
            if(parameter->name().contains("/"))
                dispName = parameter->name().section("/",1,-1);
            else
                dispName = parameter->name();

            boxLayout->addWidget(new QLabel(dispName),iRow,0);

            int type = parameter->getFieldValue(MOParameter::TYPE).toInt();
            QWidget *valueWidget;
            QVariant value = parameter->getFieldValue(MOParameter::VALUE);

            switch(type)
            {
            case MOParameter::STRING :
                valueWidget = new QLineEdit(this);
                ((QLineEdit*)valueWidget)->setText(value.toString());
                break;
            case MOParameter::FILEPATH :
                valueWidget = new QLineEdit(this);
                ((QLineEdit*)valueWidget)->setText(value.toString());
                // add button
                break;
            case MOParameter::FOLDERPATH :
                valueWidget = new QLineEdit(this);
                ((QLineEdit*)valueWidget)->setText(value.toString());
                //add button
                break;

            case MOParameter::DOUBLE :
                valueWidget = new QDoubleSpinBox(this);
                ((QDoubleSpinBox*)valueWidget)->setMinimum(parameter->getFieldValue(MOParameter::MIN).toDouble());
                ((QDoubleSpinBox*)valueWidget)->setMaximum(parameter->getFieldValue(MOParameter::MAX).toDouble());
                ((QDoubleSpinBox*)valueWidget)->setDecimals(10);
                ((QDoubleSpinBox*)valueWidget)->setValue(value.toDouble());
                break;
            case MOParameter::INT :
                valueWidget = new QSpinBox(this);
                ((QSpinBox*)valueWidget)->setMinimum(parameter->getFieldValue(MOParameter::MIN).toInt());
                ((QSpinBox*)valueWidget)->setMaximum(parameter->getFieldValue(MOParameter::MAX).toInt());
                ((QSpinBox*)valueWidget)->setValue(value.toInt());
                break;
            case MOParameter::BOOL :
                valueWidget = new QCheckBox(this);
                Qt::CheckState state;
                if(value.toBool())
                    state = Qt::Checked;
                else
                    state = Qt::Unchecked;
                connect(((QCheckBox*)valueWidget),SIGNAL(stateChanged(int)),this,SLOT(onCheckedChanged()));
                ((QCheckBox*)valueWidget)->setCheckState(state);
                break;
            case MOParameter::LIST :
                //if is a list, param should be a MOParameterListed
                valueWidget = new QComboBox(this);
                paramList = dynamic_cast<MOParameterListed*>(parameter);
                if(paramList)
                {
                    //adding list items in qcombobox
                    for(int iValue = 0 ; iValue<paramList->mapList().keys().size();iValue++)
                    {
                        ((QComboBox*)valueWidget)->addItem(
                                    paramList->mapList().values().at(iValue),
                                    paramList->mapList().keys().at(iValue));
                    }
                    // set current index
                    ((QComboBox*)valueWidget)->setCurrentIndex(((QComboBox*)valueWidget)->findData(value));
                    connect(((QComboBox*)valueWidget),SIGNAL(currentIndexChanged(int)),this,SLOT(onCheckedChanged()));
                }
                break;
            default :
                valueWidget = new QLineEdit(this);
                ((QLineEdit*)valueWidget)->setText(value.toString());
                break;

            }

            boxLayout->addWidget(valueWidget,iRow,1);
            valueWidget->setEnabled(_editable);

            // store (to save data when click ok)
            _mapValueWidgets.insert(parameter->getFieldValue(MOParameter::INDEX).toInt(),valueWidget);
            _paramNames.push_back(parameter->name());
            _paramTypes.push_back(type);

            iRow++;
            index0=index1;
            index1 = groups.indexOf(uniqueGroups.at(iG),index0+1);
        }
        box->setLayout(boxLayout);

        mainLayout->addWidget(box);
    }


    QHBoxLayout *buttonsLayout = new QHBoxLayout(this);


    QPushButton *pushOk = new QPushButton("Ok",this);

    buttonsLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum));

    buttonsLayout->addWidget(pushOk);
    connect(pushOk,SIGNAL(clicked()),this,SLOT(pushedOk()));

    if(_editable)
    {
        QPushButton *pushDefault = new QPushButton("Restore default",this);
        connect(pushDefault,SIGNAL(clicked()),this,SLOT(pushedDefault()));
        QPushButton *pushCancel = new QPushButton("Cancel",this);
        connect(pushCancel,SIGNAL(clicked()),this,SLOT(pushedCancel()));

        buttonsLayout->addWidget(pushDefault);
        buttonsLayout->addWidget(pushCancel);
    }

    pushOk->setDefault(true);

    QWidget *buttonsWidget = new QWidget(this);
    buttonsWidget->setLayout(buttonsLayout);

    mainLayout->addWidget(buttonsWidget);

    if(_editable)
        updateAllEnabled(); //update

    return mainLayout;
}

void MOParametersDlg::onCheckedChanged()
{

    QWidget* widgetChanged = dynamic_cast<QWidget*>(sender());

    if(widgetChanged)
        updateEnabled(widgetChanged);
}

void MOParametersDlg::updateAllEnabled()
{

    QList<QWidget*> listWidget = _mapValueWidgets.values();

    for(int i=0;i<listWidget.size();i++)
        updateEnabled(listWidget.at(i));
}


void MOParametersDlg::updateEnabled(QWidget* widgetChanged)
{

    QVariant value;
    int index = _mapValueWidgets.key(widgetChanged,-1);

    if(index==-1)
        return;

    QCheckBox* checkWidget = dynamic_cast<QCheckBox*>(widgetChanged);
    if(checkWidget)
        value = checkWidget->isChecked();

    QComboBox* comboWidget = dynamic_cast<QComboBox*>(widgetChanged);
    if(comboWidget)
        value = comboWidget->itemData(comboWidget->currentIndex());

    MOParameter* curParam;
    QWidget* curWidget;
    for(int i=0;i<_parameters->items.size();i++)
    {
        curParam = _parameters->items.at(i);
        if(curParam->enablingIndex()==index)
        {
           curWidget = _mapValueWidgets.value(curParam->getFieldValue(MOParameter::INDEX).toInt(),NULL);

           if(curWidget)
           {
               curWidget->setEnabled(value==curParam->enablingValue());
           }
        }
    }
}



void MOParametersDlg::pushedOk()
{
    QVariant curValue;
    MOParameter* curParam;
    QWidget* curWidget;
    int curIndex;
    int iParam;



    for(int i=0;i<_mapValueWidgets.keys().size();i++)
    {
        curIndex = _mapValueWidgets.keys().at(i);
        iParam = _parameters->findItem(curIndex,MOParameter::INDEX);
        curParam = _parameters->items.at(iParam);
        curWidget = _mapValueWidgets.value(curIndex);

        switch(curParam->getFieldValue(MOParameter::TYPE).toInt())
        {
        case MOParameter::STRING:
        case MOParameter::FILEPATH:
        case MOParameter::FOLDERPATH:
            curValue = ((QLineEdit*)curWidget)->text();
            break;
        case MOParameter::DOUBLE:
            curValue = ((QDoubleSpinBox*)curWidget)->value();
            break;
        case MOParameter::INT:
            curValue = ((QSpinBox*)curWidget)->value();
            break;
        case MOParameter::BOOL:
            curValue = ((QCheckBox*)curWidget)->isChecked();
            break;
        case MOParameter::LIST:
            curValue = ((QComboBox*)curWidget)->itemData(
                        ((QComboBox*)curWidget)->currentIndex());
            break;
        }

        curParam->setFieldValue(MOParameter::VALUE,curValue);
    }
    accept();
}

void MOParametersDlg::pushedCancel()
{
    reject();
}

void MOParametersDlg::pushedDefault()
{

    QSettings globalSettings("MO", "Settings");
    QVariant defaultValue;
    QVariant curValue;
    MOParameter* curParam;
    QWidget* curWidget;
    int curIndex;
    int iParam;



    for(int i=0;i<_mapValueWidgets.keys().size();i++)
    {
        curIndex = _mapValueWidgets.keys().at(i);
        iParam = _parameters->findItem(curIndex,MOParameter::INDEX);
        curParam = _parameters->items.at(iParam);
        curWidget = _mapValueWidgets.value(curIndex);

        // get default value
        defaultValue = curParam->getFieldValue(MOParameter::DEFAULTVALUE);

        switch(_paramTypes.at(i))
        {
        case MOParameter::STRING:
        case MOParameter::FILEPATH:
        case MOParameter::FOLDERPATH:
            ((QLineEdit*)curWidget)->setText(defaultValue.toString());
            break;
        case MOParameter::DOUBLE:
            ((QDoubleSpinBox*)curWidget)->setValue(defaultValue.toDouble());
            break;
        case MOParameter::INT:
            ((QSpinBox*)curWidget)->setValue(defaultValue.toInt());
            break;
        case MOParameter::BOOL:
            Qt::CheckState state;
            if(defaultValue.toBool())
                state = Qt::Checked;
            else
                state = Qt::Unchecked;
            ((QCheckBox*)curWidget)->setCheckState(state);
            break;
        case MOParameter::LIST:
            // set current index
            ((QComboBox*)curWidget)->setCurrentIndex(((QComboBox*)curWidget)->findData(defaultValue));
            break;
        }
    }
}


