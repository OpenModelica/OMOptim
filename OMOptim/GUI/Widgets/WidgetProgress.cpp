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
 * Main contributor 2011, Hubert Thierot, CEP - ARMINES (France)

  @file WidgetProgress.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version
*/

#include "Widgets/WidgetProgress.h"
#include "ui_WidgetProgress.h"

WidgetProgress::WidgetProgress(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetProgressClass)
{
    ui->setupUi(this);

    connect(ui->pushStop, SIGNAL(clicked()),this,SLOT(onPushedStop()));
    connect(ui->pushQuickEnd,SIGNAL(clicked()),this,SLOT(onPushedQuickEnd()));


    _fraction = 0;
    _taskName = "";
    _timer = new QTimer(this);
    connect( _timer, SIGNAL(timeout()), this, SLOT(updateTimeLeft()) );

    _nbSpeeds = 20;
    _period = 2000;
}

WidgetProgress::~WidgetProgress()
{
    delete ui;
}

void WidgetProgress::setProgress(float fraction)
{

    float oldFraction = _fraction;
    _fraction = fraction;
    ui->progressBar->setValue((int)(_fraction*100));

    // hide if 0%
    if(fraction==0)
        ui->progressBar->hide();
    else
        ui->progressBar->show();

    //speed
    int elapsed = _lastTime.elapsed();
    double newSpeed = (_fraction-oldFraction)/(double)elapsed;
    if(_lastSpeeds.size()==_nbSpeeds)
        _lastSpeeds.removeFirst();
    _lastSpeeds.push_back(newSpeed);

    // restart time counter
    _lastTime.restart();

}

void WidgetProgress::setProgress(float fraction,int curEval,int totalEval)
{
    float oldFraction = _fraction;
    _fraction = fraction;
    ui->progressBar->setValue((int)(_fraction*100));

    ui->labelTask->setText(_taskName + "(eval "+QString::number(curEval)+"/"+QString::number(totalEval)+")");

    //speed
    int elapsed = _lastTime.elapsed();
    double newSpeed = (_fraction-oldFraction)/(double)elapsed;
    if(_lastSpeeds.size()==_nbSpeeds)
        _lastSpeeds.removeFirst();
    _lastSpeeds.push_back(newSpeed);

    // restart time counter
    _lastTime.restart();
}

void WidgetProgress::newTask(QString taskName)
{
    _taskName = taskName;
    ui->labelTask->setText(_taskName);
    setProgress(0);

    _lastSpeeds.clear();

    _timer->start(_period);
}


void WidgetProgress::endTask()
{
    _taskName = "";
    ui->labelTask->setText(_taskName);
    setProgress(0);

    _timer->stop();
}

void WidgetProgress::updateTimeLeft()
{


    double averageSpeed=0;

    if(_lastSpeeds.size()>0)
    {
        ui->labelTimeLeft->show();
        for(int i=0;i<_lastSpeeds.size();i++)
            averageSpeed += _lastSpeeds.at(i);
        averageSpeed = averageSpeed / _lastSpeeds.size();

        int timeLeft = (1-_fraction)/averageSpeed;

        QTime dispTime(0,0,0);
        dispTime = dispTime.addMSecs(timeLeft);


        ui->labelTimeLeft->setText("- "+dispTime.toString());
    }
    else
        ui->labelTimeLeft->hide();
}

void WidgetProgress::onPushedStop()
{
    emit askProblemStop(_problem);
}

void WidgetProgress::onPushedQuickEnd()
{
    Optimization* optim = dynamic_cast<Optimization*>(_problem);

    if(optim)
    {
        optim->quickEnd();
    }
}

void WidgetProgress::setCurProblem(Problem* problem)
{
    _problem = problem;

    if(!_problem || !_problem->canBeStoped())
        ui->pushStop->hide();
    else
        ui->pushStop->show();

    if(!_problem || !_problem->hasQuickEndOption())
        ui->pushQuickEnd->hide();
    else
        ui->pushQuickEnd->show();

}



