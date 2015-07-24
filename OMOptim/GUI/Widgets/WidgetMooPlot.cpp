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

        @file WidgetMooPlot.cpp
        @brief Comments for file documentation.
        @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
        Company : CEP - ARMINES (France)
        http://www-cep.ensmp.fr/english/
        @version
*/

#include "Widgets/WidgetMooPlot.h"
#include "ui_WidgetMooPlot.h"


WidgetMooPlot::WidgetMooPlot(OptimResult* result,QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::WidgetMooPlotClass)
{
    _ui->setupUi(this);

    _result = result;

    //***********
    //PLOT
    //***********
    _plot1 = new MOOptPlot();
    _plot1->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    _ui->layoutInsidePlot->addWidget(_plot1);
    _ui->layoutInsidePlot->setContentsMargins(10,10,10,10);

    // connect signals for selection changed
    connect(_plot1,SIGNAL(selectionChanged(QList<int>&)),
            this,SIGNAL(selectionChanged(QList<int>&)));


    // Connecting signals and slot
    connect(_ui->comboAbscissa,SIGNAL(activated(int)),
            this,SLOT(varSelectionChanged()));
    connect(_ui->comboOrdinate,SIGNAL(activated(int)),
            this,SLOT(varSelectionChanged()));

    // update combos and plot
    updateCombos();
    varSelectionChanged();
}

WidgetMooPlot::~WidgetMooPlot()
{
    delete _ui;
}

void WidgetMooPlot::onExtSelectionChanged(QList<int>& list)
{
    _plot1->onExtSelectionChanged(list);
}

void WidgetMooPlot::onExtShownPointsChanged(QList<int>& list)
{
    this->setShownPoints(list);
}

void WidgetMooPlot::varSelectionChanged()
{
    // getting variables names
    QString xVarName = _ui->comboAbscissa->itemData(_ui->comboAbscissa->currentIndex()).toString();
    QString yVarName = _ui->comboOrdinate->itemData(_ui->comboOrdinate->currentIndex()).toString();


    // updating short names
    QString shortXVarName = xVarName;


    QString shortYVarName = yVarName;




    QFont font;
    font.setPointSize(8);
    font.setBold(true);
    font.setStyleStrategy(QFont::PreferAntialias);

    QwtText xtitle;
    xtitle.setFont(font);
    xtitle.setText(shortXVarName);

    QwtText ytitle;
    ytitle.setFont(font);
    ytitle.setText(shortYVarName);

    _plot1->setAxisTitle(QwtPlot::yLeft,ytitle);
    _plot1->setAxisTitle(QwtPlot::xBottom,xtitle);

    VariableResult *XObjResult;
    VariableResult *YObjResult;
    VariableResult *XVarResult;
    VariableResult *YVarResult;
    int XType,YType; //0 = Obj, 1 = Var



    bool xfound = false;
    bool yfound = false;


    // looking in optObjectives
    VariableResult* XVar;
    XVar= _result->optObjectivesResults()->findItem(xVarName);
    if (XVar)
    {
        XObjResult = XVar;
        XType = 0;
        xfound = true;
    }
    else
    {
        //looking in optVariables
        XVar = _result->optVariablesResults()->findItem(xVarName);
        if(XVar)
        {
            XVarResult = XVar;
            XType = 1;
            xfound=true;
        }
    }

    VariableResult* YVar;
    YVar = _result->optObjectivesResults()->findItem(yVarName);
    if (YVar)
    {
        YObjResult = YVar;
        YType = 0;
        yfound = true;
    }
    else
    {
        //looking in optVariables
        YVar = _result->optVariablesResults()->findItem(yVarName);
        if(YVar)
        {
            YVarResult = YVar;
            YType = 1;
            yfound = true;
        }
    }


    // plot new variables
    if(xfound && yfound)
    {
        switch(XType)
        {
        case 0 :
            switch(YType)
            {
            case 0 :
                _plot1->setXYVar(XObjResult,YObjResult);
                break;

            case 1 :
                _plot1->setXYVar(XObjResult,YVarResult);
                break;
            }
            break;
        case 1 :
            switch(YType)
            {
            case 0 :
                _plot1->setXYVar(XVarResult,YObjResult);
                break;

            case 1 :
                _plot1->setXYVar(XVarResult,YVarResult);
                break;
            }
            break;
        }
    }
}

void WidgetMooPlot::updateCombos()
{
    // Clear
    _ui->comboAbscissa->clear();
    _ui->comboOrdinate->clear();

    // Adding OptVariables and OptObjectives in Combo
    int nbObj = _result->optObjectivesResults()->size();
    int nbOpt = _result->optVariablesResults()->size();
    QStringList shortNames;
    QStringList fullNames;
    QString curName;
    for (int i=0;i<nbObj;i++)
    {

        fullNames.push_back(_result->optObjectivesResults()->at(i)->name(Variable::FULL));
        shortNames.push_back(_result->optObjectivesResults()->at(i)->name(Variable::SHORT));

    }
    for (int i=0;i<nbOpt;i++)
    {
        if (!fullNames.contains(_result->optVariablesResults()->at(i)->name()))
        {
            fullNames.push_back(_result->optVariablesResults()->at(i)->name(Variable::FULL));
            shortNames.push_back(_result->optVariablesResults()->at(i)->name(Variable::SHORT));
        }
    }
    for(int i=0;i<fullNames.size();i++)
    {
        _ui->comboAbscissa->addItem(shortNames.at(i),fullNames.at(i));
        _ui->comboOrdinate->addItem(shortNames.at(i),fullNames.at(i));
    }

    if(fullNames.size()>1)
        _ui->comboOrdinate->setCurrentIndex(1);
}

void WidgetMooPlot::setShownPoints(QList<int> _list)
{
    _plot1->setShownPoints(_list);

}

