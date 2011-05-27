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

 	@file ModPlusOMCtrl.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#ifndef _MODPLUSOMCTRL_H
#define _MODPLUSOMCTRL_H

#include "ModPlusCtrl.h"
#include "OpenModelica.h"


class ModPlusOMCtrl :public ModPlusCtrl
{
public:
	ModPlusOMCtrl(ModModelPlus* _model,MOomc* _oms,QString _mmoFolder,QString _moFilePath,QString modModelName);
	~ModPlusOMCtrl(void);

	ModPlusCtrl::Type type();

	// Variables functions
	bool readOutputVariables(MOVector<Variable> *,QString _resFile="");
	bool readInitialVariables(MOVector<Variable> *,QString _initFile="");

	// Parameters
	void setDefaultParameters();

	// Compile function
	bool isCompiled();
	bool compile();
	bool createInitFile();
	// Simulate function
        bool simulate(QString tempDir,MOVector<Variable> * inputVars,MOVector<Variable> * outputVars,QStringList filesToCopy = QStringList());
	void setMmoFolder(QString _mmoFolder);
private :
	QString _initFile;
	QString _resFile;
	QString _exeFile;

};



#endif
