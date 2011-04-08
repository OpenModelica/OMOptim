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
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)

 	@file EIStream.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "EIStream.h"

EIStream::EIStream(EIItem* parent, QString name)
:EIItem(parent,name)
{
	_editableFields << EIStream::NAME << EIStream::TIN_V << EIStream::TIN_U << EIStream::TOUT_V << EIStream::TOUT_U
		<< EIStream::QFLOW_V << EIStream::QFLOW_U << EIStream::DTMIN2;
}

EIStream::~EIStream(void)
{
}

EIStream::EIStream(const EIStream & _stream):EIItem(_stream)
{
	Tin = _stream.Tin;
	Tout = _stream.Tout;
	Qflow = _stream.Qflow;
	DTmin2 = _stream.DTmin2;
}


EIItem* EIStream::clone()
{
	return (EIItem*)(new EIStream(*this));
}

EIStream::EIStream(QDomElement & domEl)
{
	QDomNamedNodeMap attributes = domEl.attributes();
	QString fieldName;
	QString fieldValue;
	int iField;

	for(int i=0;i<attributes.count();i++)
	{
		fieldName = attributes.item(i).toAttr().name();
		fieldName.replace(XMLTools::space()," ");
		fieldValue = attributes.item(i).toAttr().value();
		fieldValue.replace(XMLTools::space()," ");
		iField = getFieldIndex(fieldName,Qt::UserRole);
		setFieldValue(iField,QVariant(fieldValue));
	}

	_editableFields << EIStream::NAME << EIStream::TIN_V << EIStream::TIN_U << EIStream::TOUT_V
		<< EIStream::TOUT_U << EIStream::QFLOW_V << EIStream::QFLOW_U <<EIStream::DTMIN2;
}

QVariant EIStream::getFieldValue(int ifield, int role) const
{
	if (!_filledFields.contains(ifield))
		return QString("-");
	else
	{
		switch (ifield)
		{
		case NAME :
			return _name;
		case TIN_V :
			return Tin.value();
		case TIN_U :
			return Tin.unit();
		case TOUT_V :
			return Tout.value();
		case TOUT_U :
			return Tout.unit();
		case QFLOW_V :
			return Qflow.value();
		case QFLOW_U :
			return Qflow.unit();
		case DTMIN2 :
			return DTmin2;
		case CHECKED :
                        return _checked;
		default :
			return "unknown field";
		}
	}
}

QString EIStream::sFieldName(int ifield, int role)
{
	switch (ifield)
	{
		case NAME :
			return "Name";
		case TIN_V :
			return "Tin";
		case TIN_U :
			switch(role)
			{
			case Qt::DisplayRole:
				return "[Tin]";
			default :
				return "Tin_Unit";
			}
		case TOUT_V :
			return "Tout";
		case TOUT_U :
			switch(role)
			{
			case Qt::DisplayRole:
				return "[Tout]";
			default :
				return "Tout_Unit";
			}
		case QFLOW_V :
			return "Qflow";
		case QFLOW_U :
			switch(role)
			{
			case Qt::DisplayRole:
				return "[Qflow]";
			default :
				return "Qflow_Unit";
			}
		case CHECKED :
			return "Checked";
		case DTMIN2 :
			return "DTmin_2";
		default :
			return "unknown field";
	}
}



bool EIStream::setFieldValue(int ifield,QVariant value_)
{
	bool ok=true;

	switch (ifield)
	{
		case NAME :
			_name=value_.toString();
			break;
		case TIN_V :
			Tin.setValue(value_);
			break;
		case TIN_U :
			if(value_.type()==QVariant::String)
				ok=Tin.setUnit(value_.toString());
			else
				Tin.setUnit(value_.toInt());
			break;
		case TOUT_V :
			Tout.setValue(value_);
			break;
		case TOUT_U :
			if(value_.type()==QVariant::String)
				ok=Tout.setUnit(value_.toString());
			else
				Tout.setUnit(value_.toInt());
			break;
		case QFLOW_V :
			Qflow.setValue(value_);
			break;
		case QFLOW_U :
			if(value_.type()==QVariant::String)
				ok=Qflow.setUnit(value_.toString());
			else
				Qflow.setUnit(value_.toInt());
			break;
		case CHECKED :
                        _checked =value_.toBool();
			break;
		case DTMIN2:
			DTmin2 =value_.toDouble();
			break;
	}
	if(!_filledFields.contains(ifield))
		_filledFields.push_back(ifield);
	
	emit modified();
	return ok;
}


bool EIStream::isValid(MOOptVector* variables, QString &errMsg)
{
	//#TODO : do not recheck every times, store valid state
	bool okTin,okTout,okQflow;
	double numTin,numTout,numQFlow;

	numTin = this->Tin.getNumValue(variables,METemperature::K,okTin);
	numTout = this->Tout.getNumValue(variables,METemperature::K,okTout);
	numQFlow = this->Qflow.getNumValue(variables,MEQflow::W,okQflow);
		
	bool okDT = (numTin!=numTout);
	bool okQPos = (numQFlow>=0);
	
	return (okDT&&okQPos&&okTin&&okTout&&okQflow);
}

bool EIStream::isHot(MOOptVector* variables)
{
	bool okTin,okTout;
	return Tin.getNumValue(variables,METemperature::K,okTin)>Tout.getNumValue(variables,METemperature::K,okTout);
}


QStringList EIStream::references()
{
    QStringList refs;

    refs.push_back(this->Tin.reference());
    refs.push_back(this->Tout.reference());
    refs.push_back(this->Qflow.reference());

    refs.removeAll(QString());

    return refs;
}

