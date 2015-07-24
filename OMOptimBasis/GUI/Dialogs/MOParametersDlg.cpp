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
  @version
*/

#include "MOParametersDlg.h"
#include "MOTableView.h"
#include "qsciencespinbox.h"
#include "LowTools.h"

void WidgetParameters::onSelectFileClicked()
{
    QPushButton* button = dynamic_cast<QPushButton*>(sender());

    QLineEdit* line = _pathsMap.value(button,NULL);

    if(line)
    {
        QString filename = QFileDialog::getOpenFileName(this);
        if(!filename.isEmpty())
            line->setText(filename);
    }
}

void WidgetParameters::onSelectFolderClicked()
{
    QPushButton* button = dynamic_cast<QPushButton*>(sender());

    QLineEdit* line = _pathsMap.value(button,NULL);

    if(line)
    {
        QString filename = QFileDialog::getExistingDirectory(this);
        if(!filename.isEmpty())
            line->setText(filename);
    }
}

void WidgetParameters::setDefaultValues()
{
    QVariant defaultValue;
    MOParameter* curParam;
    QWidget* curWidget;


    for(int i=0;i<_mapValueWidgets.keys().size();i++)
    {
        //        curIndex = _mapValueWidgets.keys().at(i);
        //        iParam = _localParameters->findItem(curIndex,MOParameter::INDEX);
        curParam = _mapValueWidgets.keys().at(i);
        curWidget = _mapValueWidgets.value(curParam,NULL);

        // get default value
        defaultValue = curParam->getFieldValue(MOParameter::DEFAULTVALUE);

        setValue(curWidget,defaultValue);
    }

}



/**
  * useDirectLink: if true, widget works directly on parameters. Otherwise,
  * it works on a copy.
  *
  */
WidgetParameters::WidgetParameters(MOParameters *parameters,bool useDirectLink, bool editable)
{
    this->setLocale(QLocale::C);
    this->setWindowTitle("Parameters");
    //    if(parameters->size()==0)
    //    {
    //        // simulate an accept() (doesn't work in ctor)
    //        // no need to wait for a click on OK !
    //        setResult(QDialog::Accepted);
    //        QApplication::postEvent( this, new QCloseEvent() );
    //    }

    _useDirectLink = useDirectLink;
    if(_useDirectLink)
        _localParameters = parameters;
    else
        _localParameters = parameters->clone();

    _editable = editable;

    QGridLayout *newLayout = buildLayoutFromParameters();
    this->setLayout(newLayout);

}

WidgetParameters::~WidgetParameters()
{
    if(!_useDirectLink)
        delete _localParameters;
}


