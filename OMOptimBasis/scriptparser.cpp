#include "scriptparser.h"
#include "ProjectBase.h"
#include "InfoSender.h"
#include "MOSettings.h"
#include "MOThreads.h"
#include <QApplication>

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

    return parseFile(text,commands,definitions);
}



bool ScriptParser::parseFile(const QString & text,QStringList &commands,QMap<QString,QString> & definitions)
{
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
            definitions.insert(definitionRegExp.cap(1).toLower(),definitionRegExp.cap(2).toLower()); // keep in lower cases
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

    InfoSender::instance()->send(Info(command,ListInfo::SCRIPT));
    bool result = launchFunction(function,args,foundFunction);
    if(result)
        InfoSender::instance()->send(Info("Ok",ListInfo::SCRIPT));
    else
        InfoSender::instance()->send(Info("Error",ListInfo::SCRIPT));

    return result;
}

bool ScriptParser::executeCommands(QStringList commands)
{
    bool overalResult = true;
    for(int i=0;i<commands.size();i++)
    {
        overalResult = executeCommand(commands.at(i)) && overalResult;
    }
    return overalResult;
}

ScriptFunction ScriptParser::findFunction(QString functionName, int nbArgs, bool &ok)
{
    QStringList functionNames = _functions.keys();
    functionNames = functionNames.filter(functionName,Qt::CaseInsensitive);

    if(functionNames.isEmpty())
    {
        ok = false;
        return ScriptFunction(QString(),QStringList());
    }
    else
    {
        ScriptFunction function = _functions.value(functionNames.first());
        if(nbArgs==function.args.size())
        {
            ok = true;
            return function;
        }
        else
        {
            ok = false;
            return ScriptFunction(QString(),QStringList());
        }
    }
}

QStringList ScriptParser::functionsList()
{
    QStringList result;
    QList<ScriptFunction> functions = _functions.values();
    for(int i=0;i<functions.size();i++)
    {
        result += functions.at(i).toTxt();
    }
    return result;
}

QString ScriptFunction::toTxt() const
{
    bool includesDescription=true;
    QString result = name+"(";
    for(int i=0;i<args.size();i++)
    {
        result+=args.at(i)+",";
    }
    result.remove(QRegExp("[,]$"));
    result +=")";
    if(includesDescription && !description.isEmpty())
        result +="\t#"+description;
    return result;
}

ScriptParserOMOptimBasis::ScriptParserOMOptimBasis(ProjectBase* projectBase)
    :ScriptParser()
{
    _projectBase = projectBase;
    initFunctions();
}


bool ScriptParserOMOptimBasis::launchFunction(QString functionName, QStringList args, bool & foundFunction)
{

    ScriptFunction function = findFunction(functionName,args.size(),foundFunction);
    if(!foundFunction)
        return false;

    if(!_continue)
        return false;

    if(!function.name.compare("loadProject",Qt::CaseInsensitive))
    {
        QString filePath = args.at(0);
        filePath.remove("\"");
        return _projectBase->load(filePath);
    }

    if(!function.name.compare("saveProject",Qt::CaseInsensitive))
    {
        _projectBase->save(true);
        return true;
    }

    if(!function.name.compare("addProblem",Qt::CaseInsensitive))
    {
        _projectBase->addOMCase(args.at(0));
        return true;
    }

    if(!function.name.compare("launchProblem",Qt::CaseInsensitive))
    {
        _problemThreadFinished = false;
        // launch in multi threading but wait until it is finished
        MOThreads::ProblemThread* thread = _projectBase->launchProblem(args.at(0),true);
        if(!thread)
            return false;

        connect(thread,SIGNAL(finished(Problem*,Result*)),this,SLOT(onProblemFinished()));
        connect(_projectBase,SIGNAL(forgetProblems()),this,SLOT(onProblemFinished()));

        while(!_problemThreadFinished && _continue)
        {
            QApplication::processEvents();
        }
        return true;
    }

    if(!function.name.compare("removeAllResults",Qt::CaseInsensitive))
    {
        _projectBase->removeAllResults();
        return true;
    }

    if(!function.name.compare("setSetting",Qt::CaseInsensitive))
    {
        QString settingName = args.at(0);
        QString settingValue = args.at(1);

        return MOSettings::instance()->setValue(settingName,settingValue);
    }

    // if not found
    foundFunction = false;
    return false;
}

void ScriptParserOMOptimBasis::initFunctions()
{
    QStringList names;
    QList<QStringList> args;
    QStringList descriptions;

    names+= "loadProject";
    args += QStringList() << "\"projectFilePath\"";
    descriptions += QString();

    names+= "saveProject";
    args += QStringList() ;
    descriptions += QString();

    names+= "addProblem";
    args += QStringList() << "\"problemFilePath\"";
    descriptions += QString();

    names+= "launchProblem";
    args += QStringList() << "problemName";
    descriptions += QString();

    names+= "removeAllResults";
    args += QStringList() ;
    descriptions += QString();

    names+= "setSetting";
    args += QStringList()  << "settingName" << "settingValue";
    descriptions += QString();

    for(int i=0;i<names.size();i++)
    {
        _functions.insert(names.at(i),ScriptFunction(names.at(i),args.at(i),descriptions.at(i)));
    }
}

QString ScriptParserOMOptimBasis::helpText()
{
    QString text = "#List of functions \n \n";

    QStringList functions = this->functionsList();
    text += functions.join("\n");

    text += "\n \n";
    text+=annexHelpText();

    return text;
}

QString ScriptParserOMOptimBasis::annexHelpText()
{
    QString text = "#Set display \n \n";
    text += "displayMode = console\n";
    text += "#nogui  = do not display gui\n";
    text += "#console = display console\n";
    text += "#gui = display normal gui\n";

    return text;
}

ScriptFunction::ScriptFunction(QString functionName, QStringList argsNames, QString functionDescription)
{
    name = functionName;
    args = argsNames;
    description = functionDescription;
}

void ScriptParserOMOptimBasis::onProblemFinished()
{
    _problemThreadFinished = true;
}
