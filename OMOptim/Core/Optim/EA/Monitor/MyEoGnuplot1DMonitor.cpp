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

     @file MyEoGnuplot1DMonitor.cpp
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version 0.9

  */
#include <sstream>
#include "MyEoGnuplot1DMonitor.h"
#include "utils/eoParam.h"


eoMonitor& MyEoGnuplot1DMonitor::operator() (void)
{
    // update file using the eoFileMonitor
    //eoFileMonitor::operator()();

    // sends plot order to gnuplot
    // assumes successive plots will have same nb of columns!!!
    //    if (firstTime)
    //    {
    //        FirstPlot();
    //        firstTime = false;
    //    }
    //    else
    //    {
    //        if(gnuplotPipe)
    //        {
    //            gnuplotCommand("replot\n");
    //        }
    //    }
    gnuplotCommand(plotArchiveCommand());
    return *this;
}



//void MyEoGnuplot1DMonitor::FirstPlot()
//{
//    if (this->vec.size() < 2)
//    {
//        InfoSender::instance()->debug("GnuplotMonitor : Must have some stats to plot!");
//    }
//    QString command;

//    command += "plot";
//    for (unsigned i=1; i<this->vec.size(); i++) {
//        command += " '" + QString(getFileName().c_str()) +
//            "' using 1:" + QString::number(i+1) + " title '" + QString((this->vec[i])->longName().c_str()) + "' with lines" ;
//        if (i<this->vec.size()-1)
//            command += ", ";
//    }
//    command += '\n';

//    gnuplotCommand(command);
//}

QString MyEoGnuplot1DMonitor::plotArchiveCommand()
{


    QString cmd;
    QStringList archFiles = _folder.entryList(QStringList("archive.*"));
    int nbArchFilesConsidered =  5;// disp only the five last generations
    while(archFiles.size()>nbArchFilesConsidered)
        archFiles.removeFirst();

    if((_nbObj>0)&&(archFiles.size()>0))
    {
        int nbRows = (int)(std::ceil(std::sqrt(_nbObj)));
        int nbCols = nbRows;
        int nbPlots = (_nbObj*(_nbObj-1))/2;
        int xCol=0;
        int yCol=1;

        cmd = "cd \'"+_folder.absolutePath()+"\'\n";
        cmd += "set multiplot layout " +QString::number(nbRows)+","+QString::number(nbCols) + "\n";
        cmd += "set autoscale \n";

        for(int iPlot=0;iPlot<nbPlots;iPlot++)
        {
            QString columns = "u "+QString::number(xCol)+":"+QString::number(yCol)+" ";
            QString labels = "set xlabel "+QString::number(xCol)+" \n";
            labels += "set ylabel "+QString::number(yCol)+"\n";
            cmd += labels;

            QString title;
            cmd+= "plot ";
            for(int i=0;i<archFiles.size();i++)
            {
                title = " t \'"+QFileInfo(_folder,archFiles.at(i)).completeSuffix()+"\' ";
                cmd+= "\'"+archFiles.at(i)+"\'" + columns + title;
                cmd+=",";
            }
            cmd.remove(-1,1);
            cmd+="\n";

            // update data columns to plot
            if(yCol<_nbObj-1)
                yCol++;
            else
            {
                xCol++;
                yCol = xCol+1;
            }
        }
        cmd += "unset multiplot";
    }
    return cmd;
}



