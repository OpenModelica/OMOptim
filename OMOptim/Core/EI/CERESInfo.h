#ifndef CERESINFO_H
#define CERESINFO_H

#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QApplication>
#include <QtCore/QDir>

class MOomc;

namespace EI
{
/**
  * Basic functions concerning CERESInfo modelica library (only concerns CERES project)
  */
class CERESInfo
{
public:
    CERESInfo();

    static QString getFilePath();
    static bool loadCERESInfo(MOomc*);


};
}

#endif // CERESINFO_H
