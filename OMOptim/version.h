#ifndef VERSION_H
#define VERSION_H

#include <QtCore/QString>

namespace Version
{
 static const int MAJOR = 0;
 static const int MINOR = 9;
 static const int REVISION = 16501;
 
 
 static QString version()
 {
   return QString::number(MAJOR)+QString::number(MINOR)+QString::number(REVISION);
 }
}

#endif // VERSION_H
