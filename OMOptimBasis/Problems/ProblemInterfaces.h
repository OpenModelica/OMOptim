#ifndef PROBLEMINTERFACES_H
#define PROBLEMINTERFACES_H


#include <QtCore>
#include <QMap>

class ProblemInterface;
class Problem;
class Result;

class ProblemInterfaces :  public QMap<QString,ProblemInterface*>
{

public :
    bool addProblemInterface(ProblemInterface* itf);
    bool addProblemInterfaces(QList<ProblemInterface*> itfs);
    bool removeProblemInterface(QString problemInterfaceName);

    ProblemInterface* interfaceOf(Problem* problem);
    ProblemInterface* interfaceOf(Result* result);
    ProblemInterface* interfaceOf(QString caseType);

    QList<ProblemInterface*> uniqueInterfaces();
};



#endif // PROBLEMINTERFACES_H
