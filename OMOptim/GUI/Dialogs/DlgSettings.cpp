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

 	@file DlgSettings.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 

*/

#include "DlgSettings.h"
#include "MOTableView.h"
#include "LowTools.h"
#include <QtGui/QGroupBox>
#include <QtGui/QCheckBox>
DlgSettings::DlgSettings(MOSettings _settings)
{
	adjustSize();
	settings = _settings;
	buildLayoutFromSettings();
	
}

QGridLayout* DlgSettings::buildLayoutFromSettings()
{
	//Adding Layout
	QGridLayout *mainLayout = new QGridLayout(this);


	// get groups
	QStringList uniqueGroups;
	QStringList groups;
	
	for(int i=0;i<settings.names.size();i++)
	{
		if(settings.names.at(i).contains("/"))
			groups.push_back(settings.names.at(i).section("/",0,0));
		else
			groups.push_back(QString());
	}
	uniqueGroups=groups;
	LowTools::removeDuplicates(uniqueGroups);

	
	// cerate group box
	for(int iG=0;iG<uniqueGroups.size();iG++)
	{
		int iRow=0;
		QGroupBox *box = new QGroupBox(uniqueGroups.at(iG),this);
		QGridLayout *boxLayout = new QGridLayout(box);
		
		int index0=0;
		int index1 = groups.indexOf(uniqueGroups.at(iG),index0);
		while(index1>-1)
		{
			// add setting
			QString dispName;
//			if(settings.names.at(index1).contains("/"))
//				dispName = settings.names.at(index1).section("/",1,-1);
//			else
//				dispName = settings.names.at(index1);
                        dispName = settings.descs.at(index1);
			boxLayout->addWidget(new QLabel(dispName),iRow,0);

			int type = settings.types.at(index1);
			QWidget *valueWidget;
			QVariant value = settings.getValue(index1);

			switch(type)
			{
			case MOSettings::STRING :
				valueWidget = new QLineEdit(this);
				((QLineEdit*)valueWidget)->setText(value.toString());
				break;
			case MOSettings::FILEPATH :
				valueWidget = new QLineEdit(this);
				((QLineEdit*)valueWidget)->setText(value.toString());
				// add button
				break;
			case MOSettings::FOLDERPATH :
				valueWidget = new QLineEdit(this);
				((QLineEdit*)valueWidget)->setText(value.toString());
				//add button
				break;
			case MOSettings::DOUBLE :
				valueWidget = new QDoubleSpinBox(this);
				((QDoubleSpinBox*)valueWidget)->setValue(value.toDouble());
				break;
			case MOSettings::INT :
				valueWidget = new QSpinBox(this);
				((QSpinBox*)valueWidget)->setMinimum(-1);
				((QSpinBox*)valueWidget)->setValue(value.toInt());
				break;
			case MOSettings::BOOL :
				valueWidget = new QCheckBox(this);
				Qt::CheckState state;
				if(value.toBool())
					state = Qt::Checked;
				else
					state = Qt::Unchecked;
				((QCheckBox*)valueWidget)->setCheckState(state);
				break;				
			default :
				valueWidget = new QLineEdit(this);
				((QLineEdit*)valueWidget)->setText(value.toString());
				break;

			}

			boxLayout->addWidget(valueWidget,iRow,1);
			
			// store (to save data when click ok)
			valueWidgets.push_back(valueWidget);
			settingsNames.push_back(settings.names.at(index1));
			settingsTypes.push_back(type);

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






void DlgSettings::pushedOk()
{
	QVariant curValue;

	for(int i=0;i<valueWidgets.size();i++)
	{
		switch(settingsTypes.at(i))
		{
		case MOSettings::STRING:
		case MOSettings::FILEPATH:
		case MOSettings::FOLDERPATH:
			curValue = ((QLineEdit*)valueWidgets.at(i))->text();
			break;
		case MOSettings::DOUBLE:
			curValue = ((QDoubleSpinBox*)valueWidgets.at(i))->value();
			break;
		case MOSettings::INT:
			curValue = ((QSpinBox*)valueWidgets.at(i))->value();
			break;
		case MOSettings::BOOL:
			curValue = ((QCheckBox*)valueWidgets.at(i))->isChecked();
			break;
		}

		settings.setValue(settingsNames.at(i),curValue);
	}
	accept();
}

void DlgSettings::pushedCancel()
{
	reject();
}

void DlgSettings::pushedDefault()
{

	QSettings globalSettings("MO", "Settings");
	QVariant defaultValue;

	for(int i=0;i<valueWidgets.size();i++)
	{
		// get default value
		int j = settings.names.indexOf(settingsNames.at(i));
		defaultValue = settings.defaultValues.at(j);

		switch(settingsTypes.at(i))
		{
		case MOSettings::STRING:
		case MOSettings::FILEPATH:
		case MOSettings::FOLDERPATH:
			((QLineEdit*)valueWidgets.at(i))->setText(defaultValue.toString());
			break;
		case MOSettings::DOUBLE:
			((QDoubleSpinBox*)valueWidgets.at(i))->setValue(defaultValue.toDouble());
			break;
		case MOSettings::INT:
			((QSpinBox*)valueWidgets.at(i))->setValue(defaultValue.toInt());
			break;
		case MOSettings::BOOL:
			Qt::CheckState state;
			if(defaultValue.toBool())
				state = Qt::Checked;
			else
				state = Qt::Unchecked;
			((QCheckBox*)valueWidgets.at(i))->setCheckState(state);
			break;				
		}
	}
}
