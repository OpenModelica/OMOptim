#ifndef SA1FITNESSASSIGNEMENT_H
#define SA1FITNESSASSIGNEMENT_H


#include <EOStd.h>



template< class EOT, class MEOEFitness = double >
class SA1FitnessAssignement
{
public:
    void operator() (EOT & sol)
    {
        MEOEFitness fit = 0.0;
        EOStdObjectiveVector objVec = sol.objectiveVector();

        for(int i=0; i< (EOStdObjectiveVector::nObjectives()); i++)
           {
            if(EOStdObjectiveVector::minimizing(i))
              fit +=  (-(sol.objectiveVector()).at(i));
            else
              fit += (sol.objectiveVector()).at(i);
           }
        sol.fitness(fit);
    };


};

#endif // SA1FITNESSASSIGNEMENT_H
