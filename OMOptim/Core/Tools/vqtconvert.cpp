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

#include "vqtconvert.h"

#ifdef WIN32

#include "windows.h"
#undef min
#undef max

//Convert a QString To LPCTSTR
LPCTSTR VQTConvert::QString_To_LPCTSTR(QString mQTData)
{
    return (LPCTSTR)mQTData.utf16();
}

//Convert a QString To LPCSTR
LPCSTR VQTConvert::QString_To_LPCSTR(QString mQTData)
{
    //return (LPCSTR)mQTData.utf16();
    char* test = mQTData.toLatin1().data();
    return (LPCSTR)(mQTData.toLatin1().data());
}

//Convert a QString To LPTSTR
LPTSTR VQTConvert::QString_To_LPTSTR(QString mQTData)
{
    return (LPTSTR)mQTData.utf16();
}

//Convert a LPCTSTR To QString
QString VQTConvert::LPCTSTR_To_QString(LPCTSTR mWinData)
{
    return QString::fromUtf16((ushort*)mWinData);
}

//Convert a LPBYTE To QString
QString VQTConvert::LPBYTE_To_QString(LPBYTE mWinData)
{
    return QString::fromUtf16((ushort*)mWinData);
}

//Convert a Char[] To QString
QString VQTConvert::Char_To_QString(char mWinData[])
{
    return QString::fromUtf16((ushort*)mWinData);
}

////Convert a WCHAR* to a QString
//QString VQTConvert::WCHAR_to_QString(WCHAR* mBuffer)
//{
//    return QString::fromWCharArray(mBuffer);
//}

//Convert a TCHAR To QString
QString VQTConvert::TChar_To_QString(TCHAR mWinData[])
{
    return QString::fromUtf16((ushort*)mWinData);
}

#endif
