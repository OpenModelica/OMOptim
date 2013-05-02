// $Id$
/**
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linkopings universitet, Department of Computer and Information Science,
 * SE-58183 Linkoping, Sweden.
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

  @file main.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version
*/

#include <QtGui/QApplication>
#include <QtCore/QSettings>
#include "MainWindow.h"
#include "OMOptimSettings.h"
#include "MOStyleSheet.h"
#include "version.h"
#include "registermetatypes.h"
#include "config.h"

#define HAVE_QAPPLICATION_H
#define HAVE_QSOCKETNOTIFIER_H


#include <omniORB4/CORBA.h>
CORBA::ORB_var orb;




int main(int argc, char *argv[])
{




    // Register Info as a metaType
    // Needed for Info communication between threads
    // register meta types for connect signals/slots
    RegisterMetaTypes::init();


    // Application
    QApplication *app = new QApplication(argc,argv);
    app->setApplicationName("OMOptim");


    // Setting the Application version
    //app->setApplicationVersion(APP_VERSION);
    QString version = QString::number(Version::MAJOR)+"."+QString::number(Version::MINOR)+"."+QString::number(Version::REVISION);
    app->setApplicationVersion(version);

    // Settings
    OMOptimSettings::initialize();

    // Style
    MOStyleSheet::initialize(qApp);

    // Project
    Project* project = new Project();

    // Message handler
    QString logFilePath = app->applicationDirPath()+QDir::separator()+"MOLog.txt";
    QFile logFile(logFilePath);
    logFile.open(QIODevice::WriteOnly);


    //#ifdef _DEBUG
    QTextStream logStream(&logFile);
    InfoSender::instance()->setLogStream(&logStream);
    //#endif


    // Starting
    MainWindow w(project);
    // w.setWindowTitle("CERES");
    app->connect( app, SIGNAL( lastWindowClosed() ), &w, SLOT( quit() ) );

    w.show();

//     load plugins
    QDir pluginsDir = QDir(QApplication::applicationDirPath()+QDir::separator()+"Plugins");
    if(pluginsDir.exists())
    {
  QStringList pluginsNames = pluginsDir.entryList();
  pluginsNames.removeAll(".");
  pluginsNames.removeAll("..");
  for(int i=0;i<pluginsNames.size();i++)
      project->loadPlugin(pluginsDir.filePath(pluginsNames.at(i)),false,true);
    }

    try
    {
  app->exec();
    }
    catch(std::exception &e)
    {
  QString msg(e.what());
  InfoSender::instance()->debug(msg);
  //#ifdef _DEBUG
  logStream.flush();
  logFile.close();
  //#endif
    }

    logFile.close();


    // delete project
    delete project;
    return 0;
}


