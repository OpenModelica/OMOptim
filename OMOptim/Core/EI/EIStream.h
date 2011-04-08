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

 	@file EIStream.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#if !defined(_EI_STREAM_H)
#define _EI_STREAM_H

#include "EIItem.h"
#include "ModClass.h"
#include "METemperature.h"
#include "MERefValue.h"
#include "MEQflow.h"
#include "MOomc.h"

class EIStream : public EIItem
{
public:

	enum Field
	{
		//Modelica fields
		NAME,
		CHECKED,
		TIN_V,
		TIN_U,
		TOUT_V,
		TOUT_U,
		QFLOW_V,
		QFLOW_U,
		DTMIN2
	};

	static const int nbFields = 9;
	virtual unsigned getNbFields(){return nbFields;};

        EIStream(EIItem* parent,QString name=QString());
	EIStream(const EIStream &);
	EIStream(QDomElement &);
	virtual EIItem* clone();
	~EIStream(void);
	virtual QString getClassName(){return "EIStream";};

	bool isValid(MOOptVector*, QString &errMsg);
	bool isHot(MOOptVector* variables);

public:

	virtual EI::Type getEIType(){return EI::STREAM;};	
	virtual QString getEITypeName(){return "EIStream";};
	
	MERefValue<METemperature> Tin;
	MERefValue<METemperature> Tout;
	MERefValue<MEQflow> Qflow;
	double DTmin2;
        QStringList references();

	

	QVariant getFieldValue(int, int role = Qt::UserRole) const;
	bool setFieldValue(int ifield, QVariant value_);
	
	static QString sFieldName(int field, int role);
	virtual QString getFieldName(int i, int role = Qt::DisplayRole){return EIStream::sFieldName(i,role);};
};


#endif
