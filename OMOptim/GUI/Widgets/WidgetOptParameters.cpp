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

 	@file WidgetOptParameters.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 
*/

#include "WidgetOptParameters.h"
#include "ui_WidgetOptParameters.h"
#include <QtGui/QErrorMessage>


WidgetOptParameters::WidgetOptParameters(Project* project,Optimization* problem,QWidget *parent):
    QWidget(parent),
    _ui(new Ui::WidgetOptParametersClass)
{
    _ui->setupUi(this);
	
	_project = project;
	_problem = problem;


	connect(_ui->pushLaunch, SIGNAL(clicked()), this, SLOT(launch()));
	connect(_ui->pushEAParameters, SIGNAL(clicked()), this, SLOT(openAlgoParameters()));
	connect(_ui->pushRestore,SIGNAL(clicked()), this, SLOT(restoreProblem()));
	connect(_ui->pushPursue, SIGNAL(clicked()), this, SLOT(pursueMoo()));
	connect(_ui->pushSelectStartFile,SIGNAL(clicked()),this,SLOT(selectStartFile()));
	connect(_ui->checkUseStartFile,SIGNAL(stateChanged(int)),this,SLOT(onChangedCheckUseSartFile()));

	//changed algorithm
	connect(_ui->comboAlgo, SIGNAL(currentIndexChanged(int)),this, SLOT(changedAlgorithm()));
	connect(_problem,SIGNAL(algoConfigsChanged()),this,SLOT(onAlgosConfigChanged()));
	connect(_ui->lineStartFile,SIGNAL( textChanged ( const QString &)),this,SLOT(onTextStartFileChanged( const QString &)));

	//Algorithm box
	QStringList _algoNames = _problem->getAlgoNames();
	int iCurAlgo = _problem->getiCurAlgo();
	_ui->comboAlgo->addItems(_algoNames);
	if(iCurAlgo>=0)
		_ui->comboAlgo->setCurrentIndex(iCurAlgo);

	// actualize gui
	onAlgosConfigChanged();


	
}

WidgetOptParameters::~WidgetOptParameters()
{
    delete _ui;
}


void WidgetOptParameters::launch()
{

	QString error;
	bool ok = _problem->checkBeforeComp(error);
	if(!ok)
                QMessageBox::warning(this, "Error",error,QMessageBox::Ok,QMessageBox::Ok);
	else
		_project->launchProblem(_problem->num());
}

void WidgetOptParameters::changedAlgorithm()
{
	int iAlgo = _ui->comboAlgo->currentIndex();
	_problem->setiCurAlgo(iAlgo);
}

void WidgetOptParameters::onChangedCheckUseSartFile()
{
	int iEA = _ui->comboAlgo->currentIndex();
	
	bool _useStartFile = _ui->checkUseStartFile->isChecked();
	((EAConfig*)_problem->getCurAlgoConfig())->setUseStartFile(_useStartFile);
}

void WidgetOptParameters::openAlgoParameters()
{
	if(_problem->getCurAlgo())
	{
		EAConfig* _EAConfig = (EAConfig*)_problem->getCurAlgo()->_config;
		EAConfigDialog *eaConfigDlg = new EAConfigDialog(_EAConfig);
		eaConfigDlg->exec();
	}
}



void WidgetOptParameters::restoreProblem()
{
	int iSolved = _project->solvedProblems()->findItem(_problem->name());

	if(iSolved>-1)
		_project->restoreProblemFromSolvedOne(iSolved);
}

void WidgetOptParameters::pursueMoo()
{

	QString filename = QFileDialog::getOpenFileName(
		this,
		"MO - Choose start file",
		_problem->saveFolder(),
		"Iterations files (iteration.*);;All files (*.*)" );

	if(!filename.isEmpty())
	{
		
		int iSolved = _project->solvedProblems()->findItem(_problem->name());
		if(iSolved>-1)
		{
			Optimization* newPb = (Optimization*)_project->restoreProblemFromSolvedOne(iSolved);
			((EAConfig*)newPb->getCurAlgoConfig())->setReloadFilePath(filename);
			((EAConfig*)newPb->getCurAlgoConfig())->setUseStartFile(true);
		}
	}
}

void WidgetOptParameters::selectStartFile()
{
	QString openedPath = _project->solvedProblemsFolder();
	QString filename = QFileDialog::getOpenFileName(
		this,
		"MO - Choose start file",
		openedPath,
		"Archive files (iteration*.sav);;All files (*.*)" );

	if(!filename.isEmpty())
		_ui->lineStartFile->setText(filename);

}

void WidgetOptParameters::onTextStartFileChanged(const QString &newText)
{
	int iEA = _ui->comboAlgo->currentIndex();
	
	QString _newText = newText;
	((EAConfig*)_problem->getCurAlgoConfig())->setReloadFilePath(_newText);
}



void WidgetOptParameters::onAlgosConfigChanged()
{
	_ui->lineStartFile->setText(((EAConfig*)_problem->getCurAlgoConfig())->getReloadFilePath());
	_ui->checkUseStartFile->setChecked(((EAConfig*)_problem->getCurAlgoConfig())->getUseSartFile());
}




void WidgetOptParameters::actualizeGui()
{
	
	// list of widgets to hide when problem is solved
	QWidgetList unsolvedWidgets;
	unsolvedWidgets << _ui->pushLaunch << _ui->pushSelectStartFile;

	// list of widgets to hide when problem is unsolved
	QWidgetList solvedWidgets;
	solvedWidgets << _ui->pushRestore  << _ui->pushPursue ;

	// if problem is solved
	if(_problem->isSolved())
	{
		for(int i=0; i < unsolvedWidgets.size(); i++)
			unsolvedWidgets.at(i)->hide();
	
		for(int i=0; i < solvedWidgets.size(); i++)
			solvedWidgets.at(i)->show();
	
		// combo algo
		_ui->comboAlgo->setCurrentIndex(_problem->getiCurAlgo());
		_ui->comboAlgo->setEnabled(false);

		// start file
		_ui->checkUseStartFile->setEnabled(false);
		_ui->lineStartFile->setEnabled(false);
		
	}
	else
	{
		for(int i=0; i < unsolvedWidgets.size(); i++)
			unsolvedWidgets.at(i)->show();
	
		for(int i=0; i < solvedWidgets.size(); i++)
			solvedWidgets.at(i)->hide();
	
		// combo algo
		_ui->comboAlgo->setCurrentIndex(_problem->getiCurAlgo());
		_ui->comboAlgo->setEnabled(true);

		// start file
		_ui->checkUseStartFile->setEnabled(true);
		_ui->lineStartFile->setEnabled(true);	
	}
}
