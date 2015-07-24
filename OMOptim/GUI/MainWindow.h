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
    @version
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui/QMainWindow>
#include <QtGui/QProgressBar>
#include <QtGui/QWidget>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtCore/QMutex>
#include <QtXml/QDomDocument>
#include <QtGui/QMessageBox>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QInputDialog>
#include <QtGui/QTabBar>
#include <QtGui/QDockWidget>
#include <QSettings>
#include <QFileSystemWatcher>
#include <QDebug>
#endif

#include "MOomc.h"
#include "Project.h"
#include "TabProject.h"
#include "Tabs/MOMainTab.h"
#include "ListInfo.h"
#include "MyTextLog.h"
#include "DlgSettings.h"
#include "ui_MainWindow.h"
#include "Widgets/WidgetProgress.h"
#include "Widgets/WidgetSelectModModel.h"
#include "AboutOMOptim.h"
#include "OpenModelica.h"
#include "OMCases.h"
#include "ProblemInterface.h"
#include "MyTreeView.h"


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
    void clearLog();
    void displayInfo(Info);
    void displayProgress(float);
    void displayProgress(float,int,int);

    void onStartedOMCThread(QString);
    void onFinishedOMCThread(QString);
    void onTerminatingOMCThread(QString);
    void enableOMCaseTab(QModelIndex);
    void onSelectedModItem(QModelIndex);
    void updateProblemsMenu();

    void connectProject();
    void actualizeGuiFromProject();

    void newProject();
    void saveProject();
    void loadProject();
    void launchScript();
    void dispScriptFunctions();
    void launchScriptText();
    void exportFolder();

    void loadProject(QString);
    void loadCases();
    void loadPlugins();
    void newOMCShell();
    void OMCClear();
    void quit();
    void openSettings();

    void onProjectAboutToBeReset();
    void loadMoFile();
    void loadModelicaLibrary();
    void loadExecutableModel();
    void refreshModelTree();
    void refreshModelTreeView();


    void onPushedNewProblem();
    void onAddedProblem(Problem*);
    void onAddedResult(Result*);
    void onProblemBegun(Problem*);
    void onProblemFinished(Problem*,Result*);
    void onNewProblemProgress(float);
    void onNewProblemProgress(float,int,int);

    void showModItem(ModItem*);
    void onMoFileChanged(const QString &);


    void removeResult();
    void removeResult(Result*);
    void removeResultTab(Result*);

    void removeProblem();
    void removeProblem(Problem*);
    void removeProblemTab(Problem*);

    void renameProblem();
    void renameProblem(Problem*);
    void renameResult();
    void renameResult(Result*);

    // Pop-up menus
    void rightClickedOnCase(const QPoint & iPoint);
    void showModItemsTreePopup(const QPoint & iPoint);

    //Recent files
    void openRecentFile();
    void clearRecentFilesMenu();

    // GUI configuration
    void closeEvent(QCloseEvent *event);
    void readSettings();

    // status and progress bar
    void showStatusBar();
    void hideStatusBar();
    void setStatusBarText(QString);
    void eraseStatusBarText();
    void increaseStProgressBar();


    // about dialogs
    void dispAboutOMOptim();
    void dispAboutQt();
    void openUserManual();

signals:
    void sendInfo(Info);

private:

    //Recent Files and folders
    void createRecentFilesMenu();
    void updateRecentFilesList(QString);

    void setLastProjectFolder(QString folderName);
    void setLastDatabaseFolder(QString folderName);
    QString getLastProjectFolder();
    QString getLastDatabaseFolder();




private :
    Project *_project;

    OMCasesCombiner *_casesTree;
    OMCasesTreeView *_casesTreeView;

    //Gui
    Ui::MainWindowClass *_ui;
    MOMainTab *_tabMain;
    TabProject *_tabProject;
    WidgetProgress* _widgetProgress;
    QStatusBar *_statusBar;
    QProgressBar *_stProgressBar;
    enum { MaxRecentFiles = 5 };
    QList<QAction*> _recentFileActs;

    MyTextLog *_textLog;


};

#endif // MAINWINDOW_H
