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
