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

MOParametersDlg::MOParametersDlg(MOParameters *parameters)
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

                            }
                            break;
                    default :
                            valueWidget = new QLineEdit(this);
                            ((QLineEdit*)valueWidget)->setText(value.toString());
                            break;

                    }

                    boxLayout->addWidget(valueWidget,iRow,1);

                    // store (to save data when click ok)
                    _valueWidgets.push_back(valueWidget);
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
    QPushButton *pushDefault = new QPushButton("Restore default",this);
    QPushButton *pushCancel = new QPushButton("Cancel",this);
    QPushButton *pushOk = new QPushButton("Ok",this);

    buttonsLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum));
    buttonsLayout->addWidget(pushDefault);
    buttonsLayout->addWidget(pushCancel);
    buttonsLayout->addWidget(pushOk);

    connect(pushCancel,SIGNAL(clicked()),this,SLOT(pushedCancel()));
    connect(pushOk,SIGNAL(clicked()),this,SLOT(pushedOk()));
    connect(pushDefault,SIGNAL(clicked()),this,SLOT(pushedDefault()));

    pushOk->setDefault(true);

    QWidget *buttonsWidget = new QWidget(this);
    buttonsWidget->setLayout(buttonsLayout);

    mainLayout->addWidget(buttonsWidget);

    return mainLayout;
}





void MOParametersDlg::pushedOk()
{
        QVariant curValue;
        MOParameter* curParam;
        for(int i=0;i<_valueWidgets.size();i++)
        {
            curParam = _parameters->items.at(i);

            switch(curParam->getFieldValue(MOParameter::TYPE).toInt())
                {
                case MOParameter::STRING:
                case MOParameter::FILEPATH:
                case MOParameter::FOLDERPATH:
                        curValue = ((QLineEdit*)_valueWidgets.at(i))->text();
                        break;
                case MOParameter::DOUBLE:
                        curValue = ((QDoubleSpinBox*)_valueWidgets.at(i))->value();
                        break;
                case MOParameter::INT:
                        curValue = ((QSpinBox*)_valueWidgets.at(i))->value();
                        break;
                case MOParameter::BOOL:
                        curValue = ((QCheckBox*)_valueWidgets.at(i))->isChecked();
                        break;
                case MOParameter::LIST:
                        curValue = ((QComboBox*)_valueWidgets.at(i))->itemData(
                                ((QComboBox*)_valueWidgets.at(i))->currentIndex());
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
        MOParameter* curParam;
        for(int i=0;i<_valueWidgets.size();i++)
        {
                // get default value
                int j = _parameters->findItem(_paramNames.at(i));
                curParam = _parameters->items.at(j);
                defaultValue = curParam->getFieldValue(MOParameter::DEFAULTVALUE);

                switch(_paramTypes.at(i))
                {
                case MOParameter::STRING:
                case MOParameter::FILEPATH:
                case MOParameter::FOLDERPATH:
                        ((QLineEdit*)_valueWidgets.at(i))->setText(defaultValue.toString());
                        break;
                case MOParameter::DOUBLE:
                        ((QDoubleSpinBox*)_valueWidgets.at(i))->setValue(defaultValue.toDouble());
                        break;
                case MOParameter::INT:
                        ((QSpinBox*)_valueWidgets.at(i))->setValue(defaultValue.toInt());
                        break;
                case MOParameter::BOOL:
                        Qt::CheckState state;
                        if(defaultValue.toBool())
                                state = Qt::Checked;
                        else
                                state = Qt::Unchecked;
                        ((QCheckBox*)_valueWidgets.at(i))->setCheckState(state);
                        break;
                case MOParameter::LIST:
                        // set current index
                        ((QComboBox*)_valueWidgets.at(i))->setCurrentIndex(((QComboBox*)_valueWidgets.at(i))->findData(defaultValue));
                        break;
                }
        }
}