QGridLayout* WidgetParameters::buildLayoutFromParameters()
{
    //Adding Layout
    QGridLayout *mainLayout = new QGridLayout(this);

    // get groups
    QMultiMap<QString,MOParameter*> groupmap = _localParameters->groupmap();
    QStringList groups = groupmap.uniqueKeys();


    QStringList paramNames;
    for(int i=0;i<_localParameters->size();i++)
        paramNames.push_back(_localParameters->at(i)->name());

    QPushButton *newPush;
    MOParameter* parameter;
    MOParameterListed *paramList;
    QList<MOParameter*> groupParameters;
    QGridLayout *curLayout;
    QGroupBox *curBox;

    // create group box
    for(int iG=0;iG<groups.size();iG++)
    {
        int iRow=0;
        if(groups.size()>1)
        {
            curBox = new QGroupBox(groups.at(iG),this);
            curLayout = new QGridLayout(curBox);
        }
        else
            curLayout = mainLayout;

        groupParameters = groupmap.values(groups.at(iG));

        // to reproduce parameters order, start from the end
        // it seems MultiMap behaves like a pile
        for(int iP=groupParameters.size()-1;iP>=0;iP--)
        {
            parameter = groupParameters.at(iP);
            // add setting
            QString dispName;
            if(parameter->name().contains("/"))
                dispName = parameter->name().section("/",1,-1);
            else
                dispName = parameter->name();

            curLayout->addWidget(new QLabel(parameter->description()),iRow,0);
            //boxLayout->addWidget(new QLabel(dispName),iRow,0);

            int type = parameter->getFieldValue(MOParameter::TYPE).toInt();
            QWidget *valueWidget;
            QVariant value = parameter->getFieldValue(MOParameter::VALUE);

            switch(type)
            {
            case MOParameter::STRING :
                valueWidget = new QLineEdit(this);
                ((QLineEdit*)valueWidget)->setText(value.toString());
                connect(((QLineEdit*)valueWidget),SIGNAL(textChanged(QString)),this,SLOT(onValueChanged()));
                break;
            case MOParameter::FILEPATH :
                valueWidget = new QLineEdit(this);
                ((QLineEdit*)valueWidget)->setText(value.toString());
                connect(((QLineEdit*)valueWidget),SIGNAL(textChanged(QString)),this,SLOT(onValueChanged()));
                // add button
                newPush = new QPushButton("...",this);
                newPush->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Preferred);
                _pathsMap.insert(newPush,((QLineEdit*)valueWidget));
                curLayout->addWidget(newPush,iRow,2);
                connect(newPush,SIGNAL(clicked()),this,SLOT(onSelectFileClicked()));
                break;
            case MOParameter::FOLDERPATH :
                valueWidget = new QLineEdit(this);
                ((QLineEdit*)valueWidget)->setText(value.toString());
                connect(((QLineEdit*)valueWidget),SIGNAL(textChanged(QString)),this,SLOT(onValueChanged()));
                //add button
                newPush = new QPushButton("...",this);
                newPush->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Preferred);
                _pathsMap.insert(newPush,((QLineEdit*)valueWidget));
                curLayout->addWidget(newPush,iRow,2);
                connect(newPush,SIGNAL(clicked()),this,SLOT(onSelectFolderClicked()));
                break;

            case MOParameter::DOUBLE :
                valueWidget = new QScienceSpinBox(this);
                ((QScienceSpinBox*)valueWidget)->setMinimum(parameter->getFieldValue(MOParameter::MIN).toDouble());
                ((QScienceSpinBox*)valueWidget)->setMaximum(parameter->getFieldValue(MOParameter::MAX).toDouble());
                ((QScienceSpinBox*)valueWidget)->setDecimals(10);
                ((QScienceSpinBox*)valueWidget)->setValue(value.toDouble());
                connect(((QScienceSpinBox*)valueWidget),SIGNAL(valueChanged(double)),this,SLOT(onValueChanged()));
                break;
            case MOParameter::INT :
                valueWidget = new QSpinBox(this);
                ((QSpinBox*)valueWidget)->setMinimum(parameter->getFieldValue(MOParameter::MIN).toInt());
                ((QSpinBox*)valueWidget)->setMaximum(parameter->getFieldValue(MOParameter::MAX).toInt());
                ((QSpinBox*)valueWidget)->setValue(value.toInt());
                connect(((QSpinBox*)valueWidget),SIGNAL(valueChanged(int)),this,SLOT(onValueChanged()));
                break;
            case MOParameter::BOOL :
                valueWidget = new QCheckBox(this);
                Qt::CheckState state;
                if(value.toBool())
                    state = Qt::Checked;
                else
                    state = Qt::Unchecked;
                connect(((QCheckBox*)valueWidget),SIGNAL(stateChanged(int)),this,SLOT(onValueChanged()));
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
                    connect(((QComboBox*)valueWidget),SIGNAL(currentIndexChanged(int)),this,SLOT(onValueChanged()));
                }
                break;
            default :
                valueWidget = new QLineEdit(this);
                ((QLineEdit*)valueWidget)->setText(value.toString());
                connect(((QLineEdit*)valueWidget),SIGNAL(textChanged(QString)),this,SLOT(onValueChanged()));
                break;

            }

            curLayout->addWidget(valueWidget,iRow,1);
            valueWidget->setEnabled(_editable);

            // store (to save data when click ok)
            _mapValueWidgets.insert(parameter,valueWidget);
            _paramNames.push_back(parameter->name());
            _paramTypes.push_back(type);

            iRow++;
        }

        if(groups.size()>1)
        {
            curBox->setLayout(curLayout);
            mainLayout->addWidget(curBox);
        }
    }




    if(_editable)
        updateEnabled(); //update

    return mainLayout;
}

