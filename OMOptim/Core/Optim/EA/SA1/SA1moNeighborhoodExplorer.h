#ifndef SA1MONEIGHBORHOODEXPLORER_H
#define SA1MONEIGHBORHOODEXPLORER_H



//EO inclusion
#include <eoFunctor.h>

#include <neighborhood/moNeighborhood.h>
#include <eval/moEval.h>
#include <neighborhood/moDummyNeighborhood.h>
#include <eval/moDummyEval.h>
#include <EOStd.h>
/**
 * Explore the neighborhood
 */
template< class Neighbor >
class SA1moNeighborhoodExplorer : public eoUF<typename Neighbor::EOT&, void>
{
public:
    typedef moNeighborhood<Neighbor> Neighborhood;
    typedef typename Neighbor::EOT EOT;
    typedef typename EOT::Fitness Fitness ;

    SA1moNeighborhoodExplorer():neighborhood(dummyNeighborhood), eval(dummyEval), isMoved(false) {}

    /**
     * Constructor with a Neighborhood and evaluation function
     * @param _neighborhood the neighborhood
     * @param _eval the evaluation function
     */
    SA1moNeighborhoodExplorer(Neighborhood& _neighborhood, moEval<Neighbor>& _eval):neighborhood(_neighborhood), eval(_eval), isMoved(false) {}

    /**
     * Init Search parameters
     * @param _solution the solution to explore
     */
    virtual void initParam (EOT& _solution) = 0 ;

    /**
     * Update Search parameters
     * @param _solution the solution to explore
     */
    virtual void updateParam (EOT& _solution) = 0 ;

    /**
     * Test if the exploration continue or not
     * @param _solution the solution to explore
     * @return true if the exploration continue, else return false
     */
    virtual bool isContinue(EOT& _solution) = 0 ;

    /**
     * Move a solution
     * @param _solution the solution to explore
     */
    virtual void move(EOT& _solution) = 0 ;

    /**
     * Test if a solution is accepted
     * @param _solution the solution to explore
     * @return true if the solution is accepted, else return false
     */
    virtual bool accept(EOT& _solution) = 0 ;

    /**
     * Terminate the search
     * @param _solution the solution to explore
     */
    virtual void terminate(EOT& _solution) = 0 ;

    /**
     * Getter of "isMoved"
     * @return true if move is applied
     */





    bool moveApplied() {
        return isMoved;
    }

    /**
     * Setter of "isMoved"
     * @param _isMoved
     */
    void moveApplied(bool _isMoved) {
        isMoved=_isMoved;
    }

protected:
    moDummyNeighborhood<Neighbor> dummyNeighborhood;
    moDummyEval<Neighbor> dummyEval;
    Neighborhood & neighborhood;
    moEval<Neighbor>& eval;
    bool isMoved;
};

#endif // SA1MONEIGHBORHOODEXPLORER_H
