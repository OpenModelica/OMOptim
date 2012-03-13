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

     @file MOPlot.h
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version 
*/

#ifndef _MINPLOT_H
#define _MINPLOT_H

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
#include "qwt_plot_zoomer.h"
#include <QApplication>

//#include "CCTools.h"
//#include "Plots/MOCCCurve.h"




class MOPlot : public QwtPlot
{
    Q_OBJECT 
public:
    inline MOPlot(void);
    inline ~MOPlot(void);
        inline void addCurve(QwtPlotCurve *curve);
        inline void setCurves(QList<QwtPlotCurve*> & _curves);
    inline int getNearestPointIndex(QwtPlotCurve *,const QwtDoublePoint &);

    inline void clear();

    


private:
    
    
    
    double selectedX;
    double selectedY;

    QwtPlotPicker *picker1;

    QwtPlotZoomer *zoomer1;

        QList<QwtPlotCurve*> curves;

    

    inline virtual void drawItems (QPainter *painter, const QRect &rect,
            const QwtScaleMap map[axisCnt], const QwtPlotPrintFilter &pfilter) const;
    



    //inline void updateCurveStyle(QwtPlotCurve*,MOCCCurve &);



public slots:
    inline void onClicked(const QwtDoublePoint & pos);

    // Zoom
    void zoomed (const QwtDoubleRect &);
    void setEnabledZoom(bool on);
    void zoomOut();
    bool zoomIsOn();
        void enableZoom(bool checked);

    // Data picker

    void contextMenuEvent(QContextMenuEvent *);

signals :
        inline void clickedOnCurves(QwtPlotCurve *);


};


MOPlot::MOPlot()
{
    

    // Picker on plot
    picker1 = new QwtPlotPicker(canvas());
    picker1->setTrackerMode(QwtPicker::AlwaysOff);
    picker1->setSelectionFlags(QwtPicker::PointSelection);
    connect(picker1, SIGNAL(selected(const QwtDoublePoint &)),this, SLOT(onClicked(const QwtDoublePoint &)));

    // Style
    this->canvas()->setFrameStyle(QFrame::NoFrame);
    QFont _axisFont(QApplication::font().family(),8,QFont::Normal);
    _axisFont.setStyleStrategy(QFont::PreferAntialias);
    this->canvas()->setFont(_axisFont);
    this->setAxisFont(QwtPlot::xBottom,_axisFont);
    this->setAxisFont(QwtPlot::yLeft,_axisFont);
    

    QFont font;
    font.setPointSize(8);
    this->axisWidget(QwtPlot::xBottom)->setFont(font);
    this->axisWidget(QwtPlot::yLeft)->setFont(font);


    this->setCanvasBackground(QColor(Qt::white));
    this->setMargin(5);
    this->setContentsMargins(5,5,5,5);
    

    
        
    // grid 
    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->enableXMin(false);
    grid->enableYMin(false);
    grid->setMajPen(QPen(Qt::lightGray, 0, Qt::SolidLine));
    grid->setMinPen(QPen(Qt::lightGray, 0 , Qt::SolidLine));
    grid->attach(this);
    
    //zoom
    zoomer1 = new QwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft,
            QwtPicker::DragSelection | QwtPicker::CornerToCorner, QwtPicker::AlwaysOff, canvas());
    zoomer1->setRubberBandPen(QPen(Qt::black));


    connect (zoomer1,SIGNAL(zoomed (const QwtDoubleRect &)),this,SLOT(zoomed (const QwtDoubleRect &)));
    setEnabledZoom(false);

}

MOPlot::~MOPlot()
{
    //detach curves because they will be deleted elsewhere
    // and should not be deleted here
    for(int i=0;i<curves.size();i++)
        curves.at(i)->detach();
}

void MOPlot::setCurves(QList<QwtPlotCurve*>& _curves)
{
    QString msg = "MOPlot::drawItems";
    qDebug(msg.toLatin1().data());


    clear();
    
    for(int i =0;i<_curves.size();i++)
    {
        addCurve(_curves.at(i));
    }

    setAxisAutoScale(QwtPlot::xBottom);
    setAxisAutoScale(QwtPlot::yLeft);

    replot();
}

void MOPlot::clear()
{
    QString msg = "MOPlot::clear";
    qDebug(msg.toLatin1().data());

    // first detach curves in order not to delete them
    for(int i=0;i<curves.size();i++)
    {
        curves.at(i)->detach();
    }


    QwtPlot::clear();
    
    curves.clear();
}

void MOPlot::addCurve(QwtPlotCurve *_curve)
{
    QString msg = "MOPlot::addCurve";
    qDebug(msg.toLatin1().data());

    if(!curves.contains(_curve))
        curves.push_back(_curve);
    
    _curve->attach(this);
}

