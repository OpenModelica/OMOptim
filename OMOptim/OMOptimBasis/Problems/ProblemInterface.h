#ifndef PROBLEMINTERFACE_H
#define PROBLEMINTERFACE_H


#include <QtPlugin>
#include <QtCore>
#include <QObject>
#include <QDomElement>
#include <QWidget>


class ProjectBase;
class Problem;
class Result;
class MOParameters;

/**
  * ProblemInterface is used to connect problems to OMOptimLib.
  * Every kind of problem (e.g. OneSimulation, Optimization) should have its own interface inheriting abstract class ProblemInterface.
  * It allows to dynamically plug new problem and result types.
  *
  * More precisely, it offers several functions for each problem :
  * - create a new instance of problem or result
  * - save/load those problem/result
  * - build GUI for problem/result
*/

class ProblemInterface : public QObject
{
    Q_OBJECT

public :

    ProblemInterface();
    virtual ~ProblemInterface(){}

    //******************
    // Create instance
    //******************
    virtual Problem* createNewProblem(ProjectBase*,const QStringList modelNames,QString problemType) = 0;

    //******************
    // Information
    //******************
    virtual QStringList problemTypes() = 0;
    virtual QString name() = 0;

    //******************
    // GUI
    //******************
    virtual QWidget* createProblemTab(Problem *,QWidget* parent) = 0;
    virtual QWidget* createResultTab(Result*,QWidget* parent) = 0;

    //******************
    // Save / Load
    //******************
    virtual bool saveProblem(Problem*);
    virtual bool saveResult(Result*);
    virtual Problem* loadProblem(QFileInfo loadedFile,const QDomElement & domOMCase,ProjectBase*) = 0;
    virtual Result* loadResult(QFileInfo loadedFile,const QDomElement & domOMCase,ProjectBase*) = 0;

    //******************
    // Parameters
    //******************
    enum ModelNeeds
    {
        NOMODEL,
        ONEMODEL,
        SEVERALMODELS
    };

    virtual ModelNeeds modelNeeds(QString problemType) = 0;
    virtual void initParameters();
    virtual MOParameters* parameters();

protected :
    MOParameters *_parameters;
 };


#ifdef ISAPLUGIN
    Q_DECLARE_INTERFACE(ProblemInterface,"com.OMOptim.ProblemInterface/0.1")
#endif





#endif // PROBLEMINTERFACE_H
