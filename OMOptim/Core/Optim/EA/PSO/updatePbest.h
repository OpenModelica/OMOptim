#ifndef UPDATEPBEST_H
#define UPDATEPBEST_H

#include "eoPop.h"
#include "comparator/moeoParetoObjectiveVectorComparator.h"
#include <utils/eoRNG.h>







template <class EOT>
class UpdatePbest
{
public:
    typedef typename EOT::ObjectiveVector ObjectiveVector;

    void operator() (eoPop <EOT> & pbest, const eoPop <EOT> & _pop, moeoParetoObjectiveVectorComparator<ObjectiveVector> & pareto)
    {
        for(int i =0; i< _pop.size(); i++)
        {
            if (pareto(pbest[i].objectiveVector(), _pop[i].objectiveVector()))
            {
                pbest[i].doubleVars=_pop[i].doubleVars;
                pbest[i].intVars=_pop[i].intVars;
                pbest[i].boolVars=_pop[i].boolVars;
                pbest[i].objectiveVector(_pop[i].objectiveVector());
            }
            else if(! pareto(_pop[i].objectiveVector(), pbest[i].objectiveVector()))
            {
                int j = eo::rng.flip();

                if(j)
                {
                    pbest[i].doubleVars=_pop[i].doubleVars;
                    pbest[i].intVars=_pop[i].intVars;
                    pbest[i].boolVars=_pop[i].boolVars;
                    pbest[i].objectiveVector(_pop[i].objectiveVector());
                }

            }

        }


    }




};


#endif // UPDATEPBEST_H
