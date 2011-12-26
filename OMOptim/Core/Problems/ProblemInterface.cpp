#include "ProblemInterface.h"
#include "Problem.h"
#include <QStringList>


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
}

bool ProblemInterfaces::addProblemInterface(ProblemInterface* interface)
{
    QStringList problemNames = interface->problemTypes();
    bool oneAdded = false;
    for(int i=0;i<problemNames.size();i++)
    {
        if(!this->contains(problemNames.at(i)))
        {
            insert(problemNames.at(i),interface);
            //emit modified();
            oneAdded = true;
        }
    }
    return oneAdded;
}

bool ProblemInterfaces::addProblemInterfaces(QList<ProblemInterface*> interfaces)
{
    bool addedSome = false;
    bool allOk = true;

    for(int i=0;i<interfaces.size();i++)
    {
        QStringList problemNames = interfaces.at(i)->problemTypes();

        for(int j=0;j<problemNames.size();j++)
        {
            if(this->contains(problemNames.at(j)))
            {
                allOk=false;

            }
            else
            {
                insert(problemNames.at(j),interfaces.at(i));
                //emit modified();
                addedSome=true;
            }
        }

    }

    return allOk;
}

ProblemInterface* ProblemInterfaces::interfaceOf(Problem* problem)
{
    return QMap<QString,ProblemInterface*>::value(problem->getClassName(),NULL);
}

ProblemInterface* ProblemInterfaces::interfaceOf(QString problemType)
{
    return QMap<QString,ProblemInterface*>::value(problemType,NULL);
}

QList<ProblemInterface*> ProblemInterfaces::uniqueInterfaces()
{
    QList<ProblemInterface*> res = QSet<ProblemInterface*>::fromList(values()).toList();

    return res;
}
