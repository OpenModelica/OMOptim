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

     @file MOOptPlot.h
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version

*/

#ifndef _MINOPTPLOT_H
#define _MINOPTPLOT_H

#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_symbol.h>
#include <qwt_scale_draw.h>
#include <qwt_math.h>
#include <qwt_dial.h>
#include <qpaintengine.h>
#include <qwt_plot_grid.h>
#if QWT_VERSION >= 0x060000
#include "qwt_plot_renderer.h"
#endif

#include <QPainter>
#include <QEvent>
#include <QApplication>
#include <QPaintEvent>
#include <QMenu>
#include <QClipboard>
#include "Variable.h"
#include "Plots/MyQwtPlotPicker.h"
#include "InfoSender.h"

class MOOptPlot :  public QwtPlot
{
    Q_OBJECT
public:
    inline MOOptPlot(void);
    ~MOOptPlot(void){}
    inline void refresh(int);
#if QWT_VERSION >= 0x060000
    inline int getNearestPointIndex(QwtPlotCurve *,const QPointF &);
#else
    inline int getNearestPointIndex(QwtPlotCurve *,const QwtDoublePoint &);
#endif
    inline void setSelectionPoint(int,bool _add);
    inline void setSelectionPoints(QList<int>);
    inline QList<int> getSelectedPoints();

    inline void setShownPoint(int,bool _add);
    inline void setShownPoints(QList<int>);
    inline QList<int> getShownPoints();

    template<typename OptVarObjResultX, typename OptVarObjResultY>
    void setXYVar(OptVarObjResultX*,OptVarObjResultY*);


#if QWT_VERSION < 0x060000
    inline virtual void drawItems (QPainter *painter, const QRect &rect,
                                   const QwtScaleMap map[axisCnt], const QwtPlotPrintFilter &pfilter) const;
#endif
    inline void drawFrame(QPainter *p, const QRect& rect);
    inline virtual void paintEvent(QPaintEvent *e);

private:

    QString _xVarName,_yVarName;
    std::vector<double> _xData,_yData;


    QList<int> _selectedPoints;
    QList<int> _shownPoints;

    QwtPlotZoomer *_zoomer;
    QwtPlotCurve *_curve1;
    QwtPlotCurve *_curve2;
    MyQwtPlotPicker *_picker1;

    QAction* _actionCopy;
    QAction* _actionRefresh;

public slots:
    inline void onClicked(const QPointF & pos);
    inline void onExtSelectionChanged(QList<int> &);
    inline void onPickerAppended (const QPoint &pos);
    inline void popUpMenu(const QPoint &pos);
    inline void onCopyAsked();
    inline void onRefreshAsked();

signals :
    inline void selectionChanged(QList<int> &);
};


MOOptPlot::MOOptPlot()
{
    //_curve1 initialization
    _curve1 = new QwtPlotCurve();
    QwtSymbol sym1;
    sym1.setStyle(QwtSymbol::Ellipse);
    sym1.setSize(6);
    sym1.setPen(QPen(Qt::red));
    sym1.setBrush(QBrush(Qt::red));
#if QWT_VERSION >= 0x060000
    _curve1->setSymbol(&sym1);
#else
    _curve1->setSymbol(sym1);
#endif
    _curve1->setStyle(QwtPlotCurve::NoCurve);
    _curve1->setItemAttribute(QwtPlotItem::AutoScale,true);
    _curve1->attach(this);

    //_curve2 initialization
    _curve2 = new QwtPlotCurve();
    QwtSymbol sym2;
    sym2.setStyle(QwtSymbol::Ellipse);
    sym2.setSize(15);
    sym2.setPen(QPen(Qt::blue));
    sym2.setBrush(QBrush(Qt::blue));
#if QWT_VERSION >= 0x060000
    _curve2->setSymbol(&sym2);
#else
    _curve2->setSymbol(sym2);
#endif
    _curve2->setStyle(QwtPlotCurve::NoCurve);
    _curve2->setItemAttribute(QwtPlotItem::AutoScale,true);
    _curve2->attach(this);

    _selectedPoints.clear();

    // Style

    // Font
    QFont axisFont(QApplication::font().family(),8,QFont::Normal);
    axisFont.setStyleStrategy(QFont::PreferAntialias);
    this->canvas()->setFont(axisFont);
    this->setAxisFont(QwtPlot::xBottom,axisFont);
    this->setAxisFont(QwtPlot::yLeft,axisFont);

    // box
    QwtPlotCanvas *pPlotCanvas = static_cast<QwtPlotCanvas*>(this->canvas());
    pPlotCanvas->setFrameStyle(QFrame::NoFrame);
    this->setFrameStyle(QFrame::StyledPanel);

    // Colors
    QColor background = QColor(Qt::white);
    background.setAlpha(255);
    QPalette palette;
    palette.setColor(QPalette::Window, background);
    this->setPalette(palette);
    this->setCanvasBackground(background);


    // Zommer
    _zoomer = new QwtPlotZoomer(canvas());
    _zoomer->setMousePattern(QwtEventPattern::MouseSelect1, Qt::MidButton);
    _zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::NoButton);
    _zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::NoButton);

    // Picker
    _picker1 = new MyQwtPlotPicker(canvas());
    _picker1->setTrackerMode(QwtPicker::AlwaysOff);
