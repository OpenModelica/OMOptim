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

 	@file ModModelParameter.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#if !defined(_ModModelParameter_H)
#define _ModModelParameter_H

#include "MOItem.h"
#include "MOVector.h"

class ModModelParameter : public MOItem
{
public:

	enum Field
	{
		//Modelica fields
		NAME,
		VALUE,
		DEFAULTVALUE,
		TYPE,
		MIN,
		MAX,
		DESCRIPTION,
		INDEX // index in OpenModelica parameters or in Dymola parameters (e.g. OpenModelica::STOPVALUE)
	};

	

	ModModelParameter();
	ModModelParameter(const ModModelParameter &);
	ModModelParameter(int index,QString name,QString desc,QVariant defaultValue, QString type, QVariant minValue=0, QVariant maxValue=1);
	ModModelParameter(QString);
	ModModelParameter(QDomElement & domEl);
	~ModModelParameter(void);

	virtual QString getClassName(){return "ModModelParameter";};

	static const int nbFields = 8;
	virtual unsigned getNbFields(){return nbFields;};


	bool setFieldValue(int iField,QVariant _value);
	QVariant getFieldValue(int ifield, int role = Qt::UserRole) const;
	static QString sFieldName(int ifield, int role);
	virtual QString getFieldName(int i, int role = Qt::DisplayRole){return ModModelParameter::sFieldName(i,role);};

	ModModelParameter* clone();

private :
	QString _description;
	QVariant _value;
	QVariant _defaultValue;
	QString _type;
	QVariant _min;
	QVariant _max;
	int _index;


};






#endif
