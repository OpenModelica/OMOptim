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

 	@file ModelicaModifier.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#if !defined(_MODELICAMODIFIER_H)
#define _MODELICAMODIFIER_H

#include <vector>
#include <QtCore/QObject>
#include <QtCore/QAbstractTableModel>
#include <QtCore/QTextStream>
#include <QtGui/QSortFilterProxyModel>
#include <limits>
#include "ModClass.h"
#include "MOItem.h"


class ModelicaModifier : public MOItem
{
public:
	ModelicaModifier(void);
	ModelicaModifier(QString _componentName,QString _modifierName,QString _value,QString _description,ModClass* _component);
	~ModelicaModifier(void);
	QString getClassName(){return "ModelicaModifier";};

public :

	ModelicaModifier* clone();
	QString name();
	QString description();
	QVariant value();

	void setName(QString);
	void setDesc(QString);
	void setValue(QString);
	void setComponent(ModClass*);

	//MOItem functions
	QVariant getFieldValue(int, int role = Qt::UserRole) const;
	bool setFieldValue(int ifield, QVariant value_);
	static QString sFieldName(int field, int role);
	virtual QString getFieldName(int i, int role = Qt::DisplayRole){return ModelicaModifier::sFieldName(i,role);};


	static const int nbFields = 4;
	virtual unsigned getNbFields(){return nbFields;};


	enum Field
	{
		//Modelica fields
		NAME,
		DESCRIPTION,
		VALUE,
		COMPONENT_NAME,
	};




protected :
	// fields
	
	QString _name;
	QString _description;
	QString _value;
	QString _componentName;

	//Component concerned
	ModClass* _component;
};

#endif
