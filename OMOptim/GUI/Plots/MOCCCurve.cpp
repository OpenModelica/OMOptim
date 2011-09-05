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

 	@file MOCCCurve.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 
*/

#include "MOCCCurve.h"

MOCCCurve::MOCCCurve(int type)
{

	_type = type;
        _qflowUnit = MEQflow::KW;
        _TUnit = METemperature::K;

	init();


}

MOCCCurve::MOCCCurve(const MOCCCurve & curve)
{
	_type = curve._type;
	_name = curve._name;
	_xData = curve._xData;
	_yData = curve._yData;
}

MOCCCurve::MOCCCurve(QString name,const QList<MEQflow> & xData, const QList<METemperature> &  yData)
{
	_name = name;
	_xData = xData;
	_yData = yData;
}



MOCCCurve::~MOCCCurve(void)
{

}


int MOCCCurve::type()
{
	return _type;
}

void MOCCCurve::setType(int type)
{
	_type = type;
	init();        
}

QList<MEQflow> MOCCCurve::XData()
{
	return _xData;
}
QList<METemperature>  MOCCCurve::YData()
{
	return _yData;
}

void MOCCCurve::setTUnit(METemperature::Units newTUnit)
{
    if(newTUnit!=_TUnit)
    {
        _TUnit = newTUnit;

        //change label
        //#TODO


        //redraw
        setData(_xData,_yData);
    }
}

void MOCCCurve::setQUnit(MEQflow::Units newQUnit)
{
    if(newQUnit!=_qflowUnit)
    {
        _qflowUnit = newQUnit;

        //change label
        //#TODO


        //redraw
        setData(_xData,_yData);
    }

}


void MOCCCurve::init()
{
	QPen pen;
	QwtSymbol sym;
	switch(_type)
	{
	case CCHOT :
	//curveHot initialization
	sym.setStyle(QwtSymbol::Ellipse);
	sym.setSize(3);
	sym.setPen(QPen(Qt::red));
	sym.setBrush(QBrush(Qt::red));
	pen.setColor(Qt::red);
	pen.setWidth(2);
	this->setSymbol(sym);
	this->setPen(pen);
	this->setStyle(QwtPlotCurve::Lines);
	this->setItemAttribute(QwtPlotItem::AutoScale,true);
	this->setRenderHint(QwtPlotItem::RenderAntialiased,true);
	break;

	case CCCOLD :
		//curveCold initialization
	sym.setStyle(QwtSymbol::Ellipse);
	sym.setSize(3);
	sym.setPen(QPen(Qt::blue));
	sym.setBrush(QBrush(Qt::blue));
	this->setSymbol(sym);
	this->setStyle(QwtPlotCurve::Lines);
	pen.setColor(Qt::blue);
	pen.setWidth(2);
	this->setPen(pen);
	this->setItemAttribute(QwtPlotItem::AutoScale,true);
	this->setRenderHint(QwtPlotItem::RenderAntialiased,true);
	break;

	case GCC : 
	//curveGcc initialization
	QwtSymbol sym;
	sym.setStyle(QwtSymbol::Ellipse);
	sym.setSize(3);
	sym.setPen(QPen(Qt::red));
	sym.setBrush(QBrush(Qt::red));
	pen.setColor(Qt::red);
	pen.setWidth(2);
	this->setSymbol(sym);
	this->setPen(pen);
	this->setStyle(QwtPlotCurve::Lines);
	this->setItemAttribute(QwtPlotItem::AutoScale,true);
	this->setRenderHint(QwtPlotItem::RenderAntialiased,true);
	break;
	}
}

void MOCCCurve::setData(const QList<MEQflow> & xData, const QList<METemperature> & yData)
{
	_xData = xData;
	_yData = yData;

        updateRawXData();
        updateRawYData();

	if(_xData.size()==0)
		setRawData(NULL,NULL,0);
	else
                setRawData(_rawXData.data(),_rawYData.data(),_xData.size());
}

void MOCCCurve::clear()
{
    _xData = QList<MEQflow>();
    _yData = QList<METemperature>();

    setRawData(NULL,NULL,0);
}

void MOCCCurve::updateRawXData()
{
   _rawXData.clear();

    for(int i=0;i<_xData.size();i++)
        _rawXData.push_back(_xData.at(i).value(_qflowUnit));

}

void MOCCCurve::updateRawYData()
{
    _rawYData.clear();

    for(int i=0;i<_yData.size();i++)
        _rawYData.push_back(_yData.at(i).value(_TUnit));
}
