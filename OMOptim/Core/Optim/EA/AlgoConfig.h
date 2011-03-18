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

 	@file AlgoConfig.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#if !defined(_ALGOCONFIG_H)
#define _ALGOCONFIG_H

#include <QtCore/QObject>
#include <QtCore/QVariantList>
#include <QtCore/QStringList>
#include <QtCore/QVector>
#include "MOVector.h"


#include <eo>
#include "AlgoParameter.h"


class AlgoConfig : public QObject
{
	Q_OBJECT

public :
		
	MOVector<AlgoParameter> *parameters;
	

protected :
	QObject* problem; //just for signals


public:

	AlgoConfig(){};

	AlgoConfig(QObject* _problem){
		setProblem(_problem);
		parameters = new MOVector<AlgoParameter>();
	}

	~AlgoConfig(void){
		delete parameters;
	}

	virtual AlgoConfig* clone()
	{
		AlgoConfig* newAlgoConfig = new AlgoConfig(problem);
		newAlgoConfig->parameters = parameters->clone();
		return newAlgoConfig;
	}

	virtual double getParameterValue(QString parameterName,double defaultValue)
	{
		int i=parameters->findItem(parameterName);
		if(i<0)
			return defaultValue;
		else
			return parameters->items.at(i)->getFieldValue(AlgoParameter::VALUE).toDouble();
	}

	virtual int getParameterValue(QString parameterName,int defaultValue)
	{
		int i=parameters->findItem(parameterName);
		if(i<0)
			return defaultValue;
		else
			return parameters->items.at(i)->getFieldValue(AlgoParameter::VALUE).toInt();
	}

	virtual QString toString()
	{
		return parameters->toSavingString();
	}

	virtual void setProblem(QObject* _problem)
	{
		problem = _problem;
		connect(this,SIGNAL(changed()),problem,SLOT(onAlgoConfigChanged()));
	}

	void update(QString _name, QVariant _value)
	{

	}

signals:
	void changed();

};



#endif
