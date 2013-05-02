#include "scriptparseromoptim.h"
#include "Project.h"


ScriptParserOMOptim::ScriptParserOMOptim(Project* project)
    :ScriptParserOMOptimBasis(project)
{
    _project = project;
}

bool ScriptParserOMOptim::launchFunction(QString function, QStringList args, bool & foundFunction)
{
    if(!function.compare("loadMOFile",Qt::CaseInsensitive))
    {
  foundFunction = true;
  // arg is mo file path
  if(args.size()!=1)
      return false;

   _project->loadMoFile(args.at(0));
   return true;
    }

    if(!function.compare("loadModel",Qt::CaseInsensitive))
    {
  foundFunction = true;
  // arg is mmo file path
  if(args.size()!=1)
      return false;

   _project->loadModelPlus(args.at(0),true);
   return true;
    }


    return ScriptParserOMOptimBasis::launchFunction(function,args,foundFunction);
}