//void MOPlot::updateCurveStyle(QwtPlotCurve* _plotCurve,MOCCCurve *_curve)
//{
//QwtSymbol sym1;
//QPen pen1;
//
//    switch(_curve.type)
//    {
//    case MOCCCurve::CCCOLD :
//        sym1.setStyle(QwtSymbol::Ellipse);
//        sym1.setSize(6);
//        sym1.setPen(QPen(Qt::blue));
//        sym1.setBrush(QBrush(Qt::blue));
//        pen1.setColor(Qt::blue);
//        pen1.setWidth(2);
//        _plotCurve->setSymbol(sym1);
//        _plotCurve->setPen(pen1);
//        _plotCurve->setStyle(QwtPlotCurve::Lines);
//        _plotCurve->setItemAttribute(QwtPlotItem::AutoScale,true);
//        _plotCurve->setRenderHint(QwtPlotItem::RenderAntialiased,true);
//        break;
//    case MOCCCurve::CCHOT :
//        sym1.setStyle(QwtSymbol::Ellipse);
//        sym1.setSize(6);
//        sym1.setPen(QPen(Qt::red));
//        sym1.setBrush(QBrush(Qt::red));
//        pen1.setColor(Qt::red);
//        pen1.setWidth(2);
//        _plotCurve->setSymbol(sym1);
//        _plotCurve->setPen(pen1);
//        _plotCurve->setStyle(QwtPlotCurve::Lines);
//        _plotCurve->setItemAttribute(QwtPlotItem::AutoScale,true);
//        _plotCurve->setRenderHint(QwtPlotItem::RenderAntialiased,true);
//        break;
//    case MOCCCurve::GCC :
//        sym1.setStyle(QwtSymbol::Ellipse);
//        sym1.setSize(6);
//        sym1.setPen(QPen(Qt::red));
//        sym1.setBrush(QBrush(Qt::red));
//        pen1.setColor(Qt::red);
//        pen1.setWidth(2);
//        _plotCurve->setSymbol(sym1);
//        _plotCurve->setPen(pen1);
//        _plotCurve->setStyle(QwtPlotCurve::Lines);
//        _plotCurve->setItemAttribute(QwtPlotItem::AutoScale,true);
//        _plotCurve->setRenderHint(QwtPlotItem::RenderAntialiased,true);
//        break;
//    case MOCCCurve::ICC :
//        sym1.setStyle(QwtSymbol::Ellipse);
//        sym1.setSize(6);
//        sym1.setPen(QPen(Qt::green));
//        sym1.setBrush(QBrush(Qt::green));
//        pen1.setColor(Qt::green);
//        pen1.setWidth(2);
//        _plotCurve->setSymbol(sym1);
//        _plotCurve->setPen(pen1);
//        _plotCurve->setStyle(QwtPlotCurve::Lines);
//        _plotCurve->setItemAttribute(QwtPlotItem::AutoScale,true);
//        _plotCurve->setRenderHint(QwtPlotItem::RenderAntialiased,true);
//        break;
//    case MOCCCurve::OTHER :
//        sym1.setStyle(QwtSymbol::Ellipse);
//        sym1.setSize(6);
//        sym1.setPen(QPen(Qt::black));
//        sym1.setBrush(QBrush(Qt::black));
//        pen1.setColor(Qt::black);
//        pen1.setWidth(2);
//        _plotCurve->setSymbol(sym1);
//        _plotCurve->setPen(pen1);
//        _plotCurve->setStyle(QwtPlotCurve::Lines);
//        _plotCurve->setItemAttribute(QwtPlotItem::AutoScale,true);
//        _plotCurve->setRenderHint(QwtPlotItem::RenderAntialiased,true);
//        break;
//    }
//}



int MOPlot::getNearestPointIndex(QwtPlotCurve * curve,const QwtDoublePoint & point)
{

    double minDist=0;
    double curDist=0;

    int xPos = transform(QwtPlot::xBottom,point.x());
    int yPos = transform(QwtPlot::yLeft,point.y());

    int indexMin = -1;

    /*for(int iPoint=0;iPoint<xData.size();iPoint++)
    {
        xPointPos = transform(QwtPlot::xBottom,xData.at(iPoint));
        yPointPos = transform(QwtPlot::yLeft,yData.at(iPoint));

        curDist = sqrt(pow((double)(yPointPos-yPos),2)+pow((double)(xPointPos-xPos),2));
        if((curDist<minDist)|| (iPoint==0))
        {
            indexMin = iPoint;
            minDist = curDist;
        }
    }*/
    return indexMin;
}

void MOPlot::onClicked(const QwtDoublePoint & pos)
{
    
    /*int selectedPoint = getNearestPointIndex(curveHot,pos);

    MOVector<EIStream> *clickedStreams;
    emit clickedOnCurves(clickedStreams);*/
}

void MOPlot::drawItems (QPainter *painter, const QRect &rect,
                           const QwtScaleMap map[axisCnt], const QwtPlotPrintFilter &pfilter) const
{

    QString msg = "MOPlot::drawItems";
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



#endif
