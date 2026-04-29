/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
 * VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the OSMC (Open Source Modelica Consortium)
 * Public License (OSMC-PL) are obtained from OSMC, either from the above
 * address, from the URLs:
 * http://www.openmodelica.org or
 * https://github.com/OpenModelica/ or
 * http://www.ida.liu.se/projects/OpenModelica,
 * and in the OpenModelica distribution.
 *
 * GNU AGPL version 3 is obtained from:
 * https://www.gnu.org/licenses/licenses.html#GPL
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

/*
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)
 * @file MOThreads.cpp
 * @brief Comments for file documentation.
 * @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 * Company : CEP - ARMINES (France)
 * http://www-cep.ensmp.fr/english/
 * @version
 */


// $Id$

#include "MOThreads.h"
#include <QApplication>
#include "ProjectBase.h"


namespace MOThreads
{

ProblemThread::ProblemThread(ProjectBase* project,Problem* problem,ProblemConfig config)
    :QThread(project)
{
    _problem = problem;
    _config = config;
    _projectBase = project;
    _result = NULL;

    _name = problem->name();
    _isAlive = false;

    connect(this,SIGNAL(finished()),this,SLOT(onFinished()));

    // propagate signals
    connect(_problem,SIGNAL(newProgress(float)),this,SIGNAL(newProgress(float)));
    connect(_problem,SIGNAL(newProgress(float,int,int)),this,SIGNAL(newProgress(float,int,int)));
}

void ProblemThread::run()
{
    _isAlive = true;
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
    _isAlive = false;
}

Result* ProblemThread::result()
{
    return _result;
}

void ProblemThread::setIsAlive(bool isAlive)
{
    _isAlive = isAlive;
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
