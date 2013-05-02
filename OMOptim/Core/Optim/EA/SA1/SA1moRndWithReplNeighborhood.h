#ifndef SA1MORNDWITHREPLNEIGHBORHOOD_H
#define SA1MORNDWITHREPLNEIGHBORHOOD_H

#include <neighborhood/moIndexNeighborhood.h>
#include <neighborhood/moRndNeighborhood.h>
#include <utils/eoRNG.h>
#include <utils/eoRealBounds.h>
#include <utils/eoIntBounds.h>


/**
 * A Random With replacement Neighborhood
 */
template< class Neighbor >
class SA1moRndWithReplNeighborhood : public moIndexNeighborhood<Neighbor>, public moRndNeighborhood<Neighbor>
{
public:

    /**
     * Define type of a solution corresponding to Neighbor
     */
    typedef typename Neighbor::EOT EOT;

    using moIndexNeighborhood<Neighbor>::neighborhoodSize;

    /**
     * Constructor
     * @param _neighborhoodSize the size of the neighborhood
     */
    SA1moRndWithReplNeighborhood(unsigned int _neighborhoodSize, std::vector<eoRealInterval> doubleBounds, std::vector<eoIntInterval> intBounds, int nbBool): moIndexNeighborhood<Neighbor>(_neighborhoodSize), _doubleBounds(doubleBounds), _intBounds(intBounds), _nbBool(nbBool){}

    /**
     * Test if it exist a neighbor
     * @param _solution the solution to explore
     * @return true if the neighborhood was not empty
     */
    virtual bool hasNeighbor(EOT& _solution) {
        return neighborhoodSize > 0;
    }



    /**
     * Initialization of the neighborhood
     * @param _solution the solution to explore
     * @param _neighbor the first neighbor
     */
    virtual void init(EOT & _solution, Neighbor & _neighbor) {
        _neighbor.index(rng.random(neighborhoodSize));
        std::vector<eoRealInterval> tmpvar1 = this->getDoubleBounds();
        _neighbor.setDoubleBounds(tmpvar1 );
        std::vector<eoIntInterval> tmpvar2 = this->getIntBounds();
        _neighbor.setIntBounds(tmpvar2);
        _neighbor.setNbBool(this->getNbBool());
    }

    /**
     * Give the next neighbor
     * @param _solution the solution to explore
     * @param _neighbor the next neighbor
     */
    virtual void next(EOT & _solution, Neighbor & _neighbor) {
        _neighbor.index(rng.random(neighborhoodSize));
    }

    /**
     * test if all neighbors are explore or not,if false, there is no neighbor left to explore
     * @param _solution the solution to explore
     * @return true if there is again a neighbor to explore
     */
    virtual bool cont(EOT & _solution) {
        return neighborhoodSize > 0;
    }

    /**
     * Return the class Name
     * @return the class name as a std::string
     */
    virtual std::string className() const {
        return "moRndWithReplNeighborhood";
    }


    std::vector<eoRealInterval> getDoubleBounds() const
    {
        return _doubleBounds;
    }

    std::vector<eoIntInterval> getIntBounds() const
    {
        return _intBounds;
    }

    int getNbBool() const
    {
        return _nbBool;
    }





private:
    std::vector<eoRealInterval> _doubleBounds;
    std::vector<eoIntInterval> _intBounds;
    int _nbBool;
};





#endif // SA1MORNDWITHREPLNEIGHBORHOOD_H
