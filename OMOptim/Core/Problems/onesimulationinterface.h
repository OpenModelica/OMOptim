#ifndef ONESIMULATIONINTERFACE_H
#define ONESIMULATIONINTERFACE_H

#include "ProblemInterface.h"
#include "OneSimulation.h"



class OneSimulationInterface : public ProblemInterface
{
    virtual Problem* createNewProblem(Project*,const QList<ModModelPlus*> &,QString problemType);



    QWidget* createProblemTab(Problem *,QWidget* parent);
    QWidget* createResultTab(Result *,QWidget* parent);


    virtual ModModelPlusNeeds modModelPlusNeeds(){return ONEMODMODELPLUS;}
    virtual QStringList problemTypes(){return QStringList(OneSimulation::className());}
    QString name(){return OneSimulation::className();}
    virtual Problem* loadProblem(QFileInfo saveFile,const QDomElement & domOMCase,Project*);
    virtual Result* loadResult(QFileInfo saveFile,const QDomElement & domOMCase,Project*);
};

#endif
