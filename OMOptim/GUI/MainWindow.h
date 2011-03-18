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

	@file MainWindow.h
	@brief Comments for file documentation.
	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
	Company : CEP - ARMINES (France)
	http://www-cep.ensmp.fr/english/
	@version 0.9 
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QWidget>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtCore/QMutex>
#include "MOomc.h" 
#include "Project.h"
#include "tabProject.h"
#include "MOMainTab.h"
#include "ListInfo.h"
#include "MyTextLog.h"
#include "DlgSettings.h"
#include "ui_MainWindow.h"
#include "WidgetProgress.h"
#include <QtXml/QDomDocument>
#include "WidgetSelectModModel.h"
#include "AboutOMOptim.h"


namespace Ui
{
    class MainWindowClass;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Project*,QWidget *parent = 0);
    ~MainWindow();


public slots :
	void displayInfo(Info);
	void displayProgress(float);
	void displayProgress(float,int,int);

	void onStartedOMCThread(QString);
	void onFinishedOMCThread(QString);
	void onTerminatingOMCThread(QString);
	void enableProblemTab(QModelIndex);
	void enableSolvedProblemTab(QModelIndex);
	void onSelectedModClass(QModelIndex);


	void actualizeGuiFromProject();

	void newProject();
	void saveProject();
	void loadProject();
	void loadProject(QString);
	void newOneSimulation();
	void newOptimization();
	void newProblemEI();
	void newOMCShell();
	void OMCClear();
	void quit();
	void openSettings();

	void projectReset();
	void loadMoFile();
	void loadModelicaLibrary();

	void onAddedProblem(Problem*);
	void onAddedSolvedProblem(Problem*);
	void onComponentsUpdated();
	void onConnectionsUpdated();
	void onModifiersUpdated();
	void onProblemBegun(Problem*);
	void onProblemFinished(Problem*);
	void onNewProblemProgress(float);
	void onNewProblemProgress(float,int,int);
	void onProblemStopAsked();


	void removeSolvedProblem();
	void removeSolvedProblem(int);
	void removeSolvedProblemTab(int);

	void removeProblem();
	void removeProblem(int);
	void removeProblemTab(int);

	void renameProblem();
	void renameProblem(int);
	void renameSolvedProblem();
	void renameSolvedProblem(int);

	// Pop-up menus
	void showSolvedProblemPopup(const QPoint & iPoint);
	void showProblemPopup(const QPoint & iPoint);
	void showModClassTreePopup(const QPoint & iPoint);

	//Recent files
	void openRecentFile();
	void clearRecentFilesMenu();

	// GUI configuration
	void closeEvent(QCloseEvent *event);
	void readSettings();

	// about dialogs
	void dispAboutOMOptim();
	void dispAboutQt();

signals:
	void sendInfo(Info);

private:

	//Recent Files and folders
	void createRecentFilesMenu();
	void updateRecentFilesMenu();
	void updateRecentFilesList(QString);

	void setLastProjectFolder(QString folderName);
	void setLastDatabaseFolder(QString folderName);
	QString getLastProjectFolder();
	QString getLastDatabaseFolder();

	


private :
	Project *_project;

	//Gui
    Ui::MainWindowClass *_ui;
	MOMainTab *_tabMain;
	TabProject *_tabProject;
	WidgetProgress* _widgetProgress;
	enum { MaxRecentFiles = 5 };
    QAction *_recentFileActs[MaxRecentFiles];
	QAction *_separatorAct;
	MyTextLog *_textLog;


};

#endif // MAINWINDOW_H
