#ifndef PSOBREED_H
#define PSOBREED_H
#include "eoBreed.h"
#include "eoIntBounds.h"
#include "eoRealBounds.h"




template<class EOT>
class PSOBreed
{

   public:

    void operator ()(eoPop <EOT> & pop, const eoPop <EOT> & pbest, const EOT & gbest, const std::vector<EOT> & velocity, const int & nbdouble, const int & nbint, const int & nbbool, const std::vector<eoRealInterval> & doubleBounds, const std::vector<eoIntInterval> & intBounds)
    {
        for(int i =0; i < pop.size(); i++)
        {
            for (int j=0; j< nbdouble; j++)
            {
                (pop[i].doubleVars)[j]            +=  (velocity[i].doubleVars)[j];

                // limit conditions
                if((pop[i].doubleVars)[j]   >  doubleBounds[j].maximum())
                    (pop[i].doubleVars)[j]  =  doubleBounds[j].maximum();

                else if ((pop[i].doubleVars)[j]  <  doubleBounds[j].minimum())
                    (pop[i].doubleVars)[j]       =  doubleBounds[j].minimum();

            }

            for(int j=0; j< nbint; j++)
            {
                (pop[i].intVars)[j]  += (velocity[i].intVars)[j];

                if((pop[i].intVars)[j]   >  intBounds[j].maximum())
                    (pop[i].intVars)[j]  =  intBounds[j].maximum();

                else if ((pop[i].intVars)[j]  <  intBounds[j].minimum())
                    (pop[i].intVars)[j]       =  intBounds[j].minimum();


            }

            for(int j=0; j< nbbool; j++)
            {
                (pop[i].boolVars)[j] = (velocity[i].boolVars)[j];
            }

            pop[i].invalidateObjectiveVector();
        }

    }

//void operator ()(eoPop <EOT> & pop1, eoPop <EOT> & pop2)
//{}

};


#endif // PSOBREED_H
