// $Id$
/**
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linkpings universitet, Department of Computer and Information Science,
 * SE-58183 Linkping, Sweden.
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

    @file LowTools.cpp
    @brief Comments for file documentation.
    @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
    Company : CEP - ARMINES (France)
    http://www-cep.ensmp.fr/english/
    @version

  */
#include "LowTools.h"

LowTools::LowTools(void)
{
}

LowTools::~LowTools(void)
{
}


bool LowTools::removeDir(QString folder)
{

    QDir dir = QDir(folder);
    // if dir already exists, deleting it
    if (dir.exists())
    {
        bool removed,tempBool;

        QStringList files = dir.entryList(QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden);
        QString provFile;
        for (int indf=0;indf<files.size();indf++)
        {
            provFile = files[indf];
            tempBool = dir.remove(provFile);
        }

        QStringList folders = dir.entryList(QDir::AllDirs| QDir::NoDotAndDotDot);
        QString provFolder;
        for (int indf=0;indf<folders.size();indf++)
        {
            provFolder = folders[indf];
            removeDir(dir.absolutePath()+QDir::separator()+provFolder);
        }

        bool temp = dir.cdUp();
        dir.refresh();
        removed = dir.rmdir(folder);
        if (!removed)
        {
            //wait one second before retrying (maybe useful during MOO)
            SleeperThread::msleep(1000);
            removed = dir.rmdir(folder);

            if(!removed)
            {
                InfoSender::instance()->send(Info(ListInfo::FOLDERUNREMOVABLE,folder));
                return false;
            }
        }

        return removed;

    }
    else
        return true;

}

void LowTools::copyDir(QString org,QString dest)
{
    QDir orgDir = QDir(org);

    QDir destDir(dest);
    // if dir already exists, deleting it
    if(destDir.exists())
        removeDir(dest);

    destDir.mkpath(dest);


    if (orgDir.exists())
    {
        QStringList files = orgDir.entryList();
        for (int indf=0;indf<files.size();indf++)
        {
            QString orgFilePath = orgDir.absoluteFilePath(files[indf]);
            QString destFilePath = destDir.absoluteFilePath(files[indf]);

            QFile::copy(orgFilePath,destFilePath);
        }
    }
}

QStringList LowTools::getDuplicates(const QStringList & list)
{
    QStringList result;

    for(int i=0;i<list.size();i++)
    {
        if(list.indexOf(list.at(i),i+1)>-1)
            result.push_back(list.at(i));
    }
    return result;
}

void LowTools::removeDuplicates(QVector<double> &vector)
{

    for(int i=0;i<vector.size();i++)
    {
        while(vector.lastIndexOf(vector.at(i))>i)
        {
            vector.remove(vector.lastIndexOf(vector.at(i)));
        }
    }
}

void LowTools::removeDuplicates(QStringList &list)
{

    for(int i=0;i<list.size();i++)
    {
        while(list.lastIndexOf(list.at(i))>i)
        {
            list.removeAt(list.lastIndexOf(list.at(i)));
        }
    }
}


void LowTools::removeWhiteSpaceStrings(QStringList &list)
{
    // remove empty strings
    int iEmpty=list.indexOf(QRegExp("\\s*"));
    while(iEmpty>-1)
    {
        list.removeAt(iEmpty);
        iEmpty=list.indexOf(QRegExp("\\s*"));
    }
}

void LowTools::openFolder(QString folderPath)
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(folderPath));


}

bool LowTools::openFile(QString filePath)
{
    return QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}


QString LowTools::getValueFromElementInfo(QString elementInfoLine,QString fieldName)
{

    QStringList fields = elementInfoLine.split(",");
    QRegExp regExp(".*"+fieldName+"=.*");

    int iField=fields.indexOf(regExp);
    if(iField>-1)
    {
        try{
            QString value = fields.at(iField).section("=",1,1);

            // removing whitespaces for non-path fields
            if(value.indexOf("\"")==-1)
            {
                value.remove(" ");
            }
            return value;
        }
        catch(std::exception &e)
        {
            return QString();
        }
    }
    return QString();
}


QList<int> LowTools::nextIndex(QList<int> oldIndex, QList<int>  maxIndex)
{
    if(oldIndex.size()<0 || oldIndex.size() != maxIndex.size())
    {
        return QList<int>();
    }
    else
    {
        int iIndex = oldIndex.size()-1;

        while((iIndex>-1) && (oldIndex.at(iIndex) == maxIndex.at(iIndex)))
            iIndex--;

        if(iIndex==-1)
            return QList<int>();
        else
        {
            QList<int> newIndex = oldIndex;
            newIndex.replace(iIndex,newIndex.at(iIndex)+1);

            for(int i = iIndex+1; i<newIndex.size(); i++)
                newIndex.replace(i,0);

            return newIndex;
        }
    }
}

int LowTools::round(double d)
{
    return (int)(d+0.5);
}

double LowTools::roundToMultiple(double value, double multiple)
{
    if(multiple==0)
        return value;

    return multiple * round(value/multiple);
}

bool LowTools::isNan(double d)
{
    return (d!=d);
}

double LowTools::nan()
{
    return sqrt((double)-1);
}

double LowTools::gaussRandom(double average,double dev)
{
    // generating gauss random function from uniform
    double x1, x2, w, y;

    do {
        x1 = 2.0 * double(rand()) / double(1.0 + RAND_MAX)- 1.0;
        x2 = 2.0 * double(rand()) / double(1.0 + RAND_MAX) - 1.0;
        w = x1 * x1 + x2 * x2;
    } while ( w >= 1.0 );

    w = sqrt( (-2.0 * log( w ) ) / w );
    y = x1 * w;

    double result = average + dev * y;
    return result;

}

