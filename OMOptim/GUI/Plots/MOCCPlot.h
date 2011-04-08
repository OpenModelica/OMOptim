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

 	@file MinCCPlot.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 
*/

#ifndef _MINCCPLOT_H
#define _MINCCPLOT_H

#include "qwt_plot.h"
#include "qwt_painter.h"
#include "qwt_plot_canvas.h"
#include "qwt_plot_marker.h"
#include "qwt_plot_curve.h"
#include "qwt_scale_widget.h"
#include "qwt_legend.h"
#include "qwt_symbol.h"
#include "qwt_scale_draw.h"
#include "qwt_math.h"
#include "qwt_dial.h"
#include "qpaintengine.h"
#include "MOVector.h"
#include "qwt_plot_picker.h"
#include "qwt_plot_grid.h"
#include "QApplication"
#include "CCTools.h"
#include "MOPlot.h"
#include "MOCCCurve.h"

#include "MERResult.h"


class MinCCPlot : public MOPlot
{
	Q_OBJECT 

public:
	
	enum CCPlotType
	{
		CC,
		GCC,
		ICC
	};

	inline MinCCPlot(MERResult *);
	~MinCCPlot(void){};

//	inline int getNearestPointIndex(QwtPlotCurve *,const QwtDoublePoint &);
	//inline void setStreams(MOVector<EIStream> *);
	inline void setCCType(int);

	double getTPinch(){return result->TPinch->finalValue(0,0);};
	double getMER(){return result->MER->finalValue(0,0);};
	double getMERCold(){return result->MERCold->finalValue(0,0);};

private:
	
	MERResult* result;

	int ccType;
	
	double selectedT;
	double selectedH;

	QwtPlotPicker *picker1;


	

	inline virtual void drawItems (QPainter *painter, const QRect &rect,
			const QwtScaleMap map[axisCnt], const QwtPlotPrintFilter &pfilter) const;




public slots:
	inline void onClicked(const QwtDoublePoint & pos);
	inline void relaunch();

signals :
	inline void clickedOnCurves(MOVector<EIStream>*);

};


MinCCPlot::MinCCPlot(MERResult* _result)
{	
	result = _result;
	
	// Picker on plot
	picker1 = new QwtPlotPicker(canvas());
	picker1->setTrackerMode(QwtPicker::AlwaysOff);
	picker1->setSelectionFlags(QwtPicker::PointSelection);
	connect(picker1, SIGNAL(selected(const QwtDoublePoint &)),this, SLOT(onClicked(const QwtDoublePoint &)));

	// Style
	QPalette myPalette;
	myPalette.setColor(QPalette::Background,QColor(Qt::white));
	myPalette.setColor(QPalette::Window,QColor(Qt::white));
	


	//this->canvas()->setFrameStyle(QFrame::NoFrame);
	QFont _axisFont(QApplication::font().family(),8,QFont::Bold);
	this->canvas()->setFont(_axisFont);
	this->setAxisFont(QwtPlot::xBottom,_axisFont);
	this->setAxisFont(QwtPlot::yLeft,_axisFont);
	//this->setCanvasBackground(QColor(Qt::white));
	this->setPalette(myPalette);
	this->canvas()->setPalette(myPalette);
	this->setAutoFillBackground(true);
	

	this->setMargin(5);
	this->setContentsMargins(5,5,5,5);

	QwtText xtitle;
	xtitle.setFont(_axisFont);
	xtitle.setText("Heat load [kW]");

	QwtText ytitle;
	ytitle.setFont(_axisFont);
	ytitle.setText("Temperature [K]");

	this->setAxisTitle(QwtPlot::yLeft,ytitle);
	this->setAxisTitle(QwtPlot::xBottom,xtitle);
	
		
	// grid 
    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->enableXMin(false);
	grid->enableYMin(false);
	grid->setMajPen(QPen(Qt::lightGray, 0, Qt::SolidLine));
    grid->setMinPen(QPen(Qt::lightGray, 0 , Qt::SolidLine));
    grid->attach(this);
	

	setCCType(GCC);
}



void MinCCPlot::relaunch()
{
	result->problem()->launch(ProblemConfig());
	replot();
}

//void MinCCPlot::setStreams(MOVector<EIStream> *_streams)
//{
//	streams = _streams;
//	connect(streams,SIGNAL(hasChanged()),this,SLOT(refresh()));
//	refresh();
//}

//int MinCCPlot::getNearestPointIndex(QwtPlotCurve * curve,const QwtDoublePoint & point)
//{
//
//	double minDist=0;
//	double curDist=0;
//
//	int xPos = transform(QwtPlot::xBottom,point.x());
//	int yPos = transform(QwtPlot::yLeft,point.y());
//
//	int xPointPos;
//	int yPointPos;
//
//	int indexMin = -1;
//
//	/*for(int iPoint=0;iPoint<xData.size();iPoint++)
//	{
//		xPointPos = transform(QwtPlot::xBottom,xData.at(iPoint));
//		yPointPos = transform(QwtPlot::yLeft,yData.at(iPoint));
//
//		curDist = sqrt(pow((double)(yPointPos-yPos),2)+pow((double)(xPointPos-xPos),2));
//		if((curDist<minDist)|| (iPoint==0))
//		{
//			indexMin = iPoint;
//			minDist = curDist;
//		}
//	}*/
//	return indexMin;
//}

void MinCCPlot::onClicked(const QwtDoublePoint & pos)
{
	
	int selectedPoint = getNearestPointIndex(result->curveHot,pos);

}

void MinCCPlot::drawItems (QPainter *painter, const QRect &rect,
						   const QwtScaleMap map[axisCnt], const QwtPlotPrintFilter &pfilter) const
{

	painter->save();

	const QwtPlotItemList& itmList = itemList();
	for ( QwtPlotItemIterator it = itmList.begin();
		it != itmList.end(); ++it )
	{
		QwtPlotItem *item = *it;
		if ( item && item->isVisible() )
		{
			if ( !(pfilter.options() & QwtPlotPrintFilter::PrintGrid)
				&& item->rtti() == QwtPlotItem::Rtti_PlotGrid )
			{
				continue;
			}

#if QT_VERSION >= 0x040000
			painter->setRenderHint(QPainter::Antialiasing,true);
#endif

			item->draw(painter, 
				map[item->xAxis()], map[item->yAxis()],
				rect);
		}
	}

	painter->restore();
}

void MinCCPlot::setCCType(int _type)
{
	if(ccType!=_type)
	{
		clear();
		ccType = _type;

		switch(ccType)
		{
			case CC :
				addCurve(result->curveHot);
				addCurve(result->curveCold);
				break;
			case GCC :
				addCurve(result->curveGcc);
				break;
			default :
				break;
		}
		replot();
	}
}





#endif