#if QWT_VERSION >= 0x060000
    /*! @todo Subclass QwtPickerMachine and provide a transition function to enable selection flags.
      */
    connect(_picker1, SIGNAL(selected(QPointF)),this, SLOT(onClicked(QPointF)));
#else
    _picker1->setSelectionFlags(QwtPicker::PointSelection | QwtPicker::RectSelection );
    connect(_picker1, SIGNAL(selected(const QwtDoublePoint &)),this, SLOT(onClicked(const QPointF &)));
#endif
    //    connect(_picker1, SIGNAL(selected(const QwtDoubleRect &rect)),this, SLOT(onPickerSelected(const QwtDoubleRect &rect)));
    //    connect(_picker1, SIGNAL(appended (const QPoint &pos)),this, SLOT(onPickerAppended(const QPoint &pos)));


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

    /*this->setCanvasBackground(QColor(Qt::white));
    QPalette _pal = this->palette();
    _pal.setColor(QPalette::Window,Qt::white);
    _pal.setColor(QPalette::Background,Qt::white);
    this->setPalette(_pal);
    this->canvas()->setPalette(_pal);

    this->setBackgroundRole(QPalette::Background);*/


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

    // margins
    this->setContentsMargins(5,5,5,5);
    //this->canvas()->setContentsMargins(5,5,5,5);

}

void MOOptPlot::setSelectionPoint(int selectedPoint, bool add)
{
    if(add)
    {
        if(!_selectedPoints.contains(selectedPoint))
        {
            _selectedPoints.push_back(selectedPoint);
            refresh(2);
        }
    }
    else
    {
        if((_selectedPoints.size()!=1)||(selectedPoint != _selectedPoints.at(0)))
        {
            _selectedPoints.clear();
            if(selectedPoint!=-1)
                _selectedPoints.push_back(selectedPoint);

            refresh(2);
        }
    }
}

void MOOptPlot::setShownPoint(int shownPoint, bool add)
{
    if(add)
    {
        if(!_shownPoints.contains(shownPoint))
        {
            _shownPoints.push_back(shownPoint);
            refresh(2);
        }
    }
    else
    {
        if((_shownPoints.size()!=1)||(shownPoint != _shownPoints.at(0)))
        {
            _shownPoints.clear();
            _shownPoints.push_back(shownPoint);
            refresh(2);
        }
    }
}


void MOOptPlot::setSelectionPoints(QList<int>  selectedPoints)
{
    if(_selectedPoints != selectedPoints)
    {
        _selectedPoints = selectedPoints;
        refresh(2);
    }
}

void MOOptPlot::setShownPoints(QList<int>  shownPoints)
{
    _shownPoints = shownPoints;
    // remove selected that are not shown anymore
    int i=0;
    while(i<_selectedPoints.size())
    {
        if(!_shownPoints.contains(_selectedPoints.at(i)))
            _selectedPoints.removeAt(i);
        else
            i++;
    }
    refresh(0);
}

QList<int> MOOptPlot::getSelectedPoints()
{
    return _selectedPoints;
}

QList<int> MOOptPlot::getShownPoints()
{
    return _shownPoints;
}

void MOOptPlot::refresh(int iCurve = 0)
{
    // iCurve = 0 -> refresh both curves
    // iCurve = 1 -> refresh _curve1 only
    // iCurve = 2 -> refresh _curve2 only

    double* dataX;
    double* dataY;
    int iPoint;

    if((iCurve==0)||(iCurve==1))
    {
        //Plot _curve1
        int nbPoints = _shownPoints.size();

        dataX = new double[nbPoints];
        dataY = new double[nbPoints];

        for (int i=0;i<nbPoints;i++)
        {
            iPoint = _shownPoints.at(i);
            if(iPoint<std::min(_xData.size(),_yData.size()))
            {
                dataX[i]=_xData.at(_shownPoints.at(i));
                dataY[i]=_yData.at(_shownPoints.at(i));
            }
            else
            {
                QString msg;
                msg.sprintf("MOOptPlot : tried to reach point number %d but data.length = %d",
                            iPoint,std::min(_xData.size(),_yData.size()));
                InfoSender::instance()->debug(msg);
            }
        }
#if QWT_VERSION >= 0x060000
        _curve1->setSamples(dataX,dataY,nbPoints);
#else
        _curve1->setData(dataX,dataY,nbPoints);
#endif
        _curve1->setItemAttribute(QwtPlotItem::AutoScale,true);
        delete[] dataX;
        delete[] dataY;
    }

    if((iCurve==0)||(iCurve==2))
    {
        if(_selectedPoints.size()>0)
        {
            dataX = new double[_selectedPoints.size()];
            dataY = new double[_selectedPoints.size()];

            for(int i=0;i<_selectedPoints.size();i++)
            {
                iPoint = _selectedPoints.at(i);
                if(iPoint<std::min(_xData.size(),_yData.size()))
                {
                    dataX[i]=_xData.at(_selectedPoints.at(i));
                    dataY[i]=_yData.at(_selectedPoints.at(i));
                }
                else
                {
                    QString msg;
                    msg.sprintf("MOOptPlot : tried to reach point number %d but data.length = %d",
                                iPoint,std::min(_xData.size(),_yData.size()));
                    InfoSender::instance()->debug(msg);
                }
            }
#if QWT_VERSION >= 0x060000
            _curve2->setSamples(dataX,dataY,_selectedPoints.size());
#else
            _curve2->setData(dataX,dataY,_selectedPoints.size());
#endif
            _curve2->setItemAttribute(QwtPlotItem::AutoScale,true);
            _curve2->show();
            delete[] dataX;
            delete[] dataY;
        }
        else
        {
            _curve2->hide();
        }
    }
    replot();
    if((iCurve==0)||(iCurve==1))
        _zoomer->setZoomBase();

}

