#ifndef MODPLUSOMEXECTRL_H
#define MODPLUSOMEXECTRL_H

#include "ModPlusExeCtrl.h"

class Project;
class ModelPlus;
class ModPlusCtrl;
class QString;
class QDir;



class ModPlusOMExeCtrl: public ModPlusExeCtrl
{
public:
    ModPlusOMExeCtrl(Project* project,ModelPlus* model);
    ~ModPlusOMExeCtrl(void);

    ModPlusCtrl *clone();


    QString name();
    bool useMat();
    QString resFile();
    QString resMatFile();
    QString resCsvFile();
    ModPlusCtrl::Type type() const;
    void setDefaultParameters();

    virtual bool readInitialVariables(MOVector<Variable> *,bool forcerecompile, QString initFile="");

    bool simulate(QDir tempFolder, MOVector<Variable> *inputVars, MOVector<Variable> *outputVars, QFileInfoList filesToCopy, QFileInfoList moDependencies);
    bool start(QString exeFile,int maxnsec);
    bool readOutputVariables(MOVector<Variable> *,QString resFile="");
};

#endif // MODPLUSOMEXECTRL_H
