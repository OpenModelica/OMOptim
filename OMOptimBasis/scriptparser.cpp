#include "scriptparser.h"
#include "ProjectBase.h"
#include "InfoSender.h"





bool ScriptParser::parseFile(QFileInfo fileInfo)
{
    QFile file(fileInfo.absoluteFilePath());
    if(!file.exists())
  return false;

    file.open(QIODevice::ReadOnly);
    QTextStream tsfront( &file );
    QString text = tsfront.readAll();
    file.close();

    QStringList commands = text.split(QRegExp("[\\n|;]"),QString::SkipEmptyParts);
    return parseCommands(commands);
}

bool ScriptParser::parseCommand(QString command)
{
    // if empty
    if(command.isEmpty())
  return true;

    // if several commands
    QStringList commands = command.split(";",QString::SkipEmptyParts);
    if(commands.size()>1)
  return parseCommands(commands);

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

bool ScriptParser::parseCommands(QStringList commands)
{
    bool overalResult = true;
    for(int i=0;i<commands.size();i++)
    {
  overalResult = overalResult & parseCommand(commands.at(i));
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


