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

        @file MilpTools.h
        @brief Comments for file documentation.
        @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
        Company : CEP - ARMINES (France)
        http://www-cep.ensmp.fr/english/
        @version 0.9

  */


#ifndef MILPTOOLS_H
#define MILPTOOLS_H

#include <QtCore/QObject>
#include <QtCore/QString>

class MilpKey2D
{
public :
        MilpKey2D(){};
    MilpKey2D(QString,QString);

    bool operator==(const MilpKey2D& key2) const;
    bool operator<(const MilpKey2D& key2) const;
    MilpKey2D operator=(const MilpKey2D& key2);

    QString toString();

    QString row(){return _row;};
    QString col(){return _col;};
private :
        QString _row;
        QString _col;
};

class MilpKey3D
{
public :
        MilpKey3D(){};
    MilpKey3D(QString,QString,QString);

    bool operator==(const MilpKey3D& key2) const;
    bool operator<(const MilpKey3D& key2) const;
    MilpKey3D operator=(const MilpKey3D& key2);

    QString toString();

    QString i1(){return _i1;};
    QString i2(){return _i2;};
    QString i3(){return _i3;};
private :
        QString _i1;
        QString _i2;
        QString _i3;
};

class MilpKey4D
{
public :
        MilpKey4D(){};
    MilpKey4D(QString,QString,QString,QString);

    bool operator==(const MilpKey4D& key2) const;
    bool operator<(const MilpKey4D& key2) const;
    MilpKey4D operator=(const MilpKey4D& key2);

    QString toString();

    QString i1(){return _i1;};
    QString i2(){return _i2;};
    QString i3(){return _i3;};
    QString i4(){return _i4;};
private :
        QString _i1;
        QString _i2;
        QString _i3;
        QString _i4;
};


#endif // MILPTOOLS_H
