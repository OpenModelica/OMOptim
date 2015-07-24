#ifndef MODPLUSBLACKBOXEXECTRL_H
#define MODPLUSBLACKBOXEXECTRL_H

#include "ModPlusCtrl.h"
#include "ModPlusExeCtrl.h"


class Project;
class ModelPlus;
class PlugInterface;


class ModPlusBlackBoxExeCtrl : public ModPlusExeCtrl
{
public:
    ModPlusBlackBoxExeCtrl(Project* project,ModelPlus* model);
    ~ModPlusBlackBoxExeCtrl(void);

    virtual ModPlusCtrl::Type type() const;
    ModPlusCtrl *clone();
    QString name();

    //Load Plugin
    PlugInterface* loadPlugInterface() const;

    //Variables Functions
    virtual bool readInitialVariables(MOVector<Variable> *,QFileInfoList filesToCopy, bool forcerecompile, QString initFile="");
    void setInputVarsInInitFileUsingPluginFunc(QString tempInitFile, QString modelName, MOVector<Variable> *variables);
    bool simulate(QDir tempDir, MOVector<Variable> * inputVars, MOVector<Variable> * outputVars, QFileInfoList filesToCopy ,QFileInfoList moDependencies);
    bool readInitialVarsFromPluginFunc(QString filename, QString modelName,MOVector<Variable> *initVariables);
    bool setStopTime(double time);

    //read output resfiles
    bool getOutputVarsUsingPluginFunc(QString fileName, MOVector<Variable> * variables, QString modelName);
    void readResultsFromXmlDoc(const QDomDocument &doc, MOVector<Variable> * variables, QString modelName);
    QString resFileNameFromPlugFunc() const;

private:
    PlugInterface* _plugInter;

};

#endif // MODPLUSBLACKBOXEXECTRL_H