void WidgetParameters::onValueChanged()
{

    QWidget* widgetChanged = dynamic_cast<QWidget*>(sender());

    // update value
    MOParameter* param = _mapValueWidgets.key(widgetChanged,NULL);
    if(param)
        param->setFieldValue(MOParameter::VALUE,getValue(widgetChanged));

    // update enabled widgets
    if(widgetChanged)
        updateEnabled();
}



void WidgetParameters::updateEnabled()
{
    MOParameter* curParam;
    QWidget* curWidget;
    if(_editable)
    {
        for(int i=0;i<_localParameters->size();i++)
        {
            curParam = _localParameters->at(i);
            curWidget = _mapValueWidgets.value(curParam,NULL);

            if(curWidget)
            {
                curWidget->setEnabled(_localParameters->shouldBeEnabled(curParam->name()));
            }
        }
    }
}

QVariant WidgetParameters::getValue(QWidget* curWidget)
{

    QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(curWidget);
    if(lineEdit)
        return lineEdit->text();

    QScienceSpinBox* dblspinbox = dynamic_cast<QScienceSpinBox*>(curWidget);
    if(dblspinbox)
        return dblspinbox->value();

    QSpinBox* spinbox = dynamic_cast<QSpinBox*>(curWidget);
    if(spinbox)
        return spinbox->value();

    QCheckBox* checkbox = dynamic_cast<QCheckBox*>(curWidget);
    if(checkbox)
        return checkbox->isChecked();

    QComboBox* combo = dynamic_cast<QComboBox*>(curWidget);
    if(combo)
        return combo->itemData(combo->currentIndex());
}

void WidgetParameters::setValue(QWidget* curWidget,QVariant value)
{

    QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(curWidget);
    if(lineEdit)
        lineEdit->setText(value.toString());

    QScienceSpinBox* dblspinbox = dynamic_cast<QScienceSpinBox*>(curWidget);
    if(dblspinbox)
        dblspinbox->setValue(value.toDouble());

    QSpinBox* spinbox = dynamic_cast<QSpinBox*>(curWidget);
    if(spinbox)
        spinbox->setValue(value.toInt());

    QCheckBox* checkbox = dynamic_cast<QCheckBox*>(curWidget);
    if(checkbox)
        checkbox->setChecked(value.toBool());

    QComboBox* combo = dynamic_cast<QComboBox*>(curWidget);
    if(combo)
    {
        combo->setCurrentIndex(combo->findData(value));
    }

}


void MOParametersDlg::pushedOk()
{
    _orgParameters->cloneFromOtherVector(_widget->localParameters());
    accept();
}

void MOParametersDlg::pushedCancel()
{
    reject();
}

void MOParametersDlg::pushedDefault()
{
    _widget->setDefaultValues();
}

MOParametersDlg::MOParametersDlg(MOParameters *parameters, bool editable)
{
    // save parameters
    _orgParameters = parameters;

    // create layout
    this->setLayout(new QVBoxLayout(this));

    // add parameters widget
    // no direct link : widget works on a copy. Only when clicked on Ok,
    // parameters values are updated.
    _widget = new WidgetParameters(parameters,false,editable);
    this->layout()->addWidget(_widget);

    // add buttons
    QHBoxLayout *buttonsLayout = new QHBoxLayout(this);
    QPushButton *pushOk = new QPushButton("Ok",this);
    buttonsLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum));
    buttonsLayout->addWidget(pushOk);
    connect(pushOk,SIGNAL(clicked()),this,SLOT(pushedOk()));

    if(editable)
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

    this->layout()->addWidget(buttonsWidget);
}

MOParametersDlg::~MOParametersDlg()
{
    delete _widget;
}
