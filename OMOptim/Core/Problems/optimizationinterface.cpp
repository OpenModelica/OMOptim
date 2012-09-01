#include "optimizationinterface.h"

#include "TabOptimization.h"
#include "TabResOptimization.h"
#include "OptimResult.h"
#include "Project.h"



Problem* OptimizationInterface::createNewProblem(ProjectBase* projectBase,const QStringList modelsList,QString problemType)

{
    Q_ASSERT(problemType==Optimization::className());

    if(modelsList.size()<1)
    {
        InfoSender::instance()->send(Info("Model for optimization problem not defined",ListInfo::ERROR2));
        return NULL;
    }
    else
    {
        Project* project = dynamic_cast<Project*>(projectBase);
        if(!project)
            return NULL;
        else
            return new Optimization(project,modelsList);
    }

}


bool OptimizationInterface::saveResult(Result* result)
{
    ProblemInterface::saveResult(result);

    OptimResult* optimResult = dynamic_cast<OptimResult*>(result);
    //*********************************
    // Writing points in Front File
    //*********************************
    QDir dir(result->saveFolder());
    QString optVarsfrontFileName = optimResult->_optVarsFrontFileName;
    QString allVarsfrontFileName = optimResult->_allVarsFrontFileName;
    optimResult->exportFrontCSV(dir.absoluteFilePath(optVarsfrontFileName), false);
    optimResult->exportFrontCSV(dir.absoluteFilePath(allVarsfrontFileName), true);

    return true;
}

QWidget* OptimizationInterface::createProblemTab(Problem * problem,QWidget* parent)
{
    Optimization* optim = dynamic_cast<Optimization*>(problem);
    return new TabOptimization(optim,parent);
}

QWidget* OptimizationInterface::createResultTab(Result * result,QWidget* parent)
{
    OptimResult* optimResult = dynamic_cast<OptimResult*>(result);
    return new TabResOptimization(optimResult,parent);
}


Problem* OptimizationInterface::loadProblem(QFileInfo saveFile,const QDomElement & domOMCase, ProjectBase * projectBase)
{
    if(domOMCase.isNull() || domOMCase.tagName()!="OMCase" )
        return NULL;

    Project* project = dynamic_cast<Project*>(projectBase);
    if(!project)
        return NULL;

    QDomElement domOMProblem = domOMCase.firstChildElement("OMProblem");


    Problem* problem = NULL;
    bool ok = true;

    QDomElement domProblem = domOMProblem.firstChildElement(Optimization::className());

    problem = new Optimization(domProblem,project,ok);

    if(!ok)
    {
        delete problem;
        problem = NULL;
    }
    else
    {
        problem->setEntireSavePath(saveFile.absoluteFilePath());
    }

    return problem;
}



Result* OptimizationInterface::loadResult(QFileInfo saveFile,const QDomElement & domOMCase, ProjectBase * projectBase)
{
    if(domOMCase.isNull() || domOMCase.tagName()!="OMCase" )
        return NULL;

    Project* project = dynamic_cast<Project*>(projectBase);
    if(!project)
        return NULL;

    // read problem
    bool ok;
    QDomElement domOMProblem = domOMCase.firstChildElement("OMProblem");
    QDomElement domProblem = domOMProblem.firstChildElement(Optimization::className());
    Optimization optim(domProblem,project,ok);

    if(!ok)
    {
        InfoSender::instance()->send( Info(ListInfo::RESULTFILECORRUPTED,saveFile.filePath()));
        return NULL;
    }

    // create result
    QDomElement domOMResult = domOMCase.firstChildElement("OMResult");
    QDomElement domResult = domOMResult.firstChildElement(OptimResult::className());
    Result* result = new OptimResult(project,domResult,optim,saveFile.absoluteDir(),ok);

    if(!result)
    {
        InfoSender::instance()->send( Info(ListInfo::RESULTFILECORRUPTED,saveFile.filePath()));
        return NULL;
    }

    // attribute problem to result
    if(result)
    {
        // attribute file path to result
        result->setEntireSavePath(saveFile.filePath());
    }

    return result;
}


