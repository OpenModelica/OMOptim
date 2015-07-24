#ifndef MODPLUSEXECTRL_H
#define MODPLUSEXECTRL_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QDir>
#endif

#include "ModPlusCtrl.h"
#include "ModExePlus.h"

class PlugInterface;

class ModPlusExeCtrl : public ModPlusCtrl
{
public:

    enum Output{CSV,MAT};
    enum Parameters{STOPTIME,MAXSIMTIME,SOLVER,TOLERANCE,STEPSIZE,STARTTIME,OUTPUT};
    enum Solvers{DASSL,EULER};


    ModPlusExeCtrl(Project* project,ModelPlus* model);
    virtual ~ModPlusExeCtrl(void);

    virtual ModPlusCtrl::Type type() const = 0;
    QString name();


    // Variables functions
    virtual bool readInitialVariables(MOVector<Variable> *, QFileInfoList filesToCopy,bool forcerecompile = false, QString initFile="") =0;
    void setInputVariablesXml(QDomDocument & doc, QString modelName, MOVector<Variable> *variables);
//    void setInputParametersXml(QDomDocument &doc,MOParameters *parameters);

    // compatible models
    virtual QList<ModelPlus::ModelType> compatibleModels();

    // Parameters
    void setDefaultParameters();

    // Compile function
    bool isCompiled(){return true;}
    bool compile(const QFileInfoList & moDeps = QFileInfoList(), QFileInfoList filesToCopy = QFileInfoList()){return true;}

    bool createInitFile(const QStringList & moDeps = QStringList());

    // Simulate function
    bool getInputVariablesFromXmlFile(QString filePath, QString modModelName, MOVector<Variable> * variables);
    bool getInputVariablesFromXmlFile(const QDomDocument & doc , QString modModelName,MOVector<Variable> * variables);
    Variable* variableFromFmi(const QDomElement & el,QString modModelName,  bool & ok);
    virtual bool simulate(QDir tempDir, MOVector<Variable> * inputVars, MOVector<Variable> * outputVars, QFileInfoList filesToCopy ,QFileInfoList moDependencies) = 0;
    virtual bool start(QString tempExeFile, int maxNSec);

    //read output resfiles
    bool getFinalVariablesFromFile(QString fileName_, MOVector<Variable> *variables,QString _modelName);
    bool getFinalVariablesFromFile(QTextStream *text, MOVector<Variable> * variables,QString _modelName);
    virtual bool readOutputVariables(MOVector<Variable> *,QString outputfile=""){return true;}
//    void readResultsFromXmlDoc(const QDomDocument & doc, MOVector<Variable> * variables, QString modelName);
    QFileInfo initFile() const;
    QFileInfo exeFile() const;
protected :


};


#endif // MODPLUSEXECTRL_H
