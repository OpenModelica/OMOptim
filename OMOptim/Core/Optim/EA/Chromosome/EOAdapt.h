// $Id$
/**
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR 
 * THIS OSMC PUBLIC LICENSE (OSMC-PL). 
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE
 * OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3, ACCORDING TO RECIPIENTS CHOICE. 
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from OSMC, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or  
 * http://www.openmodelica.org, and in the OpenModelica distribution. 
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)

     @file EOAdapt.h
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version 0.9 

  */
#ifndef _EOADAPT1_H
#define _EOADAPT1_H

#include <stdio.h>
#include <eo>
#include <moeo>
#include <es/eoRealInitBounded.h>
#include <es/eoRealOp.h>




// objective vector of real values
typedef moeoRealObjectiveVector < moeoObjectiveVectorTraits > EOAdapt1ObjectiveVector;



// multi-objective evolving object for the Sch1 problem
class EOAdapt : public moeoVector < EOAdapt1ObjectiveVector,double>
{

public:
    /**
    * class name
    */
    std::string className(){return "EOAdapt";};

    EOAdapt()
    {
    }

    void printOn(std::ostream& _os) const
    {

        // first write size informations
        _os << objectiveVector().size() << ' ';
        _os << objectiveVector() << ' ';


        // write the fitness
        //moeoVector<EOAdapt1ObjectiveVector,double>::printOn(_os);
    //    _os << ' ';

        // START Code of default output 
        _os << doubleVars.size() << ' ' ;
        for (unsigned i=0; i<doubleVars.size(); i++)
            _os << doubleVars[i] << ' ' ;

        _os << intVars.size() << ' ' ;
        for (unsigned i=0; i<intVars.size(); i++)
            _os << intVars[i] << ' ' ;

        _os << boolVars.size() << ' ' ;
        for (unsigned i=0; i<boolVars.size(); i++)
            _os << boolVars[i] << ' ' ;
        

        _os << dblStdDev.size() << ' ' ;
        for (unsigned i=0; i<dblStdDev.size(); i++)
            _os << dblStdDev[i] << ' ' ;

        _os << intProbMut;

        _os << savedVars.size() << ' ' ;
        for (unsigned i=0; i<savedVars.size(); i++)
            _os << savedVars[i] << ' ' ;
        
    }

    void readFrom(std::istream& _is)
    {
        // START Code of input
        unsigned doubleSize,intSize,boolSize,nbObj, nbStdDev, savedVarsSize;
        double tmpDouble;
        int tmpInt;
        bool tmpBool;



        //read nbobjectives first
        _is >> nbObj;

        // read and store  obj values
        //moeoVector<EOAdapt1ObjectiveVector,double>::readFrom(_is);
        
        EOAdapt1ObjectiveVector newObjVector;
        
        for (unsigned i=0; i<nbObj; i++)
        {
            _is >> tmpDouble;
            newObjVector.at(i)=tmpDouble;
        }            
        objectiveVector(newObjVector);

            

        _is >> doubleSize;
        doubleVars.resize(doubleSize);
        for (unsigned i=0; i<doubleSize; i++)
        {
            _is >> tmpDouble;
            doubleVars[i]=tmpDouble;
        }            

        _is >> intSize;
        intVars.resize(intSize);
        for (unsigned i=0; i<intSize; i++)
        {
            _is >> tmpInt;
            intVars[i]=tmpInt;
        }

        _is >> boolSize;
        boolVars.resize(boolSize);
        for (unsigned i=0; i<boolSize; i++)
        {
            _is >> tmpBool;
            boolVars[i]=tmpBool;
        }

        _is >> nbStdDev;
        dblStdDev.resize(nbStdDev);
        for (unsigned i=0; i<nbStdDev; i++)
        {
            _is >> tmpDouble;
            dblStdDev[i]=tmpDouble;
        }

        _is >> intProbMut;
        
        _is >> savedVarsSize;
        savedVars.resize(savedVarsSize);
        for (unsigned i=0; i<savedVarsSize; i++)
        {
            _is >> tmpDouble;
            savedVars[i]=tmpDouble;
        }


        // END   Code of input
    }



    std::vector<double> doubleVars;
    std::vector<int> intVars;
    std::vector<bool> boolVars;
    std::vector<double> savedVars; // vars that are neither optVariables nor optObjectives,
                                    // but which we want to keep information of.
    
    std::vector<double> dblStdDev; //sigma, see adaptive mutation articles
    double intProbMut; //probability for int mutation (see Evolution Strategies for MIOpt...) Back & Schutz 
    
};

#endif
