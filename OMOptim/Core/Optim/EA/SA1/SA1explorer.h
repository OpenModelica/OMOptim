#ifndef SA1EXPLORER_H
#define SA1EXPLORER_H

#include <cstdlib>

#include <explorer/moNeighborhoodExplorer.h>
#include <SA1moSolNeighborComparator.h>
#include <coolingSchedule/moCoolingSchedule.h>
#include <neighborhood/moNeighborhood.h>
#include <SA1FitnessAssignement.h>
#include <EOStd.h>
#include <SA1moNeighborhoodExplorer.h>
#include <SA1Archive.h>
#include <utils/eoRNG.h>

/**
 * Explorer for the Simulated Annealing
 * Only the symetric case is considered when Q(x,y) = Q(y,x)
 * Fitness must be > 0
 *
 */
template< class Neighbor >
class SA1explorer : public moNeighborhoodExplorer<Neighbor>
{
public:
    typedef typename Neighbor::EOT EOT ;
    typedef moNeighborhood<Neighbor> Neighborhood ;
    typedef typename EOT::ObjectiveVector ObjectiveVector;

    using moNeighborhoodExplorer<Neighbor>::neighborhood;
    using moNeighborhoodExplorer<Neighbor>::eval;

    /**
     * Constructor
     * @param _neighborhood the neighborhood
     * @param _eval the evaluation function
     * @param _solNeighborComparator a solution vs neighbor comparator
     * @param _coolingSchedule the cooling schedule
     */
    SA1explorer(Neighborhood& _neighborhood, moEval<Neighbor>& _eval, SA1moSolNeighborComparator<Neighbor>& _solNeighborComparator, moCoolingSchedule<EOT>& _coolingSchedule) : moNeighborhoodExplorer<Neighbor>(_neighborhood, _eval), solNeighborComparator(_solNeighborComparator), coolingSchedule(_coolingSchedule) {
        isAccept = false;

        if (!neighborhood.isRandom()) {
            std::cout << "moSAexplorer::Warning -> the neighborhood used is not random" << std::endl;
        }
        iter =0; //for debugging issues: it's the number of iterations
    }

    /**
     * Destructor
     */
    ~SA1explorer() {
    }

    /**
     * initialization of the initial temperature
     * @param _solution the solution
     */
    virtual void initParam(EOT & _solution) {
        temperature = coolingSchedule.init(_solution);
        isAccept = true;
        fitnessAssignement(_solution);
        arch(_solution);
    };

    /**
     * decrease the temperature if necessary
     * @param _solution unused solution
     */
    virtual void updateParam(EOT & _solution) {
        coolingSchedule.update(temperature);
    };

    /**
     * terminate: NOTHING TO DO
     * @param _solution unused solution
     */
    virtual void terminate(EOT & _solution) {};

    /**
     * Explore one random solution in the neighborhood
     * @param _solution the solution
     */
    virtual void operator()(EOT & _solution) {
        //Test if _solution has a Neighbor
        if (neighborhood.hasNeighbor(_solution)) {
            //init on the first neighbor: supposed to be random solution in the neighborhood
            neighborhood.init(_solution, current);

            //eval the _solution moved with the neighbor and stock the result in the neighbor
            eval(_solution, current);
        }
        else {
            //if _solution hasn't neighbor,
            isAccept=false;
        }
    };

    /**
     * continue if the temperature is not too low
     * @param _solution the solution
     * @return true if the criteria from the cooling schedule is true
     */
    virtual bool isContinue(EOT & _solution) {
        return coolingSchedule(temperature);
    };

    /**
     * move the solution to the accepted solution
     * @param _solution the solution to move
     */
    virtual void move(EOT & _solution) {
        //move the solution
        _solution = current.getTmp();
    };

    /**
     * acceptance criterion according to the boltzmann criterion
     * @param _solution the solution
     * @return true if better neighbor or rnd < exp(delta f / T)
     */
    virtual bool accept(EOT & _solution) {
        double alpha=0.0;
        double fit1, fit2;

        iter++;
        if (neighborhood.hasNeighbor(_solution))
        {

            if(arch(current.getTmp()))
            {
                isAccept = true;
                fitnessAssignement(current.getTmp());
            }
            else
            {
            if (paretoComparator(_solution.objectiveVector(), current.objectiveVector()))
            {
                isAccept = true;
                fitnessAssignement(current.getTmp());
            }
            else
            {

                fitnessAssignement(current.getTmp());
                fit1=(double) current.fitness();
                fit2=(double)_solution.fitness();
                if (fit1 < fit2) // this is a maximization
                    alpha = exp((fit1 - fit2) / temperature );
                else // this is a minimization
                    alpha = exp((fit2 - fit1) / temperature );
                isAccept = (rng.uniform() < alpha) ;
            }
           }
        }
        return isAccept;
    };




    /**
     * Getter
     * @return the temperature
     */
    double getTemperature() {
        return temperature;
    }


private:
    // comparator betwenn solution and neighbor
    SA1moSolNeighborComparator<Neighbor>& solNeighborComparator;
    moeoParetoObjectiveVectorComparator < ObjectiveVector > paretoComparator;
    moCoolingSchedule<EOT>& coolingSchedule;
    SA1FitnessAssignement<EOT> fitnessAssignement;
    double temperature;
    int iter;   //for debugging issues: it's the number of iterations
    //Pointer on the best and the current neighbor
    Neighbor current;

    // true if the move is accepted
    bool isAccept ;
};



#endif // SA1EXPLORER_H
