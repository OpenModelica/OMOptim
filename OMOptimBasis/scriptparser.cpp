#include "scriptparser.h"
#include "ProjectBase.h"
#include "InfoSender.h"





bool ScriptParser::parseFile(QFileInfo fileInfo,QStringList &commands,QMap<QString,QString> & definitions)
{
    commands.clear();
    definitions.clear();

    QFile file(fileInfo.absoluteFilePath());
    if(!file.exists())
        return false;

    file.open(QIODevice::ReadOnly);
    QTextStream tsfront( &file );
    QString text = tsfront.readAll();
    file.close();

    QStringList lines = text.split(QRegExp("[\\n|;]"),QString::SkipEmptyParts);
    // remove commented lines
    QRegExp commentRegExp("^[#|\\s]+[\\s|\\S]*$");
    int iC = lines.indexOf(commentRegExp);
    while(iC!=-1)
    {
        lines.removeAt(iC);
        iC = lines.indexOf(commentRegExp);
    }

    QString line;
    QRegExp commandRegExp("^[\\s]*([\\S]*\\(.*\\)).*$");
    QRegExp definitionRegExp("^[\\s]*([\\S]+)[\\s]*=[\\s]*([\\S]*).*$");
    for(int i=0;i<lines.size();i++)
    {
        line = lines.at(i);
        // if line is a command
        if(line.contains(commandRegExp))
            commands.push_back(commandRegExp.cap(1));
        else if(line.contains(definitionRegExp))
            definitions.insert(definitionRegExp.cap(1),definitionRegExp.cap(2));
        else InfoSender::instance()->sendWarning("Unknown command: "+line);
    }
    return true;
}

bool ScriptParser::executeCommand(QString command)
{
    // if empty
    if(command.isEmpty())
        return true;

    // if several commands
    QStringList commands = command.split(";",QString::SkipEmptyParts);
    if(commands.size()>1)
        return executeCommands(commands);

    // apply one command
    QRegExp regExp("([\\S]+)\\((.*)\\)");
    command.indexOf(regExp);

    if(regExp.capturedTexts().size()!=3)
    {
        InfoSender::instance()->sendWarning("Unable to parse command: "+command);
        return false;
    }

    QString function = regExp.cap(1);
    QString arg = regExp.cap(2);
    QStringList args = arg.split(",",QString::SkipEmptyParts);

    bool foundFunction;
    return launchFunction(function,args,foundFunction);
}

bool ScriptParser::executeCommands(QStringList commands)
{
    bool overalResult = true;
    for(int i=0;i<commands.size();i++)
    {
        overalResult = overalResult & executeCommand(commands.at(i));
    }
    return overalResult;
}

ScriptParserOMOptimBasis::ScriptParserOMOptimBasis(ProjectBase* projectBase)
{
    _projectBase = projectBase;
}


bool ScriptParserOMOptimBasis::launchFunction(QString function, QStringList args, bool & foundFunction)
{

    if(!function.compare("loadProject",Qt::CaseInsensitive))
    {
        foundFunction = true;
        // arg is project file path
        if(args.size()!=1)
            return false;

        return _projectBase->load(args.at(0));
    }

    if(!function.compare("saveProject",Qt::CaseInsensitive))
    {
        foundFunction = true;
        // arg is project file path
        if(args.size()!=0)
            return false;

        _projectBase->save(true);
        return true;
    }

    if(!function.compare("addProblem",Qt::CaseInsensitive))
    {
        foundFunction = true;
        // arg is mpb file path
        if(args.size()!=1)
            return false;

        _projectBase->addOMCase(args.at(0));
        return true;
    }

    if(!function.compare("launchProblem",Qt::CaseInsensitive))
    {
        foundFunction = true;
        // arg is mpb file path
        if(args.size()!=1)
            return false;

        _projectBase->launchProblem(args.at(0),false);
        // do not use multithreading while scripting
        return true;
    }

    if(!function.compare("removeAllResults",Qt::CaseInsensitive))
    {
        foundFunction = true;
        // arg is mpb file path
        if(args.size()!=0)
            return false;

        _projectBase->removeAllResults();
        // do not use multithreading while scripting
        return true;
    }


    // if not found
    foundFunction = false;
    return false;
}


