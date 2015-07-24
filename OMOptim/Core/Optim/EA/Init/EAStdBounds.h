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

    @file EAStdBounds.h
    @brief Comments for file documentation.
    @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
    Company : CEP - ARMINES (France)
    http://www-cep.ensmp.fr/english/
    @version 0.9

  */
#ifndef _EASTDBOUNDS_H
#define _EASTDBOUNDS_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtCore/QObject>
#endif

#include "Optimization.h"
#include "VariableType.h"

class EAStdBounds
{
public :
    inline static void setBounds(Optimization*,
                                // QList<QList<ModelPlus*> > subModels,
                                 std::vector<eoRealInterval> &doubleBounds,
                                 std::vector<eoIntInterval> &intBounds,
                                 int &nbDouble,int &nbInt,int &nbBool);


//    inline static void setBounds(VariableDet*,
//                                           QVector<QVector< int> > &index,
//                                          std::vector<eoRealInterval> &doubleBounds,
//                                                std::vector<eoIntInterval> &intBounds,
//                                                int &nbDouble,int &nbInt,int &nbBool);
};





void EAStdBounds::setBounds(Optimization *_problem, /*QList<QList<ModelPlus*> > subModels,*/std::vector<eoRealInterval> &doubleBounds, std::vector<eoIntInterval> &intBounds, int &nbDouble, int &nbInt, int &nbBool)
{
    int nbVar = _problem->optimizedVariables()->size();
    OptVariable* curVar;
    eoRealInterval* realInterval;
    eoIntInterval* intInterval;
    double min,max;
    for(int i=0; i<nbVar; i++)
    {
        curVar = _problem->optimizedVariables()->at(i);
        min = curVar->optMin();
        max = curVar->optMax();

        switch(curVar->getFieldValue(OptVariable::DATATYPE).toInt())
        {
        case OMREAL:
            realInterval = new eoRealInterval(min,max);
            doubleBounds.push_back(*realInterval);
            nbDouble++;
            break;
        case OMINTEGER:
            intInterval = new eoIntInterval((int)min,(int)max);
            intBounds.push_back(*intInterval);
            nbInt++;
            break;

        case OMBOOLEAN:
            //No need to specify bounds for boolean...
            nbBool++;
            break;
        }
    }

//    //Adding int variable corresponding to subModelSelection
//    if(subModels.size()>1)
//    {
//        intInterval = new eoIntInterval((int)0,subModels.size()-1);
//        intBounds.push_back(*intInterval);
//        nbInt++;
//    }
}


#endif
