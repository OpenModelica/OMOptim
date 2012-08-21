#ifndef ONESIMULATIONINTERFACE_H
#define ONESIMULATIONINTERFACE_H

#include "ProblemInterface.h"
#include "OneSimulation.h"



class OneSimulationInterface : public ProblemInterface
{

public :

    virtual Problem* createNewProblem(ProjectBase*,const QStringList ,QString problemType);

    QWidget* createProblemTab(Problem *,QWidget* parent);
    QWidget* createResultTab(Result *,QWidget* parent);


    virtual ModelNeeds modelNeeds(QString){return ONEMODEL;}
    virtual QStringList problemTypes(){return QStringList(OneSimulation::className());}
    QString name(){return OneSimulation::className();}
    virtual Problem* loadProblem(QFileInfo saveFile,const QDomElement & domOMCase,ProjectBase *);
    virtual Result* loadResult(QFileInfo saveFile,const QDomElement & domOMCase,ProjectBase*);
};

#endif
