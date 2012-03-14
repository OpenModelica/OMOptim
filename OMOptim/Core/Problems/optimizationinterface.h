#ifndef OPTIMIZATIONINTERFACE_H
#define OPTIMIZATIONINTERFACE_H

#include "ProblemInterface.h"
#include "Optimization.h"



class OptimizationInterface : public ProblemInterface
{
    Problem* createNewProblem(ProjectBase*,const QStringList ,QString problemType) ;


    bool saveResult(Result*);

    QWidget* createProblemTab(Problem *,QWidget* parent);
    QWidget* createResultTab(Result *,QWidget* parent);

    ModelNeeds modelNeeds(){return SEVERALMODELS;}
    QStringList problemTypes(){return QStringList(Optimization::className());}
    QString name(){return Optimization::className();}

    virtual bool hasQuickEndOption(){return true;}

    virtual Problem* loadProblem(QFileInfo saveFile,const QDomElement & domOMCase,ProjectBase*);
    virtual Result* loadResult(QFileInfo saveFile,const QDomElement & domOMCase,ProjectBase*);

};

#endif // OPTIMIZATIONINTERFACE_H
