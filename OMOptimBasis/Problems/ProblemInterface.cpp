#include <QStringList>
#include <QSet>

#include "ProblemInterface.h"
#include "Problem.h"
#include "Result.h"
#include "MOParameter.h"



ProblemInterface::ProblemInterface()
{
    initParameters();
}

bool ProblemInterface::saveProblem(Problem* problem)
{
    // Root element
    QDomDocument doc("OMCase");
    QDomElement domOMCase = doc.createElement("OMCase");
    doc.appendChild(domOMCase);

    QDomElement problemRoot = doc.createElement("OMProblem");
    QDomElement problemEl = problem->toXmlData(doc);
    domOMCase.appendChild(problemRoot);
    problemRoot.appendChild(problemEl);

    // Writing to file
    if(!problem->saveFileName().isEmpty())
    {
        QFile file(problem->entireSavePath());
        QFileInfo fileInfo(problem->entireSavePath());
        QDir dir = fileInfo.absoluteDir();
        dir.mkpath(dir.absolutePath());

        if(file.exists())
        {
            file.remove();
        }
        file.open(QIODevice::WriteOnly);
        QTextStream ts( &file );
        ts << doc.toString();
        file.close();
    }

    problem->setIsSaved(true);

	return true;
}

bool ProblemInterface::saveResult(Result* result)
{
    // Root element
    QDomDocument doc("OMCase");
    QDomElement domOMCase = doc.createElement("OMCase");
    doc.appendChild(domOMCase);

    // OMCase attributes
    domOMCase.setAttribute("IsAResult","true");
    domOMCase.setAttribute("ProblemType",result->problem()->getClassName());

    //*********************
    // Problem definition
    //*********************
    Problem* problem = result->problem();
    QDomElement problemRoot = doc.createElement("OMProblem");
    QDomElement problemEl = problem->toXmlData(doc);
    domOMCase.appendChild(problemRoot);
    problemRoot.appendChild(problemEl);


    //*********************
    // Result definition
    //*********************
    QDomElement resultRoot = doc.createElement("OMResult");
    QDomElement resultEl = result->toXmlData(doc);
    resultRoot.appendChild(resultEl);
    domOMCase.appendChild(resultRoot);

    // Writing to file
    if(!result->saveFileName().isEmpty())
    {
        QFile file(result->entireSavePath());
        QFileInfo fileInfo(problem->entireSavePath());
        QDir dir = fileInfo.absoluteDir();
        dir.mkpath(dir.absolutePath());

        if(file.exists())
        {
            file.remove();
        }
        file.open(QIODevice::WriteOnly);
        QTextStream ts( &file );
        ts << doc.toString();
        file.close();
    }

    result->setIsSaved(true);

	return true;
}

void ProblemInterface::initParameters()
{
    _parameters = new MOParameters();
}

MOParameters* ProblemInterface::parameters()
{
    return _parameters;
}