template<typename OptVarObjResultX, typename OptVarObjResultY>
void MOOptPlot::setXYVar(OptVarObjResultX* varX,OptVarObjResultY* varY)
{
    _xVarName = varX->name();
    _yVarName = varY->name();

    _xData = varX->finalValuesAtScan(0).toStdVector();
    _yData = varY->finalValuesAtScan(0).toStdVector();


    // check that selected and shown points index are compatible with new values
    int index=0;
    int nbPoints = _xData.size();
    while(index<_shownPoints.size())
    {
        if(_shownPoints.at(index)>=nbPoints)
            _shownPoints.removeAt(index);
        else
            index++;
    }

    index=0;
    while(index<_selectedPoints.size())
    {
        if(_selectedPoints.at(index)>=nbPoints)
            _selectedPoints.removeAt(index);
        else
            index++;
    }


    refresh();
}

#if QWT_VERSION >= 0x060000
int MOOptPlot::getNearestPointIndex(QwtPlotCurve * curve,const QPointF & point)
#else
int MOOptPlot::getNearestPointIndex(QwtPlotCurve * curve,const QwtDoublePoint & point)
#endif
{

    double minDist=0;
    double curDist=0;

    int xPos = transform(QwtPlot::xBottom,point.x());
    int yPos = transform(QwtPlot::yLeft,point.y());

    int xPointPos;
    int yPointPos;

    int indexMin = -1;

    for(int iShown=0;iShown<_shownPoints.size();iShown++)
    {
        xPointPos = transform(QwtPlot::xBottom,_xData.at(_shownPoints.at(iShown)));
        yPointPos = transform(QwtPlot::yLeft,_yData.at(_shownPoints.at(iShown)));

        curDist = sqrt(pow((double)(yPointPos-yPos),2)+pow((double)(xPointPos-xPos),2));
        if((curDist<minDist)|| (iShown==0))
        {
            indexMin = _shownPoints.at(iShown);
            minDist = curDist;
        }
    }
    return indexMin;
}

void MOOptPlot::onClicked(const QPointF & pos)
{

    QwtPlotPicker *picker = qobject_cast<QwtPlotPicker *>(sender());

    int provSelectedPoint = getNearestPointIndex(_curve1,pos);

    setSelectionPoint(provSelectedPoint,_picker1->ctrlPressed);

    // emit signal to synchronize
    QList<int> list = getSelectedPoints();
    emit selectionChanged(list);
}

#if QWT_VERSION < 0x060000
void MOOptPlot::drawItems (QPainter *painter, const QRect &rect,
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

void MOOptPlot::drawFrame(QPainter *p, const QRect& rect)
{
    p->save();

    int lw = 1;
    QRect r = rect.adjusted(lw, lw, -lw - 1, -lw - 1);

    p->fillRect(r,palette().color(QPalette::Window));
    p->setBrush(QBrush(Qt::white));
    QwtPainter::drawRect(p, r);

    p->restore();
}

void MOOptPlot::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    drawFrame(&p, rect());
    e->accept();
}

void MOOptPlot::onExtSelectionChanged(QList<int> & newSel)
{
    if(_selectedPoints != newSel)
    {
        setSelectionPoints(newSel);
    }
}


//*********************
//    Picker
//*********************
void MOOptPlot::onPickerAppended (const QPoint &pos)
{
    bool ctrlPressed = _picker1->ctrlPressed;
}


void MOOptPlot::popUpMenu(const QPoint &pos)
{
    QMenu *menu = new QMenu;
    menu->insertAction(NULL,_actionCopy);
    menu->insertAction(NULL,_actionRefresh);

    /*
    menu->addAction(QString("Copy"), this, SLOT(onCopyAsked()), QKeySequence::Copy);
    menu->addAction(QString("Replot"), this, SLOT(onRefreshAsked()));*/
    menu->exec(this->mapToGlobal(pos));

}

void MOOptPlot::onRefreshAsked()
{
    qDebug("replot");
    replot();
}

void MOOptPlot::onCopyAsked()
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
    QList<QwtPlotCurve*> curves;
    curves << _curve1 << _curve2;
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
