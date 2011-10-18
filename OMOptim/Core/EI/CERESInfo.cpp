#include "CERESInfo.h"
#include "MOomc.h"
using namespace EI;

CERESInfo::CERESInfo()
{

}

QString CERESInfo::getFilePath()
{
//#ifdef DEBUG
//    // if debug, uses directly file in CERESInfo folder
//    // this allows to modify .mo and reload it without recompiling
//    QDir CERESInfoDir(QApplication::applicationDirPath());
//    CERESInfoDir.cdUp();
//    CERESInfoDir.cd("Core");
//    CERESInfoDir.cd("CERES");
//    QString CERESInfoPath = CERESInfoDir.absoluteFilePath("CERESInfo.mo");
//    return CERESInfoPath;

//#else
    // if not debug, .mo file is first copied from resource and then, its filepath is returned
    QFile moFile(":/CERES/CERESInfo.mo");
    qDebug(moFile.fileName().toLatin1().data());
    QString newPath = QApplication::applicationDirPath()+QDir::separator()+QFileInfo(moFile).fileName();
    QFile newInfo(newPath);
    bool removeOk;
    if(newInfo.exists())
    {
        newInfo.setPermissions(newInfo.permissions() | QFile::WriteUser | QFile::WriteOwner);
        removeOk = newInfo.remove();
        qDebug(newInfo.errorString().toLatin1().data());
    }

    moFile.copy(newPath);

    return newPath;
//#endif

}

bool CERESInfo::loadCERESInfo(MOomc* moomc)
{
    bool ok;
    QString msg;
    moomc->loadModel(getFilePath(),false,ok,msg);
    return ok;
}
