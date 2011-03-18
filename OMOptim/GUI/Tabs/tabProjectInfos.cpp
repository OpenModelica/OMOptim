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

 	@file tabProjectInfos.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 
*/

#include "tabProjectInfos.h"



namespace Ui
{
	class TabProjectInfos_Class;
}

TabProjectInfos::TabProjectInfos(Project *project_,QWidget *parent) :
    QWidget(parent), ui(new Ui::TabProjectInfos_Class)
{
    ui->setupUi(this);

	project = project_;
	connect(ui->pushEdit, SIGNAL(clicked()), this, SLOT(onPushedEdit()));
}

TabProjectInfos::~TabProjectInfos()
{
    delete ui;
}

void TabProjectInfos::actualizeGuiFromProject()
{

	// File names
	if(project->isDefined())
	{
		ui->labelProjectName->setText(project->name());
		ui->labelProjectFile->setText(project->filePath());
		
		QString listMO;
		for(int i=0;i<project->moFiles().size();i++)
		{
			listMO.push_back(project->moFiles().at(i)+"\n");
		}
		ui->labelMoFiles->setText(listMO);
	}
	else
	{
		ui->labelProjectName->setText("-");
		ui->labelProjectFile->setText("-");
		ui->labelMoFiles->setText("-");
	}
}

void TabProjectInfos::onPushedEdit()
{
	bool openFileOk = LowTools::openFile(project->filePath());
	if(!openFileOk)
	{
		LowTools::openFolder(project->folder());
	}
}
