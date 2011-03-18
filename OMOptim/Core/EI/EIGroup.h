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

 	@file EIGroup.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#ifndef _EIGroup_H
#define _EIGroup_H

#include "EIItem.h"
#include "EIGroupFact.h"


class EIGroup : public EIItem
{
public :
	enum Field
	{
		//Modelica fields
		NAME,
		CHECKED,
		FACTISVARIABLE,
		FACTMIN,
		FACTMAX,
		FACT,
		COSTFIX,
		COSTMULT
	};

	static const int nbFields = 8;
	virtual unsigned getNbFields(){return nbFields;};

	public:
		EIGroup();
		EIGroup(EIItem* _parent,QString _name);
		EIGroup(const EIGroup &);
		EIGroup(QDomElement &);
		~EIGroup(void);

		EIItem* clone();

		virtual QString getClassName(){return "EIGroup";};
		virtual EI::Type getEIType(){return EI::GROUP;};

		// access and edit functions
		QVariant getFieldValue(int iField, int role = Qt::UserRole) const;
		bool setFieldValue(int iField, QVariant value);
		static QString sFieldName(int field, int role);
		QString getFieldName(int i, int role = Qt::DisplayRole){return EIGroup::sFieldName(i,role);};



		EIGroupFact* getFact();
		bool isFactVariable();

		
protected :
	bool factIsVariable;
	EIGroupFact fact;
	double costFix;
	double costMult;
};




#endif
