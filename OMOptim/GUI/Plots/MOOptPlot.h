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
#include <QPainter>
#include <QEvent>
#include <QApplication>
#include <QPaintEvent>

#include "Variable.h"
#include "Plots/MyQwtPlotPicker.h"

class MOOptPlot :  public QwtPlot
{
    Q_OBJECT 
public:
    inline MOOptPlot(void);
    ~MOOptPlot(void){};
    inline void refresh(int);
    inline int getNearestPointIndex(QwtPlotCurve *,const QwtDoublePoint &);
    inline void setSelectionPoint(int,bool _add);
    inline void setSelectionPoints(QList<int>);
    inline QList<int> getSelectedPoints();

    inline void setShownPoint(int,bool _add);
    inline void setShownPoints(QList<int>);
    inline QList<int> getShownPoints();

    template<typename OptVarObjResultX, typename OptVarObjResultY>
    void setXYVar(OptVarObjResultX*,OptVarObjResultY*);
    


    inline virtual void drawItems (QPainter *painter, const QRect &rect,
            const QwtScaleMap map[axisCnt], const QwtPlotPrintFilter &pfilter) const;
    inline void drawFrame(QPainter *p, const QRect& rect);
    inline virtual void paintEvent(QPaintEvent *e);





private:
    
    QString xVarName,yVarName;
    std::vector<double> xData,yData;
    
    
    QList<int> selectedPoints;
    QList<int> shownPoints;

    QwtPlotCurve *curve1;
    QwtPlotCurve *curve2;
    MyQwtPlotPicker *picker1;
    QwtPlotZoomer *zoomer;

public slots:
    inline void onClicked(const QwtDoublePoint & pos);
    inline void onExtSelectionChanged(QList<int> &);
    inline void onPickerSelected(const QwtDoubleRect &rect);
    inline void onPickerAppended (const QPoint &pos);

signals :
    inline void selectionChanged(QList<int> &);
};


MOOptPlot::MOOptPlot()
{
    //Curve1 initialization
    curve1 = new QwtPlotCurve();
    QwtSymbol sym1;
    sym1.setStyle(QwtSymbol::Ellipse);
    sym1.setSize(6);
    sym1.setPen(QPen(Qt::red));
    sym1.setBrush(QBrush(Qt::red));
    curve1->setSymbol(sym1);
    curve1->setStyle(QwtPlotCurve::NoCurve);
    curve1->setItemAttribute(QwtPlotItem::AutoScale,true);
    curve1->attach(this);

    //Curve2 initialization
    curve2 = new QwtPlotCurve();
    QwtSymbol sym2;
    sym2.setStyle(QwtSymbol::Ellipse);
    sym2.setSize(15);
    sym2.setPen(QPen(Qt::blue));
    sym2.setBrush(QBrush(Qt::blue));
    curve2->setSymbol(sym2);
    curve2->setStyle(QwtPlotCurve::NoCurve);
    curve2->setItemAttribute(QwtPlotItem::AutoScale,true);
    curve2->attach(this);

    selectedPoints.clear();
    
    // Style
    
    // Font
    QFont _axisFont(QApplication::font().family(),8,QFont::Normal);
    _axisFont.setStyleStrategy(QFont::PreferAntialias);
    this->canvas()->setFont(_axisFont);
    this->setAxisFont(QwtPlot::xBottom,_axisFont);
    this->setAxisFont(QwtPlot::yLeft,_axisFont);

    // box
    this->canvas()->setFrameStyle(QFrame::NoFrame);
    this->setFrameStyle(QFrame::StyledPanel);

    // Colors
    QColor background = QColor(Qt::white);
    background.setAlpha(255);
    QPalette palette;
    palette.setColor(QPalette::Window, background);
    this->setPalette(palette);
    this->setCanvasBackground(background);
    



    // Zommer
    zoomer = new QwtPlotZoomer(canvas());
    zoomer->setMousePattern(QwtEventPattern::MouseSelect1, Qt::RightButton);
    zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::MidButton);
    zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::NoButton);

    // Picker
    picker1 = new MyQwtPlotPicker(canvas());
    picker1->setTrackerMode(QwtPicker::AlwaysOff);
    picker1->setSelectionFlags(QwtPicker::PointSelection | QwtPicker::RectSelection );
    connect(picker1, SIGNAL(selected(const QwtDoublePoint &)),this, SLOT(onClicked(const QwtDoublePoint &)));
