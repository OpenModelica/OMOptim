#ifndef EAUpdaterDispObjGUI_H_
#define EAUpdaterDispObjGUI_H_

#include <fstream>
#include <string>
#include <eoPop.h>
#include <utils/eoUpdater.h>
#include <archive/moeoArchive.h>
#include "InfoSender.h"

#define MAX_BUFFER_SIZE 1000

/**
 * Display objective vectors in ModOpt Gui every generation.
 */
template < class MOEOT >
class EAUpdaterDispObjGUI : public eoUpdater, public QObject
  {
     
  public:

    /**
     * Ctor
     * @param _arch local archive
     */
    EAUpdaterDispObjGUI (moeoArchive<MOEOT> & _arch) :
        arch(_arch), counter(0)
    {
    }


    /**
     * Saves the fitness of the archive's members into the file
     */
    void operator()()
    {
        QString msg = "Current gen. objective vector :";
        for (unsigned int i = 0; i < arch.size (); i++)
        {
            for(int j=0;j<arch.at(i).objectiveVector().size();j++)
            {
                msg+=QString::number(arch.at(i).objectiveVector().at(j))+="\t";
            }
            msg +="\n";
        }
        InfoSender::instance()->send(Info(msg));
    }

  private:
    /** local archive */
    moeoArchive<MOEOT> & arch;
    /** this variable is set to true if a new file have to be created each time () is called and to false if the file only HAVE to be updated */
    bool count;
    /** counter */
    unsigned int counter;



  };

/**
 * Display objective vectors in ModOpt Gui every generation.
 */
template < class MOEOT >
class EAUpdaterDispObjGUIOneSol : public eoUpdater, public QObject
  {

  public:

    /**
     * Ctor
     * @param _arch local archive
     */
    EAUpdaterDispObjGUIOneSol (MOEOT & _sol) :
        sol(_sol), counter(0)
    {
        }


    /**
     * Saves the fitness of the archive's members into the file
     */
    void operator()()
    {
        QString msg = "Current gen. objective vector :";

        for(int j=0;j<sol.objectiveVector().size();j++)
        {
            msg+=QString::number(sol.objectiveVector().at(j))+="\t";
        }
        msg +="\n";

        InfoSender::instance()->send(Info(msg));
    }

private:
    /** local solution */
    MOEOT & sol;
    /** this variable is set to true if a new file have to be created each time () is called and to false if the file only HAVE to be updated */
    bool count;
    /** counter */
    unsigned int counter;



  };



#endif /*EAUpdaterDispObjGUI_H_*/
