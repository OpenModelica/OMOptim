// $Id$
/**
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linkpings universitet, Department of Computer and Information Science,
 * SE-58183 Linkping, Sweden.
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

 	@file OpenModelica.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 

  */
#ifndef _OpenModelica_H
#define _OpenModelica_H

/*!
 * \file OpenModelica.h
 * \brief File containing OpenModelica class description.
 * \author Hubert Thieriot (CEP-Armines)
 * \version 0.1
 */

#include <iostream>
#include <vector>
#include "Variable.h"
#include "MOVector.h"
#include "MOomc.h"
#include "vqtconvert.h"

#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <iostream>
#include "MOParameter.h"
#include <QtCore/QProcess>

class OpenModelica
{
	/*! \class OpenModelica
   * \brief Class containing handling functions for OpenModelica.
   *
   *  All functions defined in this class are static. There are used to :
   *	-read/write input or output files
   *	-compile model
   *	
   */

public:
	OpenModelica();
	~OpenModelica(void);

        static bool compile(MOomc *_omc,QString moPath,QString modelToConsider,QString storeFolder,const QStringList & moDeps);
        static void getInputVariablesFromTxtFile(MOomc *_omc,QString filePath, MOVector<Variable> *,QString _modelName);
        static void getInputVariablesFromTxtFile(MOomc *_omc,QTextStream *, MOVector<Variable> *,QString _modelName);
        static bool getInputVariablesFromXmlFile(MOomc *_omc,QString,QString modModelName, MOVector<Variable> *);
        static bool getInputVariablesFromXmlFile(MOomc *_omc,const QDomDocument &,QString modModelName, MOVector<Variable> *);

        static bool getFinalVariablesFromFile(QString, MOVector<Variable> *,QString _modelName);
	static bool getFinalVariablesFromFile(QTextStream *, MOVector<Variable> *,QString _modelName);

        static bool getFinalVariablesFromMatFile(QString, MOVector<Variable> *,QString _modelName);


        static void setInputVariablesTxt(QString, MOVector<Variable> *,QString modModelName,MOParameters *parameters=NULL);
        static bool setInputXml(QString, MOVector<Variable> *, QString modelName,MOParameters *parameters=NULL);
        static void start(QString exeFile,int maxnsec);
	static QString sciNumRx();
        static QString home();



private :
        static Variable* variableFromFmi(const QDomElement & ,QString modModelName, bool & ok);
        static void setInputVariablesXml(QDomDocument &, QString modModelName,MOVector<Variable> *);
        static void setInputParametersXml(QDomDocument &, MOParameters *);

	// Parameters


};

#endif
