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

 	@file MyEoGnuplot1DMonitor.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#ifndef MyEoGnuplot1DMonitor_H
#define MyEoGnuplot1DMonitor_H

#include <fstream>
#include <string>

#include "eoObject.h"
#include "eoFileMonitor.h"
#include "MyEoGnuplot.h"


/** Plot eoStat

@author Marc Schoenauer
@version 0.0 (2000)

This class plots through gnuplot the eoStat given as argument

MyEoGnuplot1DMonitor plots stats through gnuplot

Assumes that the same file is appened every so and so, and replots it
everytime
*/
class MyEoGnuplot1DMonitor : public eoFileMonitor, public MyEoGnuplot
{
public:

  // this "using" directive generates a compiler internal error in GCC 4.0.0 ...
  // it's been removed, and the only call to vec was replaced by this->vec in MyEoGnuplot1DMonitor.cpp
  //    using eoMonitor::vec;

    /** Constructor */
    MyEoGnuplot1DMonitor(std::string _filename, bool _top=false) :
        eoFileMonitor(_filename, " "),
        MyEoGnuplot()
        {}

    /** Destructor */
    virtual ~MyEoGnuplot1DMonitor(){}

    virtual eoMonitor& operator()();

    virtual void FirstPlot();

    /** Class name */
    virtual std::string className() const
        { return "MyEoGnuplot1DMonitor"; }
};


#endif // EO_MyEoGnuplot1DMonitor_H



// Local Variables:
// c-file-style: "Stroustrup"
// comment-column: 35
// fill-column: 80
// mode: C++
// End:
