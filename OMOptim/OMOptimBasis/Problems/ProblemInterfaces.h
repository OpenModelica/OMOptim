#ifndef PROBLEMINTERFACES_H
#define PROBLEMINTERFACES_H


#include <QtCore>

#include "ProblemInterface.h"

class ProblemInterfaces :  public QMap<QString,ProblemInterface*>
{

public :
    bool addProblemInterface(ProblemInterface* interface);
    bool addProblemInterfaces(QList<ProblemInterface*> interfaces);
    bool removeProblemInterface(QString problemInterfaceName);

    ProblemInterface* interfaceOf(Problem* problem);
    ProblemInterface* interfaceOf(QString problemType);

    QList<ProblemInterface*> uniqueInterfaces();
};



#endif // PROBLEMINTERFACES_H
