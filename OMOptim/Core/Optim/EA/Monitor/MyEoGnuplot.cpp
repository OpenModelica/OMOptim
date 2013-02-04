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

     @file MyEoGnuplot.cpp
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version 0.9

  */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sstream>
#include <stdexcept>

#include "MyEoGnuplot.h"
#ifdef WIN32
#include "pipecom.h"
#define popen _popen
#define pclose _pclose
#else
#include <stdio.h>
#endif

unsigned MyEoGnuplot::numWindow=0;

FILE *MyEoGnuplot::gnuplotPipe=0;
bool MyEoGnuplot::pipeOpen=false;

MyEoGnuplot::MyEoGnuplot()
{
    initGnuPlot();
}

MyEoGnuplot::~MyEoGnuplot()
{
    if( gnuplotPipe )
        pclose(gnuplotPipe);

}

void MyEoGnuplot::gnuplotCommand(const QString & _command)
{
    QString cmd = _command+"\n";
    if(pipeOpen)
    {
        fprintf(gnuplotPipe, cmd.toAscii().data());
        fflush(gnuplotPipe);
    }

}


void MyEoGnuplot::initGnuPlot()
{
    bool useGnuplot = MOSettings::value("useGnuplot",true).toBool();
    
    if(!useGnuplot)
    {
        pipeOpen=false;
    }
    else
    {
        if(!pipeOpen)
        {
            QString pgnuplotFilePath = MOSettings::value("pgnuplotEXE").toString();
            QString command("\""+pgnuplotFilePath +"\" -persist");
            command.replace("/","\\");

            InfoSender::instance()->debug("Loading gnuplot : "+pgnuplotFilePath);
            gnuplotPipe = popen(command.toAscii().data(),"w");

#ifdef WIN32
            //pipeOpen = (bool)((*gnuplotPipe)._ptr);
            pipeOpen = (bool)((gnuplotPipe));
#else
            pipeOpen = gnuplotPipe != NULL;
#endif
            //pipeOpen = (gnuplotPipe!=NULL);
            if(!pipeOpen)
            {
                InfoSender::instance()->debug("Failed loading gnuplot");
            }
            else
            {
                InfoSender::instance()->debug("Succesfully loaded gnuplot");
#ifdef WIN32
                gnuplotCommand("set terminal windows");
#else
                gnuplotCommand("set terminal wxt");
#endif
            }
        }
    }
}



// Local Variables:
// c-file-style: "Stroustrup"
// fill-column: 80
// End:
