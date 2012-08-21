#ifndef MODPLUSDYMOLAEXECTRL_H
#define MODPLUSDYMOLAEXECTRL_H



#include "QProcess.h"
#include "ModPlusExeCtrl.h"


class ModPlusDymolaExeCtrl: public ModPlusExeCtrl
{
public:
    ModPlusDymolaExeCtrl(Project* project,ModelPlus* model);
    ~ModPlusDymolaExeCtrl();

    ModPlusCtrl* clone();

    QString name();
    virtual ModPlusDymolaExeCtrl::Type type() const;

    void setDefaultParameters();
    bool readOutputVariables(MOVector<Variable> *finalVariables,QString path);
    bool readOutputVariablesDSFINAL(MOVector<Variable> *finalVariables, QString dsfinalFile);
    bool readOutputVariablesDSRES(MOVector<Variable> *finalVariables, QString dsresFile);
    bool readInitialVariables(MOVector<Variable> *initVariables,bool forceRecompile, QString dsinFile);
    bool simulate(QDir tempDir,MOVector<Variable> * updatedVars,MOVector<Variable> * outputVars,QFileInfoList filesTocopy,QFileInfoList moDependencies);

protected :
    QProcess _simProcess;
    QString _dsfinalFile;
    QString _dsresFile;


};

#endif // MODPLUSDYMOLAEXECTRL_H
