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

 	@file EIHEN1.h
 	@brief File concerning EIHEN1 class
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */

#ifndef PROBLEMHEN1_H
#define PROBLEMHEN1_H

#include "EIProblem.h"
#include "EIMERResult.h"
#include "EIConnConstrs.h"
#include "ModModelPlus.h"
#include "EIValueFiller.h"
#include "EIHEN1Functions.h"
#include "MilpHEN1.h"

/*
 *  Class for heat exchanger network resolution. EIHEN1 implements a really simple version
 * based on pinch rules. No cost or surface calculations in this 
 */
class EIHEN1 : public EIProblem
{
        Q_OBJECT

public:
    EIHEN1(Project*,ModReader*,MOomc*);
	EIHEN1(const EIHEN1 &);
	virtual ~EIHEN1(void);
	virtual QString getClassName(){return "EIHEN1";};

	void launch(ProblemConfig _config);
	
	QDomElement toXmlData(QDomDocument & doc);
	bool checkBeforeComp(QString & error);
	

        void setIncludeUtilities(bool);
        EIConnConstrs* connConstrs();

protected :
        EIConnConstrs *_connConstrs; // connnection constraints
        bool _includeUtilities;

};


#endif
