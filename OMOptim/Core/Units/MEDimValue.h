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

 	@file MEDimValue.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 

  */
#if !defined(_MEDIMVALUE_H)
#define _MEDIMVALUE_H

#include <QtCore/QObject>
#include <QtCore/QAbstractTableModel>
#include <QtCore/QTextStream>
#include <QtCore/QStringList>

class MEDimValue
{
public:
	MEDimValue(double value=0,int unit=0);
	virtual ~MEDimValue(void);

	QStringList units()  const ;
        virtual QString unit() const ;
	int iUnit() const ;
	virtual QString unit(int iUnit) const =0;
	virtual unsigned nbUnits() const =0;
	virtual void setValue(double,int iUnit=-1);
	virtual bool setValue(double,QString unit);
	virtual void setUnit(int iUnit);
        virtual bool setUnit(QString iUnit);
	double value(int iUnit) const;
        double value() const;
	
        virtual QString toString() const {return QString::number(value(_unit))+" "+unit();}

        virtual MEDimValue & operator=(const MEDimValue &);



protected :
	int _unit;
	double _value;

	virtual double convert(double value,int orgUnit,int dstUnit) const =0 ;
};


#endif
