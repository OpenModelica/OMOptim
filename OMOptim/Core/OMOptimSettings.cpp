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
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)

        @file OMOptimSettings.cpp
        @brief Comments for file documentation.
        @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
        Company : CEP - ARMINES (France)
        http://www-cep.ensmp.fr/english/
        @version

  */

#include "OMOptimSettings.h"
#include "Dymola.h"
#include "OpenModelica.h"

void OMOptimSettings::initialize()
{
    QStringList names;
    QStringList descs;
    QStringList groups;
    QVariantList defaultValues;
    QVector<MOParameter::Type> types;

    //*******************************
    // Dymola path
    //*******************************
#ifdef WIN32
    names << QString("dymolaExe");
    groups << "Dymola";
    descs << QString("Path of Dymola executable");
    QString dymolaPath = Dymola::getExecutablePath();
    defaultValues << dymolaPath;
    types.push_back(MOParameter::FILEPATH);
#endif


    //*******************************
    // Modelica library path
    //*******************************
    names << QString("modelicaLibrary");
    groups << "Modelica";
    descs << QString("Path of Modelica library");
    defaultValues << OpenModelica::getLibraryPath("3.2.2");
    types.push_back(MOParameter::FILEPATH);


    //    //*******************************
    //    // Quit omc at end of program
    //    //*******************************
    //    names << QString("stopOMCwhenQuit");
    //    descs << QString("Automatically end OMC when quitting");
    //        defaultValues << true;
    //    types.push_back(BOOL);

    //*******************************
    // Model depth read at beginning
    //*******************************
    names << QString("DepthReadWhileLoadingModModel");
    groups << QString();
    descs << QString("Default reading depth in Modelica hierarchy (-1 : entire model)");
    defaultValues << 2;
    types.push_back(MOParameter::INT);

    //*******************************
    // Max number of digits in dsin
    //*******************************
    names << QString("MaxDigitsDsin");
    groups << "Dymola";
    descs << QString("Maximum number of digits in dsin.txt");
    defaultValues << 5;
    types.push_back(MOParameter::INT);

    //*******************************
    // Gnuplot
    //*******************************
    names << QString("useGnuplot");
    groups << QString();
    descs << QString("Use gnuplot to display intermediary results");
    defaultValues << false;
    types.push_back(MOParameter::BOOL);

    names << QString("pgnuplotEXE");
    groups << QString();
    descs << QString("Path of gnuplot pipe executable (pGnuplot.exe)");
    defaultValues << QString();
    types.push_back(MOParameter::FILEPATH);



    //*******************************
    // OM Flag
    //*******************************
    names << QString("OMCCommandLineOptions");
    groups << "Open Modelica";
    descs << QString("OMC Command line options");
    defaultValues << "";
    types.push_back(MOParameter::STRING);

    // processing
    MOParameters* params = new MOParameters();
    MOParameter *param;

    for(int i=0; i<names.size();i++)
    {
            // update
            param = new MOParameter(names.at(i),descs.at(i),defaultValues.at(i),types.at(i));
            param->setFieldValue(MOParameter::GROUP,groups.at(i));
            params->addItem(param);
    }

    MOSettings::instance()->addParameters(params,QApplication::applicationName());
}



