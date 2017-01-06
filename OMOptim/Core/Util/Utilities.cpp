/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2017, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.2.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3,
 * ACCORDING TO RECIPIENTS CHOICE.
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
 */
/*
 * @author Adeel Asghar <adeel.asghar@liu.se>
 * @author Adrian Pop <adrian.pop@liu.se>
 */



#include "Utilities.h"
#include <QProcessEnvironment>

/*!
 * \brief Utilities::tempDirectory
 * Returns the application temporary directory.
 * \return
 */
QString& Utilities::tempDirectory()
{
  static int init = 0;
  static QString tmpPath;
  if (!init) {
    init = 1;
#ifdef WIN32
    tmpPath = QDir::tempPath() + "/OpenModelica/OMOptim/";
#else // UNIX environment
    char *user = getenv("USER");
    tmpPath = QDir::tempPath() + "/OpenModelica_" + QString(user ? user : "nobody") + "/OMOptim/";
#endif
    tmpPath.remove("\"");
    if (!QDir().exists(tmpPath))
      QDir().mkpath(tmpPath);
  }
  return tmpPath;
}

QString& Utilities::getOMLog()
{
  static QString OMLogPath = tempDirectory() + QDir::separator() + "OMLog.txt";
  return OMLogPath;
}

QString& Utilities::getOMScriptLog()
{
  static QString OMScriptLogPath = tempDirectory() + QDir::separator() + "OMScriptLog.txt";
  return OMScriptLogPath;
}


QString& Utilities::getMinGWBinPath()
{
    static QString mingw;
#ifdef WIN32
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString om = env.value("OPENMODELICAHOME");
    QString omHome = om + QString("/bin");
    QString omdev = env.value("OMDEV");
    // adrpo: oh the horror :)
    QString mingw32msysOM = "mingw/bin";
    QString mingw32msysOMDEV = "tools/mingw/bin";
    QString mingw32msys2 = "tools/msys/mingw32/bin";
    QString mingw64msys2 = "tools/msys/mingw64/bin";
#if defined(__MINGW32__) && defined(__MINGW64__)
    // try OM first
    if (QDir(om + QDir::separator() + mingw64msys2).exists())
    {
      mingw = om + QDir::separator() + mingw64msys2;
    }
    else if (QDir(omdev + QDir::separator() + mingw64msys2).exists())
    {
      mingw = omdev + QDir::separator() + mingw64msys2;
    }
#elif defined(__MINGW32__)
    if (QDir(om + QDir::separator() + mingw32msys2).exists())
    {
      mingw = om + QDir::separator() + mingw32msys2;
    }
    else if (QDir(omdev + QDir::separator() + mingw32msys2).exists())
    {
      mingw = omdev + QDir::separator() + mingw32msys2;
    }
    if (mingw.isEmpty())
    {
        // old OpenModelica?!
        if (QDir(om + QDir::separator() + mingw32msysOM).exists())
        {
          mingw = om + QDir::separator() + mingw32msysOM;
        }
        else if (!QDir(omdev + QDir::separator() + mingw32msysOMDEV).exists())
        {
          mingw = omdev + QDir::separator() + mingw32msysOMDEV;
        }
    }
#endif
    if (mingw.isEmpty()) // assume minw64
    {
       mingw = om + QDir::separator() + mingw64msys2;
    }
#else
    mingw = "";
#endif
    return mingw;
}

QString& Utilities::getOMBinPath()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    static QString om = env.value("OPENMODELICAHOME") + QDir::separator() + "bin";
    return om;
}
