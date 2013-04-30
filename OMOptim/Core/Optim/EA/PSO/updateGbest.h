#ifndef UPDATEGBEST_H
#define UPDATEGBEST_H


#include "eoPop.h"
#include "utils/eoRNG.h"
#include "archive/moeoUnboundedArchive.h"





template <class EOT>

class UpdateGbest
{
 public:
    void operator() (EOT & gbest, const moeoUnboundedArchive<EOT>& arch)
    {
      int i = eo::rng.random(arch.size());

      gbest.doubleVars = (arch[i]).doubleVars;
      gbest.intVars    = (arch[i]).intVars;
      gbest.boolVars   = (arch[i]).boolVars;
      gbest.objectiveVector((arch[i]).objectiveVector());
    }
};





#endif // UPDATEGBEST_H
