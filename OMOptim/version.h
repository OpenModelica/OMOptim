#ifndef VERSION_H
#define VERSION_H

#include <QtCore/QString>
#include "omc_config.h"

namespace Version
{

 static QString version()
 {
   return QString(GIT_SHA);
 }
}

#endif // VERSION_H
