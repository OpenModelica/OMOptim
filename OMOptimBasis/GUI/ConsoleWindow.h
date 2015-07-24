// $Id: ConsoleWindow.h 16202 2013-06-04 07:28:54Z hubert.thieriot $
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

    @file ConsoleWindow.h
    @brief Comments for file documentation.
    @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
    Company : CEP - ARMINES (France)
    http://www-cep.ensmp.fr/english/
    @version
*/

#ifndef ConsoleWindow_H
#define ConsoleWindow_H

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
#include <QSortFilterProxyModel>
#include <QtGui/QMessageBox>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QInputDialog>
#include <QtGui/QTabBar>
#include <QtGui/QDockWidget>
#include <QSettings>
#include <QFileSystemWatcher>
#include <QDebug>
#endif

#include "ProjectBase.h"
#include "ListInfo.h"
#include "ui_ConsoleWindow.h"

namespace Ui
{
class ConsoleWindowClass;
}

class ConsoleWindow : public QMainWindow
{
    Q_OBJECT

public:
    ConsoleWindow(ProjectBase*,QStringList scriptCommands,QWidget *parent = 0);
    ~ConsoleWindow();



    void initLog();
    void actualizeGuiFromProject();
    int launch();

    int _result;
    void closeEvent(QCloseEvent *event);

private :
    ProjectBase *_project;
    QStringList _commands;


    //Gui
    Ui::ConsoleWindowClass *_ui;
    QSortFilterProxyModel* _logProxy;
};

#endif // ConsoleWindow_H
