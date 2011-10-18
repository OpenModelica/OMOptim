// $Id: CBCTools.cpp 9418 2011-07-06 11:44:40Z hubert.thieriot $
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
 * Main contributor 2011, Hubert Thierot, CEP - ARMINES (France)

        @file CBCTools.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/


  */

#include "CBCTools.h"


void CBCTools::fill(MilpVariableResult0D &variable,double defaultValue,const QString &txt)
{
    QRegExp regExp("[\\s|\\d]*"+variable.name()+"[\\s]*([\\w|\.|\\d]+)[.]*");

    bool ok;
    QString strValue;
    int index=regExp.indexIn(txt);
    double curValue;
    if((index>-1)&&(regExp.captureCount()==1))
    {
        strValue = regExp.cap(1);
        curValue = strValue.toDouble(&ok);
        if(ok)
            variable.setValue(curValue);
        else
            variable.setValue(defaultValue);
    }
    else
        variable.setValue(defaultValue);
}

void CBCTools::fill(MilpVariableResult1D &variable,const QString &txt)
{
    QRegExp regExp("[\\s|\\d]*"+variable.name()+"\\[([\\w|\.|\\d]+)\\][\\s]*([\\w|\.|\\d]+)[.]*");

    bool ok;
    QString i1,strValue;
    int index=regExp.indexIn(txt);
    double curValue;
    while(index>-1)
    {
        if (regExp.captureCount()==2)
        {
            i1 = regExp.cap(1);
            strValue = regExp.cap(2);
            curValue = strValue.toDouble(&ok);
            variable.insertValue(i1,curValue);
        }
        index = regExp.indexIn(txt,index+1);
    }

}

void CBCTools::fill(MilpVariableResult2D &variable,const QString &txt)
{
    QRegExp regExp("[\\s|\\d]*"+variable.name()+"\\[([\\w|\.|\\d]+),([\\w|\.|\\d]+)\\][\\s]*([\\w|\.|\\d]+)[.]*");

    bool ok;
    QString i1,i2,strValue;
    int index=regExp.indexIn(txt);
    double curValue;
    while(index>-1)
    {
        if (regExp.captureCount()==3)
        {
            i1 = regExp.cap(1);
            i2 = regExp.cap(2);
            strValue = regExp.cap(3);
            curValue = strValue.toDouble(&ok);
            variable.insertValue(MilpKey2D(i1,i2),curValue);
        }
        index = regExp.indexIn(txt,index+1);
    }

}

void CBCTools::fill(MilpVariableResult3D &variable,const QString &txt)
{
    QRegExp regExp("[\\s|\\d]*"+variable.name()+"\\[([\\w|\.|\\d]+),([\\w|\.|\\d]+),([\\w|\.|\\d]+)\\][\\s]*([\\w|\.|\\d]+)[.]*");

    bool ok;
    QString i1,i2,i3,strValue;
    int index=regExp.indexIn(txt);
    double curValue;
    while(index>-1)
    {
        if (regExp.captureCount()==4)
        {
            i1 = regExp.cap(1);
            i2 = regExp.cap(2);
            i3 = regExp.cap(3);
            strValue = regExp.cap(4);
            curValue = strValue.toDouble(&ok);
            variable.insertValue(MilpKey3D(i1,i2,i3),curValue);
        }
        index = regExp.indexIn(txt,index+1);
    }

}

void CBCTools::fill(MilpVariableResult4D &variable,const QString &txt)
{
       QRegExp regExp("[\\s|\\d]*"+variable.name()+"\\[([\\w|\.|\\d]+),([\\w|\.|\\d]+),([\\w|\.|\\d]+),([\\w|\.|\\d]+)\\][\\s]*([\\w|\.|\\d]+)[.]*");

       bool ok;
       QString i1,i2,i3,i4,strValue;
       int index=regExp.indexIn(txt);
       double curValue;
       while(index>-1)
       {
           if (regExp.captureCount()==5)
           {
               i1 = regExp.cap(1);
               i2 = regExp.cap(2);
               i3 = regExp.cap(3);
               i4 = regExp.cap(4);
               strValue = regExp.cap(5);
               curValue = strValue.toDouble(&ok);
               variable.insertValue(MilpKey4D(i1,i2,i3,i4),curValue);
           }
           index = regExp.indexIn(txt,index+1);
       }
}

bool CBCTools::isInstalled()
{
#ifdef WIN32 // Win32
    return false; /// \todo
#else // UNIX environment

    QProcess qp;
    QStringList arguments;
    arguments << "-c";
    QString cmd = "which cbc";
    arguments << cmd;

    qp.start("sh",arguments);
    bool ok=qp.waitForFinished();

    QString result(qp.readAll());
    return (ok && !result.isEmpty());


#endif
}



