#include "onesimulationinterface.h"

#include "TabOneSim.h"
#include "TabResOneSim.h"

Problem* OneSimulationInterface::createNewProblem(ProjectBase* projectBase,const QStringList modelsList,QString problemType)
{
    Q_ASSERT(problemType==OneSimulation::className());

    if(modelsList.size()!=1)
    {
        InfoSender::instance()->send(Info("Model for one simulation problem not defined",ListInfo::ERROR2));
        return NULL;
    }
    else
    {
        Project* project = dynamic_cast<Project*>(projectBase);
        if(!project)
            return NULL;
        else
            return new OneSimulation(project,project->modModelPlus(modelsList.at(0)));
    }
}


QWidget* OneSimulationInterface::createProblemTab(Problem * problem,QWidget* parent)
{
    OneSimulation* oneSim = dynamic_cast<OneSimulation*>(problem);
    return new TabOneSim(oneSim,parent);
}

QWidget* OneSimulationInterface::createResultTab(Result* result,QWidget* parent)
{
    OneSimResult* oneSimResult = dynamic_cast<OneSimResult*>(result);
    return new TabResOneSim(oneSimResult,parent);
}


Problem* OneSimulationInterface::loadProblem(QFileInfo saveFile,const QDomElement & domOMCase, ProjectBase * projectBase)
{
    if(domOMCase.isNull() || domOMCase.tagName()!="OMCase" )
        return NULL;

    Project* project = dynamic_cast<Project*>(projectBase);
    if(!project)
        return NULL;

   QDomElement domOMProblem = domOMCase.firstChildElement("OMProblem");


    Problem* problem = NULL;
    bool ok = true;

    QDomElement domProblem = domOMProblem.firstChildElement(OneSimulation::className());

    problem = new OneSimulation(domProblem,project,ok);

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



Result* OneSimulationInterface::loadResult(QFileInfo saveFile,const QDomElement & domOMCase, ProjectBase * projectBase)
{
    if(domOMCase.isNull() || domOMCase.tagName()!="OMCase" )
        return NULL;


    Project* project = dynamic_cast<Project*>(projectBase);
    if(!project)
        return NULL;


    // read problem
    bool ok;
    QDomElement domOMProblem = domOMCase.firstChildElement("OMProblem");
    QDomElement domProblem = domOMProblem.firstChildElement(OneSimulation::className());
    OneSimulation oneSim(domProblem,project,ok);

    if(!ok)
    {
        InfoSender::instance()->send( Info(ListInfo::RESULTFILECORRUPTED,saveFile.filePath()));
        return NULL;
    }

    // create result
    QDomElement domOMResult = domOMCase.firstChildElement("OMResult");
    QDomElement domResult = domOMResult.firstChildElement(OneSimResult::className());
    Result* result = new OneSimResult(project,domResult,oneSim,ok);

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
