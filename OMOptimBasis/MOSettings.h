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

     @file MOSettings.h
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version

  */
#ifndef MOSETTINGS_H
#define MOSETTINGS_H

#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QtCore/QVector>

#include "MOParameter.h"


/** MOSettings is a classed used to store settings like recentFiles, gui positions...
  It's only using static functions and QSettings class.
  */
class MOSettings : public MOParameters
{

public :
    static void initialize(bool preferDefault);
    static void updateFromSavedValues();
    static void save();
    static void setFromDefaultValues();
    static void addParameters(MOParameters*,QString tabName = "");

   // static QVariant value(int index,QVariant defaultValue = QVariant());
    static QVariant value(QString name,QVariant defaultValue = QVariant());
    static MOSettings* instance();

public :
    MOSettings(void);
    ~MOSettings(void);


    // this is a singleton
    static MOSettings* _instance;
};


#endif
