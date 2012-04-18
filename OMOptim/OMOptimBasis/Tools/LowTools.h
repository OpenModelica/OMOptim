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
#include "InfoSender.h"


class LowTools
{
public:
    LowTools(void);
    ~LowTools(void);

    static void openFolder(QString);
    static bool openFile(QString);

    static bool removeDir(QString);
    static void copyDir(QString,QString);

    static QStringList getDuplicates(const QStringList & list);
    static void removeDuplicates(QStringList & list);
    static void removeDuplicates(QVector<double> & vector);
    static void removeWhiteSpaceStrings(QStringList &list);



    static QString getValueFromElementInfo(QString elementInfoLine,QString fieldName);

    static int round(double);
    static double roundToMultiple(double value, double multiple);
    static QList<int> nextIndex(QList<int> oldIndex, QList<int>  maxIndex);

    static bool isNan(double d);
    static double nan();

    static double gaussRandom(double average,double dev);



    static bool removeDirContents(QString folder);
    static void copyDirContents(QString org, QString dest);
    static void copyFilesInFolder(QStringList files, QString folder);
};

template < class T>
class removeDuplicates
{
public :
    static void apply(QList<T> &list)
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
};


template < class T>
class sortItems
{
public :
    static void applyToInt(QList<T*> &list, int iField)
    {
        //filling index
        QMap<int,T*> map;
        int index;
        for(int i=0;i<list.size();i++)
        {
            index = list.at(i)->getFieldValue(iField).toInt();
            map.insert(index,list.at(i));
        }
        QList<int> indexes = map.keys();



        //sorting
        qSort(indexes.begin(),indexes.end());

        //filling list
        list.clear();
        for(int i=0;i<indexes.size();i++)
            list.push_back(map.value(indexes.at(i)));
    }
};

#endif
