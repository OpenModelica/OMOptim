#include "onesimulationinterface.h"

#include "TabOneSim.h"
#include "TabResOneSim.h"

Problem* OneSimulationInterface::createNewProblem(Project* project,const QList<ModModelPlus*> & modModelPlusList,QString problemType)
{
    Q_ASSERT(problemType==OneSimulation::className());

    if(modModelPlusList.size()!=1)
    {
        infoSender.send(Info("Model for one simulation problem not defined",ListInfo::ERROR2));
        return NULL;
    }
    else
        return new OneSimulation(project,modModelPlusList.at(0));
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


Problem* OneSimulationInterface::loadProblem(QFileInfo saveFile,const QDomElement & domOMCase, Project * project)
{
    if(domOMCase.isNull() || domOMCase.tagName()!="OMCase" )
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



Result* OneSimulationInterface::loadResult(QFileInfo saveFile,const QDomElement & domOMCase, Project * project)
{
    if(domOMCase.isNull() || domOMCase.tagName()!="OMCase" )
        return NULL;

    // read problem
    bool ok;
    QDomElement domOMProblem = domOMCase.firstChildElement("OMProblem");
    QDomElement domProblem = domOMProblem.firstChildElement(OneSimulation::className());
    OneSimulation oneSim(domProblem,project,ok);

    if(!ok)
    {
        infoSender.send( Info(ListInfo::RESULTFILECORRUPTED,saveFile.filePath()));
        return NULL;
    }

    // create result
    QDomElement domOMResult = domOMCase.firstChildElement("OMResult");
    QDomElement domResult = domOMResult.firstChildElement(OneSimResult::className());
    Result* result = new OneSimResult(project,domResult,oneSim,ok);

    if(!result)
    {
        infoSender.send( Info(ListInfo::RESULTFILECORRUPTED,saveFile.filePath()));
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
