#ifndef PROBLEMINTERFACE_H
#define PROBLEMINTERFACE_H

#include <QtPlugin>
#include <QString>
#include <QMap>
#include <QFileInfo>
#include <QDomElement>


class ProjectBase;
class MOTabBase;
class Result;
class Problem;
class MOParameters;

/**
  * @brief ProblemInterface is used to connect problems to OMOptim.
  * ProblemInterface is used to connect problems to OMOptim. Each kind of problem
  * (e.g. OneSimulation, Optimization) should have its own interface inheriting abstract class ProblemInterface.
  * It allows to dynamically plug new problem and result types.
  *
  * ProblemInterface is more precisely used to create new problem instances and to get relevant GUI.
*/
class ProblemInterface : public QObject
{
public :
    enum ModelNeeds
    {
        NOMODEL,
        ONEMODEL,
        SEVERALMODELS
    };

    ProblemInterface();
    virtual ~ProblemInterface(){}

    virtual Problem* createNewProblem(ProjectBase*,const QStringList modelNames,QString problemType) = 0;

    virtual QWidget* createProblemTab(Problem *,QWidget* parent) = 0;
    virtual QWidget* createResultTab(Result*,QWidget* parent) = 0;
    virtual ModelNeeds modelNeeds(QString problemType) = 0;
    virtual QStringList problemTypes() = 0;
    virtual QString name() = 0;


    virtual bool saveProblem(Problem*);
    virtual bool saveResult(Result*);

    virtual Problem* loadProblem(QFileInfo loadedFile,const QDomElement & domOMCase,ProjectBase*) = 0;
    virtual Result* loadResult(QFileInfo loadedFile,const QDomElement & domOMCase,ProjectBase*) = 0;


    // parameters
    virtual void initParameters();
    virtual MOParameters* parameters();

protected :
    MOParameters *_parameters;
 };

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


Q_DECLARE_INTERFACE(ProblemInterface,"com.OMOptim.ProblemInterface/0.1")






#endif // PROBLEMINTERFACE_H
