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

 	@file InfoSender.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 

  */
#include "InfoSender.h"
#include "MOThreads.h"

// Global static pointer used to ensure a single instance of the class.
InfoSender* InfoSender::_instance = NULL;

InfoSender::InfoSender()
{
        _logStream = NULL;
}

void InfoSender::setLogStream(QTextStream* logStream)
{
        _logStream = logStream;
}


InfoSender::~InfoSender(void)
{
}

InfoSender* InfoSender::instance()
{
    // instance pointer is stored in qapp properties : allows to share with plugins
    // Otherwise, plugins create a new instance
    // A correct way would be to build an OMOptim shared lib, that exe and plugin would share
    if(!_instance)
    {
        if(qApp->property("InfoSender").isValid())
        {
            _instance = dynamic_cast<InfoSender*>(qApp->property("InfoSender").value<QObject*>());
        }
        else
        {
            _instance = new InfoSender();
            qApp->setProperty("InfoSender",qVariantFromValue(qobject_cast<QObject*>(_instance)));
        }
    }
    return _instance;
}

void InfoSender::send(Info info)
{
        if(_logStream)
	{
                *_logStream << QTime::currentTime().toString().toAscii().data();
                *_logStream << "\t";
                *_logStream << info.infoMsg;
                *_logStream << "\n";
	}
        emit sent(info);
}

