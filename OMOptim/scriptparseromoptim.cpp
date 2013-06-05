#include "scriptparseromoptim.h"
#include "Project.h"
#include "Optimization.h"
#include "MOParameter.h"

ScriptParserOMOptim::ScriptParserOMOptim(Project* project)
    :ScriptParserOMOptimBasis(project)
{
    _project = project;
    initFunctions();
}

bool ScriptParserOMOptim::launchFunction(QString functionName, QStringList args, bool & foundFunction)
{
    ScriptFunction function = findFunction(functionName,args.size(),foundFunction);
    if(!foundFunction)
        return false;

    if(!function.name.compare("loadMOFile",Qt::CaseInsensitive))
    {
         _project->loadMoFile(args.at(0));
         return true;
    }

    if(!function.name.compare("loadModel",Qt::CaseInsensitive))
    {
         _project->loadModelPlus(args.at(0),true);
         return true;
    }

    if(!function.name.compare("setOptimParameter",Qt::CaseInsensitive))
    {
        // e.g. setOptimParameter(problem,parameterName,value);
        // note: model1.var1 should already be in optimization overwritedvariables
        Optimization* optim = dynamic_cast<Optimization*>(_project->findOMCase(args.at(0)));
        if(!optim)
            return false;
        return optim->parameters()->setValue(args.at(1),args.at(2));
    }


    if(!function.name.compare("setOptimAlgo",Qt::CaseInsensitive))
    {
        // e.g. setOptimParameter(problem,parameterName,value);
        // note: model1.var1 should already be in optimization overwritedvariables
        Optimization* optim = dynamic_cast<Optimization*>(_project->findOMCase(args.at(0)));
        if(!optim)
            return false;
        return optim->setCurAlgo(args.at(1));
    }


    if(!function.name.compare("setOverwritedVariableValue",Qt::CaseInsensitive))
    {
        // e.g. setOptimParameter(problem,modelName,varName,27.2);
        // note: model1.var1 should already be in optimization overwritedvariables
        Optimization* optim = dynamic_cast<Optimization*>(_project->findOMCase(args.at(0)));
        OneSimulation* oneSim = dynamic_cast<OneSimulation*>(_project->findOMCase(args.at(0)));
        if(!optim && !oneSim)
            return false;
        if(optim)
            return optim->setOverwritedVariableValue(args.at(1),args.at(2),args.at(3).toDouble());
        if(oneSim)
            return optim->setOverwritedVariableValue(args.at(1),args.at(2),args.at(3).toDouble());
    }

    return ScriptParserOMOptimBasis::launchFunction(functionName,args,foundFunction);
}

QString ScriptParserOMOptim::helpText()
{
    QString text = "#List of functions\n";
    text += this->functionsList().join("\n");
    text+="\n";
    text+="\n";

    text+=ScriptParserOMOptim::annexHelpText();
    return text;
}

QString ScriptParserOMOptim::annexHelpText()
{
    QString text;
    text += "#List of Optimization parameters \n";
    text += "#Parameters for all optimization algorithms\n";
    QStringList optimParamNames = OptimAlgosList::getCommonParametersNames();
    for(int i=0;i<optimParamNames.size();i++)
            text+= optimParamNames.at(i)+"\n";


    text += "#List of Optimization algorithms \n";
    QMap<QString,QStringList> optimParameters = OptimAlgosList::getAlgosParametersNames();
    for(int i=0;i<optimParameters.keys().size();i++)
            text+= optimParameters.keys().at(i)+"\n";


    text += "#List of optimization algorithms parameters\n";
    QStringList params;
    for(int i=0;i<optimParameters.keys().size();i++)
    {
        text+="\n Algorithm : "+optimParameters.keys().at(i)+"\n";
        params = optimParameters.values().at(i);
        for(int j=0;j<params.size();j++)
            text+= params.at(j)+"\n";
    }


    return text;
}

void ScriptParserOMOptim::initFunctions()
{
    ScriptParserOMOptimBasis::initFunctions();

    QStringList names;
    QList<QStringList> args;
    QStringList descriptions;

    names+= "loadMOFile";
    args += QStringList() << "\"moFilePath\"";
    descriptions += QString();

    names+= "loadModel";
    args += QStringList() << "\"mmoFilePath\"";
    descriptions += QString();

    names+= "setOptimParameter";
    args += QStringList() << "problemName" << "parameterName" << "parameterValue";
    descriptions += QString();

    names+= "setOptimAlgo";
    args += QStringList() << "problemName" << "optimizationAlgo";
    descriptions += QString();

    names+= "setOverwritedVariableValue";
    args += QStringList()  << "problemName" << "modelName"<<"variableName"<<"variableValue";
    descriptions += QString();

    for(int i=0;i<names.size();i++)
    {
        _functions.insert(names.at(i),ScriptFunction(names.at(i),args.at(i),descriptions.at(i)));
    }
}
