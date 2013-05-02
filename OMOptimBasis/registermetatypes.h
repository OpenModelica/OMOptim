#ifndef REGISTERMETATYPES_H
#define REGISTERMETATYPES_H

#include <QObject>
#include "ProjectBase.h"
#include "InfoSender.h"

class RegisterMetaTypes
{
public:
    RegisterMetaTypes();

    static void init();
};

class ApplicationQuiter : public QObject
{
    Q_OBJECT
public :
    ApplicationQuiter(ProjectBase* project){_project = project;}

public slots:
    void onApplicationQuit()
    {
       // delete _project;
  InfoSender::destroy();
    }

protected :
    ProjectBase* _project;
};

#endif // REGISTERMETATYPES_H
