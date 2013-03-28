#ifndef SCRIPTPARSER_H
#define SCRIPTPARSER_H

#include <QFile>
#include <QString>
#include <QFileInfo>

class ProjectBase;

class ScriptParser
{
public:
    bool parseFile(QFileInfo fileInfo);
    bool parseCommand(QString command);
    bool parseCommands( QStringList commands);

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
