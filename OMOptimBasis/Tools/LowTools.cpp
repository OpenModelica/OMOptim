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

    @file LowTools.cpp
    @brief Comments for file documentation.
    @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
    Company : CEP - ARMINES (France)
    http://www-cep.ensmp.fr/english/
    @version

  */
#include "LowTools.h"
#include <QVector>
#include <QFile>
#include "METime.h"
#include "MOItem.h"
#include "InfoSender.h"

LowTools::LowTools(void)
{
}

LowTools::~LowTools(void)
{
}

bool LowTools::removeFile(QString filePath)
{
    if(!QFile::exists(filePath))
        return true;

    bool removeOk = QFile::remove(filePath);

    if(!removeOk)
    {
        QFileInfo fileInfo(filePath);
        QFile(filePath).setPermissions(fileInfo.permissions() | QFile::WriteUser);
        removeOk = QFile::remove(filePath);
    }
    return removeOk;
}

bool LowTools::removeDirContents(QString folder)
{
    bool  allRemoved = true;
    bool fileRemoved;

    QDir dir = QDir(folder);
    QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden);
    for (int indf=0;indf<files.size();indf++)
    {
        QFileInfo provFile(files.at(indf).absoluteFilePath());

        // provFile.setPermissions(provFile.permissions() | QFile::WriteUser);
        fileRemoved = dir.remove(provFile.fileName());
        if(!fileRemoved)
        {
            QFile(provFile.absoluteFilePath()).setPermissions(provFile.permissions() | QFile::WriteUser);
            fileRemoved = dir.remove(provFile.fileName());
        }

        allRemoved = fileRemoved && allRemoved;
    }

    QStringList folders = dir.entryList(QDir::AllDirs| QDir::NoDotAndDotDot);
    QString provFolder;
    for (int indf=0;indf<folders.size();indf++)
    {
        provFolder = folders[indf];
        allRemoved = removeDir(dir.absolutePath()+QDir::separator()+provFolder) && allRemoved;
    }

    return allRemoved;

}

bool LowTools::removeDirContentsExcept(QString folder,QStringList exceptExtensions)
{
    bool  allRemoved = true;

    QDir dir = QDir(folder);
    QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden);
    for (int indf=0;indf<files.size();indf++)
    {
        QFileInfo provFile(files.at(indf).absoluteFilePath());
        if(!exceptExtensions.contains(provFile.suffix()) && !exceptExtensions.contains(provFile.completeSuffix()))
            allRemoved = dir.remove(provFile.fileName()) && allRemoved;
    }

    QStringList folders = dir.entryList(QDir::AllDirs| QDir::NoDotAndDotDot);
    QString provFolder;
    for (int indf=0;indf<folders.size();indf++)
    {
        provFolder = folders[indf];
        allRemoved = removeDir(dir.absolutePath()+QDir::separator()+provFolder) && allRemoved;
    }

    return allRemoved;
}

bool LowTools::removeDir(QString folder)
{

    bool removed;
    QDir dir = QDir(folder);
    // if dir already exists, deleting it
    if (dir.exists())
    {
        LowTools::removeDirContents(folder);

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

void LowTools::copyDirContents(QString org,QString dest)
{
    InfoSender::instance()->debug("Copy dir contents "+org +" to "+dest);

    QDir orgDir = QDir(org);

    QDir destDir(dest);
    // if dir does not exist, create it
    if(!destDir.exists())
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

bool LowTools::copyFilesInFolder(QFileInfoList files, QDir folder)
{
    bool allCopyOk = true;
    bool tmpBool;
    if(!folder.exists())
    {
        LowTools::mkpath(folder.absolutePath(),false);
    }

    for(int i=0;i<files.size();i++)
    {
        QFileInfo fileInfo(files.at(i));
        if(folder.exists(fileInfo.fileName()))
            folder.remove(fileInfo.fileName());
        tmpBool = QFile::copy(fileInfo.absoluteFilePath(),folder.filePath(fileInfo.fileName()));
        if(!tmpBool)
        {
            SleeperThread::msleep(1000);
            tmpBool = QFile::copy(fileInfo.absoluteFilePath(),folder.filePath(fileInfo.fileName()));
        }
        allCopyOk = allCopyOk && tmpBool;
    }
    return allCopyOk;
}

/**
  * Will look in folder and subfolders for all files with extension indicated.
  *
  *
  */
QFileInfoList LowTools::findFiles(QDir folder, QString extension)
{
    QStringList fileType;
    fileType<< "*."+extension;

    QFileInfoList result;
    QStringList more;
    QStringList::Iterator it;

    more = folder.entryList(fileType,QDir::Files );
    for ( int i=0;i<more.size();i++)
        result.push_back(QFileInfo(folder,more.at(i)));

    // reg exp in next line excludes . and .. dirs (and .* actually)
    more = folder.entryList( QDir::Dirs ).filter(QRegExp( "[^.]" ) );
    QString subDirPath;
    for ( int i=0;i<more.size();i++)
    {
        subDirPath = folder.absolutePath()+QDir::separator()+more.at(i);
        result.append(LowTools::findFiles(QDir(subDirPath),extension));
    }

    return result;
}


void LowTools::copyDir(QString org,QString dest)
{
    InfoSender::instance()->debug("Copy dir "+org +" to "+dest);

    QDir destDir(dest);
    // if dir already exists, deleting it
    if(destDir.exists())
        removeDir(dest);

    copyDirContents(org,dest);
}

bool LowTools::mkpath(QString dirPath,bool eraseExisting)
{
    bool ok;

    QDir tempDir(dirPath);
    QFile tempDirFile(dirPath);
    //tempDirFile.setPermissions(tempDirFile.permissions() | QFile::WriteUser);

    if(tempDir.exists()&&eraseExisting)
        LowTools::removeDir(dirPath);

    bool mkOk = QDir().mkpath(dirPath);
    tempDir.refresh();
    tempDirFile.setPermissions(tempDirFile.permissions() | QFile::WriteUser);
    ok = tempDir.exists();
    ok = ok && (tempDir.entryList(QDir::NoDotAndDotDot).isEmpty() || !eraseExisting);

    return ok;
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
    LowTools::removeDuplicates<QString>(list);
}

void LowTools::removeDuplicates(QFileInfoList &list)
{
    LowTools::removeDuplicates<QFileInfo>(list);
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

//void LowTools::sortItemsByField(QList<MOItem *> &list, int iField)
//{
//        //filling index
//        QMap<QVariant,MOItem*> map;
//        QVariant index;
//        for(int i=0;i<list.size();i++)
//        {
//            index = list.at(i)->getFieldValue(iField);
//            map.insert(index,list.at(i));
//        }
//        QList<QVariant> indexes = map.keys();


//        // try with double


//        // try with QString




//        //sorting
//        qSort(indexes.begin(),indexes.end());

//        //filling list
//        list.clear();
//        for(int i=0;i<indexes.size();i++)
//            list.push_back(map.value(indexes.at(i)));
//}

void LowTools::openFolder(QString folderPath)
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(folderPath));
}

bool LowTools::openFile(QString filePath)
{
    return QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
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