//    connect(picker1, SIGNAL(selected(const QwtDoubleRect &rect)),this, SLOT(onPickerSelected(const QwtDoubleRect &rect)));
//    connect(picker1, SIGNAL(appended (const QPoint &pos)),this, SLOT(onPickerAppended(const QPoint &pos)));
    

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
    grid->setMajPen(QPen(Qt::lightGray, 0, Qt::SolidLine));
    grid->setMinPen(QPen(Qt::lightGray, 0 , Qt::SolidLine));
    grid->attach(this);

    // margins
    this->setContentsMargins(5,5,5,5);
    //this->canvas()->setContentsMargins(5,5,5,5);

}

void MOOptPlot::setSelectionPoint(int _selectedPoint, bool _add)
{
    if(_add)
    {
        if(!selectedPoints.contains(_selectedPoint))
        {
            selectedPoints.push_back(_selectedPoint);
            refresh(2);
        }
    }
    else
    {
        if((selectedPoints.size()!=1)||(_selectedPoint != selectedPoints.at(0)))
        {
            selectedPoints.clear();
            if(_selectedPoint!=-1)
                selectedPoints.push_back(_selectedPoint);
            
            refresh(2);
        }
    }
}

void MOOptPlot::setShownPoint(int _shownPoint, bool _add)
{
    if(_add)
    {
        if(!shownPoints.contains(_shownPoint))
        {
            shownPoints.push_back(_shownPoint);
            refresh(2);
        }
    }
    else
    {
        if((shownPoints.size()!=1)||(_shownPoint != shownPoints.at(0)))
        {
            shownPoints.clear();
            shownPoints.push_back(_shownPoint);
            refresh(2);
        }
    }
}


void MOOptPlot::setSelectionPoints(QList<int>  _selectedPoints)
{
    if(_selectedPoints != selectedPoints)
    {
        selectedPoints = _selectedPoints;
        refresh(2);
    }
}

void MOOptPlot::setShownPoints(QList<int>  _shownPoints)
{
        shownPoints = _shownPoints;
        refresh(0);
    }

QList<int> MOOptPlot::getSelectedPoints()
{
    return selectedPoints;
}

QList<int> MOOptPlot::getShownPoints()
{
    return shownPoints;
}

void MOOptPlot::refresh(int iCurve = 0)
{
    // iCurve = 0 -> refresh both curves
    // iCurve = 1 -> refresh curve1 only
    // iCurve = 2 -> refresh curve2 only

    double* _dataX;
    double* _dataY;
        int iPoint;

     if((iCurve==0)||(iCurve==1)) 
    {
        //Plot curve1
        int nbPoints = shownPoints.size();

        _dataX = new double[nbPoints];
        _dataY = new double[nbPoints];

        for (int i=0;i<nbPoints;i++)
        {
                    iPoint = shownPoints.at(i);
                    if(iPoint<std::min(xData.size(),yData.size()))
                    {
            _dataX[i]=xData.at(shownPoints.at(i));
            _dataY[i]=yData.at(shownPoints.at(i));
        }
                    else
                    {
                        QString msg;
                        msg.sprintf("MOOptPlot : tried to reach point number %d but data.length = %d",
                                    iPoint,std::min(xData.size(),yData.size()));
                        InfoSender::instance()->debug(msg);
                    }
        }
        curve1->setRawData(_dataX,_dataY,nbPoints);
        curve1->setItemAttribute(QwtPlotItem::AutoScale,true);
    }

    if((iCurve==0)||(iCurve==2)) 
    {
        if(selectedPoints.size()>0)
        {
            _dataX = new double[selectedPoints.size()];
            _dataY = new double[selectedPoints.size()];

            for(int i=0;i<selectedPoints.size();i++)
            {
                            iPoint = selectedPoints.at(i);
                            if(iPoint<std::min(xData.size(),yData.size()))
                            {
                _dataX[i]=xData.at(selectedPoints.at(i));
                _dataY[i]=yData.at(selectedPoints.at(i));
            }
                            else
                            {
                                QString msg;
                                msg.sprintf("MOOptPlot : tried to reach point number %d but data.length = %d",
                                            iPoint,std::min(xData.size(),yData.size()));
                                InfoSender::instance()->debug(msg);
                            }
            }
            curve2->setRawData(_dataX,_dataY,selectedPoints.size());
            curve2->setItemAttribute(QwtPlotItem::AutoScale,true);
            curve2->show();
        }
        else
        {
            curve2->hide();
        }
    }
    replot();
    if((iCurve==0)||(iCurve==1))
        zoomer->setZoomBase();
    
}

