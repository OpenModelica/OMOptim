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

//    if(!function.compare("setOptimParameter",Qt::CaseInsensitive))
//    {
//        // e.g. setOptimParameter(problem,model1.var1,27.2);
//        // note: model1.var1 should already be in optimization overwritedvariables
//        foundFunction = true;
//        // arg is mmo file path
//        if(args.size()!=3)
//            return false;

//        Optimization* optim = dynamic_cast<Optimization*>(_project->findOMCase(args.at(0)));
//        if(!optim)
//            return false;
//        return optim->setOverwritedVariableValue(args.at(1),args.at(2),args.at(3).toDouble());
//    }

    if(!function.compare("setOverwritedVariableValue",Qt::CaseInsensitive))
    {
        // e.g. setOptimParameter(problem,modelName,varName,27.2);
        // note: model1.var1 should already be in optimization overwritedvariables
        foundFunction = true;
        // arg is mmo file path
        if(args.size()!=4)
            return false;

        Optimization* optim = dynamic_cast<Optimization*>(_project->findOMCase(args.at(0)));
        OneSimulation* oneSim = dynamic_cast<OneSimulation*>(_project->findOMCase(args.at(0)));
        if(!optim && !oneSim)
            return false;
        if(optim)
            return optim->setOverwritedVariableValue(args.at(1),args.at(2),args.at(3).toDouble());
        if(oneSim)
            return optim->setOverwritedVariableValue(args.at(1),args.at(2),args.at(3).toDouble());
    }

    return ScriptParserOMOptimBasis::launchFunction(function,args,foundFunction);
}

QString ScriptParserOMOptim::stHelpText()
{
    QString text;

    text += "#List of functions \n \n";
    text += "loadMOFile(\"MoFilePath\")";
    text += "setOptimParameter(problemName,parameterName,parameterValue) #cf. list of optim parameters below \n";
    text += "setOverwritedVariableValue(problemName,modelName,variableName,variableValue) \n";

    text+="\n";
    text += "#List of Optimization parameters \n";
    QStringList optimParamNames = OptimAlgosList::getCommonParametersNames();
    for(int i=0;i<optimParamNames.size();i++)
            text+= optimParamNames.at(i)+"\n";

    return text;
}
