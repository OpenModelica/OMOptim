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
#if QWT_VERSION >= 0x060000
#include "qwt_plot_renderer.h"
#endif

#include "qpaintengine.h"
#include "MOVector.h"

#include <QMenu>
#include <QClipboard>

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

    inline void clear();




protected:


        QFont _axisFont;
    double selectedX;
    double selectedY;

    QwtPlotPicker *picker1;
    QwtPlotZoomer *_zoomer;

    QList<QwtPlotCurve*> curves;
    QAction* _actionCopy;
    QAction* _actionRefresh;
#if QWT_VERSION < 0x060000
    inline virtual void drawItems (QPainter *painter, const QRect &rect,
                                   const QwtScaleMap map[axisCnt], const QwtPlotPrintFilter &pfilter) const;
#endif




    //inline void updateCurveStyle(QwtPlotCurve*,MOCCCurve &);



public slots :
    inline void popUpMenu(const QPoint &pos);
    inline void onCopyAsked();
    inline void onRefreshAsked();

    // Zoom
//    void zoomed (const QwtDoubleRect &);
    void setEnabledZoom(bool on);
//    void zoomOut();
    bool zoomIsOn();
    void enableZoom(bool checked);

    // Data picker

    void contextMenuEvent(QContextMenuEvent *);

//signals :
//    inline void clickedOnCurves(QwtPlotCurve *);


};


MOPlot::MOPlot()
{

    // Style
//    QPalette myPalette;
//    myPalette.setColor(QPalette::Background,QColor(Qt::white));
//    myPalette.setColor(QPalette::Window,QColor(Qt::white));

    QwtPlotCanvas *pPlotCanvas = static_cast<QwtPlotCanvas*>(this->canvas());
    pPlotCanvas->setFrameStyle(QFrame::NoFrame);
    _axisFont = QFont(QApplication::font().family(),8,QFont::Normal);
    _axisFont.setStyleStrategy(QFont::PreferAntialias);
    this->canvas()->setFont(_axisFont);
    this->setAxisFont(QwtPlot::xBottom,_axisFont);
    this->setAxisFont(QwtPlot::yLeft,_axisFont);


    QFont font;
    font.setPointSize(8);
    this->axisWidget(QwtPlot::xBottom)->setFont(font);
    this->axisWidget(QwtPlot::yLeft)->setFont(font);


    this->setCanvasBackground(QColor(Qt::white));
    this->setContentsMargins(5,5,5,5);

    // grid
    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->enableXMin(false);
    grid->enableYMin(false);
#if QWT_VERSION >= 0x060100
    grid->setMajorPen(QPen(Qt::lightGray, 0, Qt::SolidLine));
    grid->setMinorPen(QPen(Qt::lightGray, 0 , Qt::SolidLine));
#else
    grid->setMajPen(QPen(Qt::lightGray, 0, Qt::SolidLine));
    grid->setMinPen(QPen(Qt::lightGray, 0 , Qt::SolidLine));
#endif
    grid->attach(this);

    //zoom
     _zoomer = new QwtPlotZoomer(canvas());
     _zoomer->setMousePattern(QwtEventPattern::MouseSelect1, Qt::MidButton);
     _zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::NoButton);
     _zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::NoButton);

//    _zoomer = new QwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft,
//                                QwtPicker::DragSelection | QwtPicker::CornerToCorner, QwtPicker::AlwaysOff, canvas());
    _zoomer->setRubberBandPen(QPen(Qt::black));


//    connect (_zoomer,SIGNAL(zoomed (const QwtDoubleRect &)),this,SLOT(zoomed (const QwtDoubleRect &)));
//    setEnabledZoom(false);

    // action
    _actionCopy = new QAction("Copy", this);
    connect(_actionCopy,SIGNAL(triggered()),this, SLOT(onCopyAsked()));
    _actionCopy->setShortcut(QKeySequence::Copy);
    _actionCopy->setShortcutContext(Qt::WidgetShortcut);
    this->addAction(_actionCopy);

    _actionRefresh = new QAction("Refresh", this);
    connect(_actionRefresh,SIGNAL(triggered()),this, SLOT(onRefreshAsked()));
    _actionRefresh->setShortcut(QKeySequence::Refresh);
    _actionRefresh->setShortcutContext(Qt::WidgetShortcut);
    this->addAction(_actionRefresh);


    // context menu
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,SIGNAL(customContextMenuRequested(const QPoint &  )),this,SLOT(popUpMenu(const QPoint &)));

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
    // first detach curves in order not to delete them
    for(int i=0;i<curves.size();i++)
    {
        curves.at(i)->detach();
    }

#if QWT_VERSION >= 0x060000
    detachItems(QwtPlotItem::Rtti_PlotCurve);
    detachItems(QwtPlotItem::Rtti_PlotMarker);
#else
    QwtPlot::clear();
#endif

    curves.clear();
}

void MOPlot::addCurve(QwtPlotCurve *_curve)
{
    if(!curves.contains(_curve))
        curves.push_back(_curve);

    _curve->attach(this);
}


#if QWT_VERSION < 0x060000
void MOPlot::drawItems (QPainter *painter, const QRect &rect,
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
#endif



void MOPlot::popUpMenu(const QPoint &pos)
{
    QMenu *menu = new QMenu;
    menu->insertAction(NULL,_actionCopy);
    menu->insertAction(NULL,_actionRefresh);

    /*
    menu->addAction(QString("Copy"), this, SLOT(onCopyAsked()), QKeySequence::Copy);
    menu->addAction(QString("Replot"), this, SLOT(onRefreshAsked()));*/
    menu->exec(this->mapToGlobal(pos));

}

void MOPlot::onRefreshAsked()
{
    qDebug("replot");
    replot();
    //_zoomer->setZoomBase(true);
}

void MOPlot::onCopyAsked()
{
    QImage image;
    // Print the plot to an image
#if QWT_VERSION < 0x060000
    print( image );
#else
    QwtPlotRenderer plotRenderer;
    plotRenderer.renderTo(this, image);
#endif

    // Set the clilpboard image
    QClipboard * clipboard =
            QApplication::clipboard();
    clipboard->setImage(image);


    // create text
    QString csv;
    QwtPlotCurve* curve;
    QString separator = "\t";
    for(int i=0;i<curves.size();i++)
    {
        curve = curves.at(i);
        QString yTitle = this->axisTitle(QwtPlot::yLeft).text();
        QString xTitle = this->axisTitle(QwtPlot::xBottom).text();
        csv+= xTitle + separator + yTitle + "\n";
#if QWT_VERSION >= 0x060000
        for(unsigned int j=0;j<curve->data()->size();j++)
        {
          csv+= QString::number(curve->sample(j).x()) + separator + QString::number(curve->sample(j).y()) + "\n";
          csv+= "\n \n";
        }
#else
        for(unsigned int j=0;j<curve->data().size();j++)
        {
          csv+= QString::number(curve->x(j)) + separator + QString::number(curve->y(j)) + "\n";
          csv+= "\n \n";
        }
#endif
    }
    clipboard->setText(csv);
}



#endif