template<typename OptVarObjResultX, typename OptVarObjResultY>
void MOOptPlot::setXYVar(OptVarObjResultX* _varX,OptVarObjResultY* _varY)
{
    xVarName = _varX->name();
    yVarName = _varY->name();

    xData = _varX->finalValuesAtScan(0);
    yData = _varY->finalValuesAtScan(0);


    // check that selected and shown points index are compatible with new values
    int index=0;
    int nbPoints = xData.size();
    while(index<shownPoints.size())
    {
        if(shownPoints.at(index)>=nbPoints)
            shownPoints.removeAt(index);
        else
            index++;
    }

    index=0;
    while(index<selectedPoints.size())
    {
        if(selectedPoints.at(index)>=nbPoints)
            selectedPoints.removeAt(index);
        else
            index++;
    }
    

    refresh();
}

int MOOptPlot::getNearestPointIndex(QwtPlotCurve * curve,const QwtDoublePoint & point)
{

    double minDist=0;
    double curDist=0;

    int xPos = transform(QwtPlot::xBottom,point.x());
    int yPos = transform(QwtPlot::yLeft,point.y());

    int xPointPos;
    int yPointPos;

    int indexMin = -1;

    for(int iShown=0;iShown<shownPoints.size();iShown++)
    {
        xPointPos = transform(QwtPlot::xBottom,xData.at(shownPoints.at(iShown)));
        yPointPos = transform(QwtPlot::yLeft,yData.at(shownPoints.at(iShown)));

        curDist = sqrt(pow((double)(yPointPos-yPos),2)+pow((double)(xPointPos-xPos),2));
        if((curDist<minDist)|| (iShown==0))
        {
            indexMin = shownPoints.at(iShown);
            minDist = curDist;
        }
    }
    return indexMin;
}

void MOOptPlot::onClicked(const QwtDoublePoint & pos)
{

    QwtPlotPicker *_picker = qobject_cast<QwtPlotPicker *>(sender());

    int provSelectedPoint = getNearestPointIndex(curve1,pos);

    setSelectionPoint(provSelectedPoint,picker1->ctrlPressed);
    
    // emit signal to synchronize
    QList<int> list = getSelectedPoints();
    emit selectionChanged(list);
}
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

void MOOptPlot::onExtSelectionChanged(QList<int> & _newSel)
{
    if(selectedPoints != _newSel)
    {
        setSelectionPoints(_newSel);
    }
}


//*********************
//    Picker
//*********************
void MOOptPlot::onPickerSelected(const QwtDoubleRect &rect)
{
    bool ctrlPressed = picker1->ctrlPressed;
}

void MOOptPlot::onPickerAppended (const QPoint &pos)
{
    bool ctrlPressed = picker1->ctrlPressed;
}






#endif
