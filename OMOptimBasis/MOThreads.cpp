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

  @file MOThreads.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version

  */
#include "MOThreads.h"
#include <QApplication>
#include "ProjectBase.h"


namespace MOThreads
{

ProblemThread::ProblemThread(Problem* problem,ProblemConfig config)
{
    _problem = problem;
    _config = config;
    _result = NULL;
    _name = problem->name();

    connect(this,SIGNAL(finished()),this,SLOT(onFinished()));

    // propagate signals
    connect(_problem,SIGNAL(newProgress(float)),this,SIGNAL(newProgress(float)));
    connect(_problem,SIGNAL(newProgress(float,int,int)),this,SIGNAL(newProgress(float,int,int)));
}

void ProblemThread::run()
{
    QString error;
    bool ok = _problem->checkBeforeComp(error);
    if(!ok)
    {
  InfoSender::instance()->send(Info(error,ListInfo::WARNING2));
  _result = NULL;
    }
    else
    {
  emit begun(_problem);

  QString msg = "Launching problem : name = "+_problem->name()+" ; type = "+_problem->getClassName();
  InfoSender::instance()->send(Info(msg));
  _launchDate = QDateTime::currentDateTime();
  try
  {
      _result = _problem->launch(_config);
  }
  catch(const std::exception& e)
  {
      InfoSender::instance()->sendError("Error : " + QString(e.what()));
      _result = NULL;
  }
  catch(char * str)
  {
      InfoSender::instance()->sendError("Error : " + QString(str));
      _result = NULL;
  }
  catch(...)
  {
      InfoSender::instance()->sendError("Unknown Error ");
      _result = NULL;
  }


  if(_result)
  {
      // set result date and time
      _result->_date = _launchDate;
      // time spent (numberof days still not taken into account)
      int nSec = _launchDate.secsTo(QDateTime::currentDateTime());
      _result->_duration = QTime(0,0,0,0);
      _result->_duration = _result->_duration.addSecs(nSec);

      // important: result restored to main thread
      _result->moveToThread(QApplication::instance()->thread());
  }
    }
}

Result* ProblemThread::result()
{
    return _result;
}


void ProblemThread::publicExec()
{
    exec();
}
void ProblemThread::onFinished()
{
    emit finished(_problem,_result);
}
void ProblemThread::stop()
{
    if(_problem)
  _problem->stop();
//    terminate();
//    onFinished();
}



}
