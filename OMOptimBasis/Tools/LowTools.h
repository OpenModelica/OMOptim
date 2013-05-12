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

     @file LowTools.h
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version

  */
#ifndef LOWTOOLS_H
#define LOWTOOLS_H


#include <QtCore/QDir>
#include <vector>
#include <QDesktopServices>
#include <QUrl>

#include <math.h>

#include "SleeperThread.h"

class METime;
class MOItem;

/**
  * LowTools is a class offering basic functions such as files management, list managment (filtering, sorting)
  * and some numerical functions.
  */
class LowTools
{
public:
    LowTools(void);
    ~LowTools(void);

    // files/ folder functions
    static void openFolder(QString);
    static bool openFile(QString);

    static bool removeDir(QString);
    static void copyDir(QString,QString);
    static bool mkpath(QString,bool eraseExisting);

    static bool removeFile(QString filePath);
    static bool removeDirContents(QString folder);
    static bool removeDirContentsExcept(QString folder, QStringList exceptExtensions);
    static void copyDirContents(QString org, QString dest);
    static bool copyFilesInFolder(QFileInfoList files, QDir folder);

    static QFileInfoList findFiles(QDir folder,QString extension);


    // list management
    static QStringList getDuplicates(const QStringList & list);
    static void removeDuplicates(QStringList & list);
    static void removeDuplicates(QFileInfoList & list);
    static void removeDuplicates(QVector<double> & vector);
    static void removeWhiteSpaceStrings(QStringList &list);

    template < class T>
    static void removeDuplicates(QList<T> &list);

//    static void sortItemsByField(QList<MOItem*> &list,int iField);


    // numerical functions
    static int round(double);
    static double round(double, int nbDecimals);
    static double roundToMultiple(double value, double multiple);
    static QList<int> nextIndex(QList<int> oldIndex, QList<int>  maxIndex);

    static bool isNan(double d);
    static double nan();

    static double gaussRandom(double average,double dev);

    static QString commonSections(const QStringList &names);

    static QList<QList<int> > getAllCombinations(const QList<int> &indexes, int combSize);

private :
     static void getAllCombinations(const QList<int> &indexes, int combSize, int offset, QList<int> &curCombination, QList<QList<int> > &allCombinations);
};

template < class T>
void LowTools::removeDuplicates(QList<T> &list)
{
    int j;
    for(int i=0;i<list.size();i++)
    {
        j=list.indexOf(list.at(i),i+1);
        while(j>i)
        {
            list.removeAt(j);
            j=list.indexOf(list.at(i),i+1);
        }
    }
}

//template < class T>
//class removeDuplicates
//{
//public :
//    static void apply(QList<T> &list)
//    {
//        int j;
//        for(int i=0;i<list.size();i++)
//        {
//            j=list.indexOf(list.at(i),i+1);
//            while(j>i)
//            {
//                list.removeAt(j);
//                j=list.indexOf(list.at(i),i+1);
//            }
//        }
//    }
//};


#endif
