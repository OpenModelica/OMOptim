#include "ProblemInterfaces.h"
#include "Problem.h"
#include "ProblemInterface.h"

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

bool ProblemInterfaces::removeProblemInterface(QString interfaceName)
{
    int nbRemoved = this->remove(interfaceName);
    if(nbRemoved>0)
    {
        return true;
    }
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
