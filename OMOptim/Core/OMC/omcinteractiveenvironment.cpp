/*
* This file is part of OpenModelica.
*
* Copyright (c) 1998-2008, Linkopings University,
* Department of Computer and Information Science,
* SE-58183 Linkoping, Sweden.
*
* All rights reserved.
*
* THIS PROGRAM IS PROVIDED UNDER THE TERMS OF THIS OSMC PUBLIC
* LICENSE (OSMC-PL). ANY USE, REPRODUCTION OR DISTRIBUTION OF
* THIS PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE OF THE OSMC
* PUBLIC LICENSE.
*
* The OpenModelica software and the Open Source Modelica
* Consortium (OSMC) Public License (OSMC-PL) are obtained
* from Linkopings University, either from the above address,
* from the URL: http://www.ida.liu.se/projects/OpenModelica
* and in the OpenModelica distribution.
*
* This program is distributed  WITHOUT ANY WARRANTY; without
* even the implied warranty of  MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
* IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS
* OF OSMC-PL.
*
* See the full OSMC Public License conditions for more details.
*
* For more information about the Qt-library visit TrollTech's webpage
* regarding the Qt licence: http://www.trolltech.com/products/qt/licensing.html
*/

#include <exception>
#include <stdexcept>

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtGui/QMessageBox>
#endif

#include "omcinteractiveenvironment.h"
#include "Utilities.h"
#include <iostream>

namespace IAEX
{
  /*! \class OmcInteractiveEnvironment
  *
  * \brief Implements evaluation for modelica code.
  */
  OmcInteractiveEnvironment::OmcInteractiveEnvironment()
    : comm_(OmcCommunicator::getInstance()),result_("")
  {
    //Communicate with Omc.
    if(!comm_.isConnected())
    {
      if(!comm_.establishConnection())
      {
          throw std::runtime_error("OmcInteractiveEnvironment(): No connection to Omc established");
      }
    }
  }

  OmcInteractiveEnvironment::~OmcInteractiveEnvironment(){}

  QString OmcInteractiveEnvironment::result()
  {
    return result_;
  }

  void OmcInteractiveEnvironment::evalExpression(QString& expr)
  {
    // 2006-02-02 AF, Added try-catch
    try
    {
      result_ = comm_.callOmc(expr);
    }
    catch( std::exception &e )
    {
        QString msg = e.what();
        std::cerr << "Caught " << e.what( ) << std::endl;
        //throw e;
        if(msg.compare("NOT RESPONDING",Qt::CaseInsensitive)==0)
            result_ = "NOT RESPONDING";
    }
  }

  void OmcInteractiveEnvironment::closeConnection()
  {
    comm_.closeConnection();
  }

  void OmcInteractiveEnvironment::reconnect()
  {
    //Communicate with Omc.
    if(!comm_.isConnected())
    {
      if(!comm_.establishConnection())
      {
        throw std::runtime_error("OmcInteractiveEnvironment(): No connection to Omc established");
      }
    }
  }

  bool OmcInteractiveEnvironment::startDelegate()
  {
    // if not connected and can not establish connection,
    // try to start OMC
    if( !comm_.isConnected() && !comm_.establishConnection() )
    {
      return  OmcInteractiveEnvironment::startOMC();
    }
    else
      return false;
  }

  bool OmcInteractiveEnvironment::startOMC()
  {
    bool flag = false;

    try
    {
      // 2006-02-28 AF, use environment varable to find omc.exe
      std::string OMCPath( getenv( "OPENMODELICAHOME" ) );
      if( OMCPath.empty() )
      {
        throw std::runtime_error( "Could not find environment variable OPENMODELICAHOME" );
      }

      // location of omc in openmodelica folder
      QDir dir;
#ifdef WIN32
      if( dir.exists( QString(OMCPath.c_str()) + "\\bin\\omc.exe" ) )
        OMCPath += "\\bin\\";
      else if( dir.exists( QString(OMCPath.c_str()) + "\\omc.exe" ) )
        OMCPath;
      else if( dir.exists( "omc.exe" ))
        OMCPath = "";
      else
      {
        std::string msg = "Unable to find OMC, searched in:\n" +
          OMCPath + "\\bin\\\n" +
          OMCPath + "\n" +
          dir.absolutePath().toStdString();

        throw std::runtime_error( msg.c_str() );
      }
#else /* unix */
      if( dir.exists( QString(OMCPath.c_str()) + "/bin/omc" ) )
        OMCPath += "/bin/";
      else if( dir.exists( QString(OMCPath.c_str()) + "/omc" ) )
        OMCPath;
      else if( dir.exists( "omc.exe" ))
        OMCPath = "";
      else
      {
          std::string msg = "Unable to find OMC, searched in:\n" +
          OMCPath + "/bin/\n" +
          OMCPath + "\n" +
          dir.absolutePath().toStdString();

        throw std::runtime_error( msg.c_str() );
      }
#endif

      // 2006-03-14 AF, set omc loaction and parameters
      QString omc;
#ifdef WIN32
      omc = QString( OMCPath.c_str() ) + "omc.exe";
#else /* unix */
      omc = QString( OMCPath.c_str() ) + "omc";
#endif

      QStringList parameters;
      parameters << "+d=interactiveCorba" << QString("+corbaObjectReferenceFilePath=").append(Utilities::tempDirectory());

      // 2006-03-14 AF, create qt process
      QProcess *omcProcess = new QProcess();

      // 2006-03-14 AF, start omc
      omcProcess->start( omc, parameters );

      if( omcProcess->waitForStarted(7000) )
        flag = true;
      else
        flag = false;


    }
    catch( std::exception &e )
    {
      QString msg = e.what();
      QMessageBox::warning( QApplication::activeWindow(), "Error", msg, "OK" );
    }

    return flag;
  }
}
