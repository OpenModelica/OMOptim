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

     @file MyEoGnuplot.h
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version 0.9

    Slight modification of file MyEoGnuplot1DMonitor.h (paradiseo Project)
    (c) Marc Schoenauer, 2001
    Contact: Marc.Schoenauer@polytechnique.fr

     THIS PROGRAM IS PROVIDED UNDER THE TERMS OF THIS OSMC PUBLIC
    LICENSE (OSMC-PL). ANY USE, REPRODUCTION OR DISTRIBUTION OF
    THIS PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE OF THE OSMC
    PUBLIC LICENSE.

    The OpenModelica software and the Open Source Modelica
    Consortium (OSMC) Public License (OSMC-PL) are obtained
    from Linkopings University, either from the above address,
    from the URL: http://www.ida.liu.se/projects/OpenModelica
    and in the OpenModelica distribution.

    This program is distributed  WITHOUT ANY WARRANTY; without
    even the implied warranty of  MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
    IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS
    OF OSMC-PL.

    See the full OSMC Public License conditions for more details.

    For more information about the Qt-library visit TrollTech's webpage
    regarding the Qt licence: http://www.trolltech.com/products/qt/licensing.html
  */

#ifndef MYMyEoGnuplot_H
#define MYMyEoGnuplot_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QProcess>
#endif

#include <string>
#include <stdio.h>
#include <errno.h>

#include "MOSettings.h"
#include "InfoSender.h"

/** Base class for calls to gnuplot

This class is the abstract class that will be used by further gnuplot
calls to plots what is already written by some eoMonitor into a file

*/
class MyEoGnuplot
{
public:

    /** Open pipe to Gnuplot.

    @param _title Title for gnuplot window.
    @param _extra Extra parameters to gnuplot (default <none>).
    */
    MyEoGnuplot();

    /** Destructor

    Close the gnuplot windows if pipe was correctly opened
    */
    virtual ~MyEoGnuplot();

    /** Class name */
    virtual std::string className() const
        { return "MyEoGnuplot"; }

    /** Send command to gnuplot */
    void gnuplotCommand(const QString & _command);

    /** Send command to gnuplot

    @overload
    */


protected:

    /** Initialize gnuplot

    @param _title Title for gnuplot window.
    @param _extra Extra parameters to gnuplot.
    */
    void initGnuPlot();


public:
    /** Communication with gnuplot OK */
    FILE *gnuplotPipe;
    bool pipeOpen;


    /** Internal counter for gnuplot windows */
   // static unsigned numWindow;
};


#endif // EO_MyEoGnuplot_H



// Local Variables:
// c-file-style: "Stroustrup"
// comment-column: 35
// fill-column: 80
// mode: C++
// End:
