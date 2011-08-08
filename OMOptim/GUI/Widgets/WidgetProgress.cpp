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

  @file WidgetProgress.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version 0.9
*/

#include "WidgetProgress.h"
#include "ui_WidgetProgress.h"
#include <QtGui/QErrorMessage>



WidgetProgress::WidgetProgress(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetProgressClass)
{
    ui->setupUi(this);

    connect(ui->pushStop, SIGNAL(clicked()),this,SIGNAL(pushedStop()));


    fraction =0;
    taskName = "";
    timer = new QTimer(this);
    connect( timer, SIGNAL(timeout()), this, SLOT(updateTimeLeft()) );
    
    nbSpeeds = 20;
    period = 2000;
}

WidgetProgress::~WidgetProgress()
{
    delete ui;
}

void WidgetProgress::setProgress(float _fraction)
{

    float oldFraction = fraction;
    fraction = _fraction;
    ui->progressBar->setValue((int)(fraction*100));

    // hide if 0%
    if(_fraction==0)
        ui->progressBar->hide();
    else
        ui->progressBar->show();

    //speed
    int elapsed = lastTime.elapsed();
    double newSpeed = (fraction-oldFraction)/(double)elapsed;
    if(lastSpeeds.size()==nbSpeeds)
        lastSpeeds.removeFirst();
    lastSpeeds.push_back(newSpeed);

    // restart time counter
    lastTime.restart();

}

void WidgetProgress::setProgress(float _fraction,int curEval,int totalEval)
{
    float oldFraction = fraction;
    fraction = _fraction;
    ui->progressBar->setValue((int)(fraction*100));

    ui->labelTask->setText(taskName + "(eval "+QString::number(curEval)+"/"+QString::number(totalEval)+")");

    //speed
    int elapsed = lastTime.elapsed();
    double newSpeed = (fraction-oldFraction)/(double)elapsed;
    if(lastSpeeds.size()==nbSpeeds)
        lastSpeeds.removeFirst();
    lastSpeeds.push_back(newSpeed);

    // restart time counter
    lastTime.restart();
}

void WidgetProgress::newTask(QString _taskName)
{
    taskName = _taskName;
    ui->labelTask->setText(taskName);
    setProgress(0);

    lastSpeeds.clear();

    timer->start(period);
}


void WidgetProgress::endTask()
{
    taskName = "";
    ui->labelTask->setText(taskName);
    setProgress(0);

    timer->stop();
}

void WidgetProgress::updateTimeLeft()
{


    double averageSpeed=0;

    if(lastSpeeds.size()>0)
    {
        ui->labelTimeLeft->show();
        for(int i=0;i<lastSpeeds.size();i++)
            averageSpeed += lastSpeeds.at(i);
        averageSpeed = averageSpeed / lastSpeeds.size();

        int timeLeft = (1-fraction)/averageSpeed;

        QTime dispTime(0,0,0);
        dispTime = dispTime.addMSecs(timeLeft);


        ui->labelTimeLeft->setText("-"+dispTime.toString());
    }
    else
        ui->labelTimeLeft->hide();
}





