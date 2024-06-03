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

     @file Info.cpp
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version

  */
#include "Info.h"
#include "ListInfo.h"
#include <QRegExp>
#include <QRegularExpression>

using  namespace ListInfo;

Info::Info()
{
}
Info::Info(const Info & _info)
{
    infoNum = _info.infoNum;
    infoMsg = _info.infoMsg;
    infoType = _info.infoType;
}
Info::Info(ListInfo::InfoNum infoNum_)
{
    try
    {
    infoNum=infoNum_;

    //Looking for Info in static table
    int i=0;
    bool found = false;
    while (i<NBINFOS && !found)
    {
        if(strInfos[i].num==infoNum_)
        {
            found=true;
        }
        else
        {
            i++;
        }
    }

    if (!found)
    {
        infoMsg="Unknown info";
        infoType=NORMAL2;
    }
    else
    {
        infoMsg=strInfos[i].msg;
        infoType=strInfos[i].type;
    }
    }
    catch(std::exception &e)
    {
    }
}

Info::Info(ListInfo::InfoNum infoNum_, QString strSupp)
{
    try
    {
    infoNum=infoNum_;

    //Looking for Info in static table
    int i=0;
    bool found = false;
    while (i<NBINFOS && !found)
    {
        if(strInfos[i].num==infoNum_)
        {
            found=true;
        }
        else
        {
            i++;
        }
    }

    if (!found)
    {
        infoMsg="Unknown info";
        infoType=NORMAL2;
    }
    else
    {
        infoMsg.clear();
        infoMsg.asprintf(strInfos[i].msg.toLatin1().data(),strSupp.toLatin1().data());
        infoType=strInfos[i].type;
    }
    }
    catch(std::exception &e)
    {
    }
}

Info::Info(ListInfo::InfoNum infoNum_, QString strSupp1, QString strSupp2)
{
    try{
    infoNum=infoNum_;

    //Looking for Info in static table
    int i=0;
    bool found = false;
    while (i<NBINFOS && !found)
    {
        if(strInfos[i].num==infoNum_)
        {
            found=true;
        }
        else
        {
            i++;
        }
    }

    if (!found)
    {
        infoMsg="Unknown info";
        infoType=NORMAL2;
    }
    else
    {
        infoMsg.clear();
        infoMsg.asprintf(strInfos[i].msg.toLatin1().data(),strSupp1.toLatin1().data(),strSupp2.toLatin1().data());
        infoType=strInfos[i].type;
    }
    }
    catch(std::exception &e)
    {
    }
}




Info::Info(QString _msg, ListInfo::InfoType _msgType)
{
    _msg.remove(QRegularExpression("\\n$"));
    infoMsg = _msg;
    infoType = _msgType;
}
