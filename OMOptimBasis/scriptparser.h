#ifndef SCRIPTPARSER_H
#define SCRIPTPARSER_H

#include <QFile>
#include <QString>
#include <QFileInfo>
#include <QMap>


class ProjectBase;

class ScriptParser
{
public:

    static bool parseFile(QFileInfo fileInfo,QStringList &commands,QMap<QString,QString> &options);
    bool executeCommand(QString command);
    bool executeCommands( QStringList commands);

 protected:
    virtual bool launchFunction(QString function, QStringList args, bool & foundFunction)=0;
};

class ScriptParserOMOptimBasis : public ScriptParser
{
public:
    ScriptParserOMOptimBasis(ProjectBase* project);



protected :
    virtual bool launchFunction(QString function, QStringList args, bool & foundFunction);

    ProjectBase* _projectBase;
};

#endif // SCRIPTPARSER_H
