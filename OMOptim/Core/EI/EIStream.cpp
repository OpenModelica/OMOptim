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

        QMap<int,QVariant> defaultValues;
        defaultValues.insert(EIStream::TIN_V,0);
        defaultValues.insert(EIStream::TIN_U,METemperature::C);
        defaultValues.insert(EIStream::TOUT_V,0);
        defaultValues.insert(EIStream::TOUT_U,METemperature::C);
        defaultValues.insert(EIStream::QFLOW_V,0);
        defaultValues.insert(EIStream::QFLOW_U,MEQflow::W);
        defaultValues.insert(EIStream::DTMIN2,0);

        for(int i=0;i<defaultValues.count();i++)
            setFieldValue(defaultValues.keys().at(i),defaultValues.values().at(i));

        _numerized = false;
}

EIStream::~EIStream(void)
{
}

EIStream::EIStream(const EIStream & _stream):EIItem(_stream)
{
        TinRef = _stream.TinRef;
        ToutRef = _stream.ToutRef;
        QflowRef = _stream.QflowRef;
	DTmin2 = _stream.DTmin2;

        TinNum = _stream.TinNum;
        ToutNum = _stream.ToutNum;
        QflowNum = _stream.QflowNum;

        _numerized = _stream._numerized;
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
 if (!_filledFields.contains(ifield)&&(role==Qt::DisplayRole))
		return QString("-");
	else
	{
		switch (ifield)
		{
		case NAME :
			return _name;
		case TIN_V :
                        return TinRef.value();
		case TIN_U :
                        return TinRef.unit();
		case TOUT_V :
                        return ToutRef.value();
		case TOUT_U :
                        return ToutRef.unit();
		case QFLOW_V :
                        return QflowRef.value();
		case QFLOW_U :
                        return QflowRef.unit();
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
                        TinRef.setValue(value_);
			break;
		case TIN_U :
			if(value_.type()==QVariant::String)
                                ok=TinRef.setUnit(value_.toString());
			else
                                TinRef.setUnit(value_.toInt());
			break;
		case TOUT_V :
                        ToutRef.setValue(value_);
			break;
		case TOUT_U :
			if(value_.type()==QVariant::String)
                                ok=ToutRef.setUnit(value_.toString());
			else
                                ToutRef.setUnit(value_.toInt());
			break;
		case QFLOW_V :
                        QflowRef.setValue(value_);
			break;
		case QFLOW_U :
			if(value_.type()==QVariant::String)
                                ok=QflowRef.setUnit(value_.toString());
			else
                                QflowRef.setUnit(value_.toInt());
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

/**
* @brief This function is used to check if this EIStream is valid or not.
* Checks if Tin, Tout and Qflow references are found in variables, that Tin != Tout and QFlow >0
* @return true if this EIStream is valid
*/
bool EIStream::isValid(MOOptVector* variables, QString &errMsg)
{
	//#TODO : do not recheck every times, store valid state
	bool okTin,okTout,okQflow;
	double numTin,numTout,numQFlow;

        numTin = this->TinRef.getNumValue(variables,METemperature::K,okTin,model());
        numTout = this->ToutRef.getNumValue(variables,METemperature::K,okTout,model());
        numQFlow = this->QflowRef.getNumValue(variables,MEQflow::W,okQflow,model());
		
	bool okDT = (numTin!=numTout);
	bool okQPos = (numQFlow>=0);
	
	return (okDT&&okQPos&&okTin&&okTout&&okQflow);
}

bool EIStream::isHot(MOOptVector* variables)
{
	bool okTin,okTout;
        return TinRef.getNumValue(variables,METemperature::K,okTin,model())>ToutRef.getNumValue(variables,METemperature::K,okTout,model());
}


/**
* This function is used to replace reference by its numerical value extracted from variables vector.
* @return a list of references involved in Tin, Tout and Qflow.
*/
QStringList EIStream::references()
{
    QStringList refs;

    refs.push_back(this->TinRef.reference());
    refs.push_back(this->ToutRef.reference());
    refs.push_back(this->QflowRef.reference());

    refs.removeAll(QString());

    return refs;
}

/**
* This function is used to fill numerical values from references and variables.
* In EIStream, this function concerns TinNum, ToutNum and QflowNum fields.
* @return true if all references have been found in variables (or if there were no references), false otherwise
* After a successfull numerization, _numerized is set to true, otherwise to false.
*/
bool EIStream::numerize(MOOptVector* variables)
{
    bool ok1,ok2,ok3;
    TinNum.setValue(TinRef.getNumValue(variables,METemperature::K,ok1,model()),METemperature::K);
    ToutNum.setValue(ToutRef.getNumValue(variables,METemperature::K,ok2,model()),METemperature::K);
    QflowNum.setValue(QflowRef.getNumValue(variables,MEQflow::W,ok3,model()),MEQflow::W);

    _numerized = (ok1 && ok2 && ok3);

    if(!_numerized)
    {
        QString msg;
        msg.sprintf("Failed to numerize EIStream %s. Missing references values",name().utf16());
        infoSender.send(Info(msg,ListInfo::WARNING2));
    }
    return _numerized;
}

bool EIStream::numerized()
{
    return _numerized;
}

void EIStream::resetNumerize()
{
    TinNum.setValue(0,0);
    ToutNum.setValue(0,0);
    QflowNum.setValue(0,0);
    _numerized = false;
}


/**
* Returns numerical value of Cp. Need numerization first. If numerization is not done, this function will do it
* (using variables parameter)
* If failed to numerize, ok is set to false, and returns -1.
*/
double EIStream::Cp(bool &ok,MOOptVector* variables) const
{
    // try to numerize if not done
    // if failure, return -1;
    if(!numerized()&&!numerize(variables))
    {
        ok=false;
        return -1;
    }

    double tinval,toutval,qflowval;

    tinval = TinNum.value(METemperature::K);
    toutval = ToutNum.value(METemperature::K);
    qflowval = QflowNum.value(MEQflow::W);

    if(tinval==toutval)
        return std::numeric_limits<double>::max();
    else
        return fabs(qflowval/fabs(toutval-tinval));
}

