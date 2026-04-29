/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
 * VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the OSMC (Open Source Modelica Consortium)
 * Public License (OSMC-PL) are obtained from OSMC, either from the above
 * address, from the URLs:
 * http://www.openmodelica.org or
 * https://github.com/OpenModelica/ or
 * http://www.ida.liu.se/projects/OpenModelica,
 * and in the OpenModelica distribution.
 *
 * GNU AGPL version 3 is obtained from:
 * https://www.gnu.org/licenses/licenses.html#GPL
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

/*
Converts between QT types and Windows Types
Version: 1.0
Modified on: 8-20-2009
Created with: QT 4.5 and QT Creator 1.2
Tested on: Windows XP SP3
Bryan Cairns - August 2009
*/

#ifndef VQTCONVERT_H
#define VQTCONVERT_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QString>
#endif

#ifdef WIN32
    #include "windows.h"
#endif

#undef min
#undef max

class VQTConvert
{
public:
#ifdef WIN32
    //Convert a QString To LPCTSTR
    LPCTSTR static QString_To_LPCTSTR(QString mQTData);

    //Convert a QString To LPCSTR
    LPCSTR static QString_To_LPCSTR(QString mQTData);

    //Convert a QString To LPTSTR
    LPTSTR static QString_To_LPTSTR(QString mQTData);

    //Convert a LPCTSTR To QString
    QString static LPCTSTR_To_QString(LPCTSTR mWinData);

    //Convert a LPBYTE To QString
    QString static LPBYTE_To_QString(LPBYTE mWinData);

    //Convert a Char[] To QString
    QString static Char_To_QString(char mWinData[]);

    //Convert a WCHAR* to a QString
//    QString static WCHAR_to_QString(WCHAR* mBuffer);

    //Convert a TCHAR To QString
    QString static TChar_To_QString(TCHAR mWinData[]);
#endif
};

#endif // VQTCONVERT_H
