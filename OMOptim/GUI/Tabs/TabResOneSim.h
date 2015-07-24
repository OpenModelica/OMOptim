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

  @file tabResOneSim.h
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version
*/

#ifndef TABRESONESIM_H
#define TABRESONESIM_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui/QWidget>
#include "OptimResult.h"
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QHeaderView>
#include <QtGui/QSortFilterProxyModel>
#endif

#include "Project.h"
#include "Plots/MOOptPlot.h"

#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_scale_draw.h>
#include <qwt_math.h>
#include <qwt_picker.h>

#include "TabOneSim.h"

#include "Tabs/MO2ColTab.h"
#include "Widgets/WidgetTableVar.h"
#include "Widgets/WidgetSelPointScan.h"


class TabResOneSim : public MO2ColTab {
    Q_OBJECT

public:
    TabResOneSim(OneSimResult *result,QWidget *parent = 0);
    virtual ~TabResOneSim();
    TabType tabType(){return TABSOLVEDPROBLEM;};

    Project *_project;
    OneSimResult *_result;

private :
    WidgetTableVar* _inputTableVars;
    WidgetTableVar* _overTableVars;
    WidgetTableOptVar* _finalTableVars;
    WidgetCtrlParameters *_widgetCtrl;
    WidgetSelPointScan* _widgetSelPointScan;


};




#endif // TABRESONESIM_H
