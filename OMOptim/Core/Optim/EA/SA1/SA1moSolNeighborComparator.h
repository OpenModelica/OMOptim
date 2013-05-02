#ifndef SA1MOSOLNEIGHBORCOMPARATOR_H
#define SA1MOSOLNEIGHBORCOMPARATOR_H

#include <EO.h>
#include <eoFunctor.h>

#include <neighborhood/moNeighbor.h>
#include <comparator/moComparator.h>


/**
 * Comparator of two neighbors
 */
template< class Neighbor >
class SA1moSolNeighborComparator : public moComparator<typename Neighbor::EOT, Neighbor>
{
public:
    typedef typename Neighbor::EOT EOT ;

    /**
     * Compare two neighbors
     * @param _sol the solution
     * @param _neighbor the neighbor
     * @return true if the neighbor is better than sol
     */
    virtual bool operator()( const EOT& _sol, const Neighbor& _neighbor) {
  return (_sol.fitness() < _neighbor.fitness());
    }

    /**
     * Test the equality between two neighbors
     * @param _sol the solution
     * @param _neighbor the neighbor
     * @return true if the neighbor is equal to the solution
     */
    virtual bool equals( const EOT& _sol, const Neighbor& _neighbor) {
  return (_sol.fitness() == _neighbor.fitness());
    }

    /**
     * Return the class Name
     * @return the class name as a std::string
     */
    virtual std::string className() const {
  return "SA1moSolNeighborComparator";
    }
};

#endif // SA1MOSOLNEIGHBORCOMPARATOR_H
