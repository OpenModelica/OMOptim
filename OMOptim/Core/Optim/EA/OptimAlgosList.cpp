#include "OptimAlgosList.h"
#include "OptimAlgo.h"
#include "NSGA2.h"
#include "SPEA2.h"
#include "SPEA2Adapt.h"
#include "SA1.h"
#include "PSO.h"


OptimAlgo* OptimAlgosList::getNewAlgo(Project* project,Problem* problem,OptimAlgosList::Type iAlgo)
{
    switch(iAlgo)
    {
    case OptimAlgosList::iNSGA2:
        return new NSGA2(project,problem);
    case OptimAlgosList::iSPEA2:
        return new SPEA2(project,problem);
    case OptimAlgosList::iSPEA2Adapt:
        return new SPEA2Adapt(project,problem);
    case OptimAlgosList::iSA1:
        return new SA1(project,problem);
    case OptimAlgosList::iPSO:
        return new PSO(project,problem);
    default:
        //Problem
        return NULL;
    }
}

QStringList OptimAlgosList::getCommonParametersNames()
{
    QSet<QString> names;
    QStringList curNames;
    for(int i=0;i<OptimAlgosList::nbAlgos;i++)
    {
        OptimAlgo* newAlgo;
        newAlgo = getNewAlgo(NULL,NULL,(OptimAlgosList::Type)i);

        curNames = newAlgo->parameters()->getItemNames();
        if(i==0)
        {
            QSet<QString> set(curNames.begin(), curNames.end());
            names = set;
        }
        else
        {
            QSet<QString> set(curNames.begin(), curNames.end());
            names = names.intersect(set);
        }
        delete newAlgo;
    }
    return names.values();
}

QMap<QString,QStringList> OptimAlgosList::getAlgosParametersNames()
{
    QMap<QString,QStringList> result;
    QStringList curNames;
    for(int i=0;i<OptimAlgosList::nbAlgos;i++)
    {
        OptimAlgo* newAlgo;
        newAlgo = getNewAlgo(NULL,NULL,(OptimAlgosList::Type)i);

        curNames = newAlgo->parameters()->getItemNames();

        result.insert(newAlgo->name(),curNames);
        delete newAlgo;
    }
    return result;
}

QStringList OptimAlgosList::getAlgoNames()
{
    QStringList names;
    for(int i=0;i<OptimAlgosList::nbAlgos;i++)
    {
        OptimAlgo* newAlgo;
        newAlgo = getNewAlgo(NULL,NULL,(OptimAlgosList::Type)i);

        names += newAlgo->name();
        delete newAlgo;
    }
    return names;
}
