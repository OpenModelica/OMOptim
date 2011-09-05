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

InfoSender infoSender;

InfoSender::InfoSender()
{
	logStream = NULL;
}
InfoSender::InfoSender(QTextStream* _logStream)
{
	logStream = _logStream;
}

void InfoSender::setLogStream(QTextStream* _logStream)
{
	logStream = _logStream;
}


InfoSender::~InfoSender(void)
{
}

void InfoSender::send(Info _info)
{
	QString prefix;
	QString suffix;

	switch(_info.infoType)
	{
	case ListInfo::NORMAL2 :
		prefix = "Normal :";
		suffix = "";
		break;
	case ListInfo::WARNING2 :
		prefix = "Warning :";
		suffix = "";
		break;
	case ListInfo::ERROR2 :
		prefix = "<b><font color='red'>Error :";
		suffix = "</font></b>";
		break;
		
	case ListInfo::OMCNORMAL2 :
		prefix = "OMCNormal :";
		suffix = "";
		break;
	case ListInfo::OMCWARNING2 :
		prefix = "OMCWarning :";
		suffix = "";
		break;
	case ListInfo::OMCERROR2 :
		prefix = "<b><font color='red'>OMCError :";
		suffix = "</font></b>";
		break;
        case ListInfo::INFODEBUG :
		prefix = "<b><font color='blue'>Debug :";
		suffix = "</font></b>";
		break;
	}

	if(logStream)
	{
		*logStream << QTime::currentTime().toString().toAscii().data();
		*logStream << "\t";
		*logStream << prefix;
		*logStream << _info.infoMsg;
                *logStream << suffix;
		*logStream << "\n";
	}
	emit sent(_info);
}

void InfoSender::debug(QString msg)
{
        Info _info(msg,ListInfo::INFODEBUG);
	send(_info);
}
