#ifndef UPDATEVELOCITY_H
#define UPDATEVELOCITY_H

#include <vector>
#include "eoPop.h"
#include <utils/eoRNG.h>
#include <math.h>







template <class EOT>

class UpdateVelocity
{
  public:
    void operator()(std::vector <EOT> & velocity, const eoPop <EOT> & _pop, const eoPop <EOT> & pbest, const EOT & gbest, const double & inertia, const double &  alpha, const double &  betta)
    {

        int rVarS = (_pop[0].doubleVars).size();
        int iVarS = (_pop[0].intVars).size();
        int bVarS = (_pop[0].boolVars).size();



        for(int i = 0; i < velocity.size(); i++)
        {
            // for Real Variables
            for(int j=0; j < rVarS; j++)
                (velocity[i].doubleVars)[j] = inertia * (velocity[i].doubleVars)[j] + eo::rng.uniform() * betta * ( (pbest[i].doubleVars)[j] - (_pop[i].doubleVars)[j] )
                    + eo::rng.uniform() * alpha * ( (gbest.doubleVars)[j] - (_pop[i].doubleVars)[j] );

            // for int Variables
            for(int j=0; j < iVarS; j++)
                (velocity[i].intVars)[j] = (int) ceil(inertia * (velocity[i].intVars)[j] + eo::rng.uniform() * betta * ( (pbest[i].intVars)[j] - (_pop[i].intVars)[j] )
                                                      + eo::rng.uniform() * alpha * ( (gbest.intVars)[j] - (_pop[i].intVars)[j] ) );
            // for bool Variables
            for(int j=0; j < bVarS; j++)
            {

                double r1 = eo::rng.uniform();
                double r2 = eo::rng.uniform();

                if((_pop[i].boolVars)[j])
                {

                    (velocity[i].boolVars)[j] = ((inertia * int((velocity[i].boolVars)[j]) + r1 * betta * ( int((pbest[i].boolVars)[j]) - int((_pop[i].boolVars)[j]) )
                                                  + r2 * alpha * ( int((gbest.boolVars)[j]) - int((_pop[i].boolVars)[j]) ) + r1 + r2)/(inertia + r1 + r2)) > 0.5;
                }
                else
                {
                    (velocity[i].boolVars)[j] = ((inertia * int((velocity[i].boolVars)[j]) + r1 * betta * ( int((pbest[i].boolVars)[j]) - int((_pop[i].boolVars)[j]) )
                                                  + r2 * alpha * ( int((gbest.boolVars)[j]) - int((_pop[i].boolVars)[j]) ) )/(inertia + r1 + r2)) > 0.5;

                }

            }
        }



    }




};


#endif // UPDATEVELOCITY_H
