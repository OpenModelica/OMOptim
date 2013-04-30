#ifndef MYEOARCHIVEUPDATER_H_
#define MYEOARCHIVEUPDATER_H_

#include <eoPop.h>
#include <utils/eoUpdater.h>
#include <archive/moeoArchive.h>

/**
 * This class allows to update the archive at each generation with newly found non-dominated solutions.
 */
template < class MOEOT >
class myEOArchiveUpdater : public eoUpdater
  {
  public:

    /**
     * Ctor
     * @param _arch an archive of non-dominated solutions
     * @param _sol the main solution
     */
    myEOArchiveUpdater(moeoArchive < MOEOT > & _arch,MOEOT & _sol) : arch(_arch), sol(_sol)
    {}


    /**
     * Updates the archive with newly found non-dominated solutions contained in the main population
     */
    void operator()()
    {
      arch(sol);
    }


  private:

    /** the archive of non-dominated solutions */
    moeoArchive < MOEOT > & arch;
    /** the main solution */
    const MOEOT  & sol;

  };

#endif /*MOEOARCHIVEUPDATER_H_*/
