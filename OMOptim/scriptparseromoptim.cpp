#include "scriptparseromoptim.h"
#include "Project.h"
#include "Optimization.h"

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

    if(!function.compare("setOptimParameter",Qt::CaseInsensitive))
    {
        // e.g. setOptimParameter(problem,model1.var1,27.2);
        // note: model1.var1 should already be in optimization overwritedvariables
        foundFunction = true;
        // arg is mmo file path
        if(args.size()!=3)
            return false;

        Optimization* optim = dynamic_cast<Optimization*>(_project->findOMCase(args.at(0)));
        if(!optim)
            return false;
        return optim->setOverwritedVariableValue(args.at(1),args.at(2).toDouble());
    }

    return ScriptParserOMOptimBasis::launchFunction(function,args,foundFunction);
}

