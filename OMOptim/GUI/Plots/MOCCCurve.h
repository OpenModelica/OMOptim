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

 	@file MOCCCurve.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 
*/

#pragma once

#include <QtCore/QVector>
#include <QtCore/QString>
#include <QtCore/QStringList>

#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_symbol.h>
#include <qwt_scale_draw.h>
#include <qwt_math.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_grid.h>


#include "MEQflow.h"
#include "METemperature.h"




class MOCCCurve : public QwtPlotCurve
{

public :
	enum CurveType
{
	CCHOT,
	CCCOLD,
	GCC,
        ICCPROCESS,
        ICCUTILITIES,
	OTHER
};

public:
	MOCCCurve(int type);
	MOCCCurve(const MOCCCurve &);
        MOCCCurve(QString _name, const QList<MEQflow> & xData, const QList<METemperature> & yData);
	~MOCCCurve(void);

	void init();
        void setTUnit(METemperature::Units);
        void setQUnit(MEQflow::Units);

        void setData(const QList<MEQflow> & xData, const QList<METemperature> & yData);
        void clear();

	QString _name;

	QStringList _pointLabels;

        QList<MEQflow> XData();
        QList<METemperature> YData();

	int type();
	void setType(int type);

private:
	int _type;
        QList<MEQflow> _xData;
        QList<METemperature> _yData;

        QVector<double> _rawXData;
        QVector<double> _rawYData;

        MEQflow::Units _qflowUnit;
        METemperature::Units _TUnit;

        void updateRawXData();
        void updateRawYData();
};
