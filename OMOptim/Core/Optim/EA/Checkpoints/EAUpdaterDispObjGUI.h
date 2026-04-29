/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
 * VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the OSMC (Open Source Modelica Consortium)
 * Public License (OSMC-PL) are obtained from OSMC, either from the above
 * address, from the URLs:
 * http://www.openmodelica.org or
 * https://github.com/OpenModelica/ or
 * http://www.ida.liu.se/projects/OpenModelica,
 * and in the OpenModelica distribution.
 *
 * GNU AGPL version 3 is obtained from:
 * https://www.gnu.org/licenses/licenses.html#GPL
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

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
        QString msg = "Current vector [double params, int params, bool params, objectives]\n";
        for (unsigned int i = 0; i < arch.size (); i++)
        {
            // add double parameters
            for(int j=0;j<arch.at(i).doubleVars.size();j++)
            {
                msg+=QString::number(arch.at(i).doubleVars.at(j))+="\t";
            }

            // add int parameters
            for(int j=0;j<arch.at(i).intVars.size();j++)
            {
                msg+=QString::number(arch.at(i).intVars.at(j))+="\t";
            }

            // add bool parameters
            for(int j=0;j<arch.at(i).boolVars.size();j++)
            {
                msg+=QVariant(arch.at(i).boolVars.at(j)).toString()+="\t";
            }

            // add objectives
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
