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

  @file MOCCPlot.h
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version 0.9
*/

#ifndef _MOCCPLOT_H
#define _MOCCPLOT_H

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

#include "EIMERResult.h"



class MOCCPlot : public MOPlot
{
    Q_OBJECT

public:

    enum CCPlotType
    {
        CC,
        GCC,
        ICC
    };

    inline MOCCPlot(EIMERResult *);
    ~MOCCPlot(void){};

    //	inline int getNearestPointIndex(QwtPlotCurve *,const QwtDoublePoint &);
    //inline void setStreams(MOVector<EIStream> *);
    inline void setCCType(int);

    inline void setResult(EIMERResult*);
    inline EIMERResult* result();

    inline void setTUnit(METemperature::Units );
    inline void setQUnit(MEQflow::Units );

private:

    EIMERResult* _result;

    int _ccType;

    double selectedT;
    double selectedH;

    MEQflow::Units _qflowUnit;
    METemperature::Units _TUnit;


    QwtPlotPicker *picker1;
    QFont _axisFont;




    inline virtual void drawItems (QPainter *painter, const QRect &rect,
                                   const QwtScaleMap map[axisCnt], const QwtPlotPrintFilter &pfilter) const;




public slots:
    inline void onClicked(const QwtDoublePoint & pos);
    inline void relaunch();

signals :
    inline void clickedOnCurves(MOVector<EIStream>*);

};


MOCCPlot::MOCCPlot(EIMERResult* res)
{	
    _result = res;

    setQUnit(MEQflow::KW);
    setTUnit(METemperature::K);





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
    _axisFont = QFont(QApplication::font().family(),8,QFont::Bold);
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
    xtitle.setText("Heat load");

    QwtText ytitle;
    ytitle.setFont(_axisFont);
    ytitle.setText("Temperature");

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

void MOCCPlot::setResult(EIMERResult* res)
{
    QString msg = "MOCCPlot::setResult";
    qDebug(msg.toLatin1().data());

    _result = res;
    if(!_result)
        clear();
    setCCType(_ccType);
}


void MOCCPlot::setTUnit(METemperature::Units newTUnit)
{
    _TUnit = newTUnit;
    _result->curveCold->setTUnit(_TUnit);
    _result->curveHot->setTUnit(_TUnit);
    _result->curveGcc->setTUnit(_TUnit);


    //update labels
    METemperature tmp;
    QwtText ytitle;
    ytitle.setFont(_axisFont);
    ytitle.setText("Temperature ["+tmp.unit(_TUnit)+"]");
    this->setAxisTitle(QwtPlot::yLeft,ytitle);

    // replot
    replot();
}

void MOCCPlot::setQUnit(MEQflow::Units newQUnit)
{
    _qflowUnit = newQUnit;

    //update curves
    _result->curveCold->setQUnit(_qflowUnit);
    _result->curveHot->setQUnit(_qflowUnit);
    _result->curveGcc->setQUnit(_qflowUnit);



    //update labels
    MEQflow tmp;
    QwtText xtitle;
    xtitle.setFont(_axisFont);
    xtitle.setText("Heat load ["+tmp.unit(_qflowUnit)+"]");
    this->setAxisTitle(QwtPlot::xBottom,xtitle);

    // replot
    replot();
}

void MOCCPlot::relaunch()
{
    if(_result)
    {
        _result->problem()->launch(ProblemConfig());
	replot();
    }
}

//void MOCCPlot::setStreams(MOVector<EIStream> *_streams)
//{
//	streams = _streams;
//	connect(streams,SIGNAL(hasChanged()),this,SLOT(refresh()));
//	refresh();
//}

//int MOCCPlot::getNearestPointIndex(QwtPlotCurve * curve,const QwtDoublePoint & point)
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

void MOCCPlot::onClicked(const QwtDoublePoint & pos)
{
    int selectedPoint = getNearestPointIndex(_result->curveHot,pos);

}

void MOCCPlot::drawItems (QPainter *painter, const QRect &rect,
                           const QwtScaleMap map[axisCnt], const QwtPlotPrintFilter &pfilter) const
{
    QString msg = "MOCCPlot::drawItems";
    qDebug(msg.toLatin1().data());

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

void MOCCPlot::setCCType(int type)
{

    QString msg = "MOCCPlot::setCCType" + QString::number(type).toInt();
    qDebug(msg.toLatin1().data());

    clear();

    if(_result)
    {
        clear();
        _ccType = type;

        switch(_ccType)
        {
        case CC :
            addCurve(_result->curveHot);
            addCurve(_result->curveCold);
            break;
        case GCC :
            addCurve(_result->curveGcc);
            break;
        default :
            break;
        }
        replot();
    }
}





#endif
