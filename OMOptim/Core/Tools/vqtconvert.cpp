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
