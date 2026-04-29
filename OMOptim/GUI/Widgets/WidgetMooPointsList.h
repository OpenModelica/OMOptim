/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
 * VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the OSMC (Open Source Modelica Consortium)
 * Public License (OSMC-PL) are obtained from OSMC, either from the above
 * address, from the URLs:
 * http://www.openmodelica.org or
 * https://github.com/OpenModelica/ or
 * http://www.ida.liu.se/projects/OpenModelica,
 * and in the OpenModelica distribution.
 *
 * GNU AGPL version 3 is obtained from:
 * https://www.gnu.org/licenses/licenses.html#GPL
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

/*
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)
 * @file WidgetMooPointsList.h
 * @brief Comments for file documentation.
 * @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 * Company : CEP - ARMINES (France)
 * http://www-cep.ensmp.fr/english/
 * @version
 */


// $Id$


#ifndef  WidgetMooPointsList_H
#define WidgetMooPointsList_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui/QDialog>
#include <QtGui/QWidget>
#include <QtGui/QFileDialog>
#include <QtGui/QErrorMessage>
#endif

#include "Widgets/WidgetList.h"
#include "ParetoDominance.h"
#include "Optimization.h"
#include "OptimResult.h"
#include "ui_WidgetMooPointsList.h"

        namespace Ui {
    class WidgetMooPointsListClass;
}


class QErrorMessage;


class WidgetMooPointsList : public QWidget {
    Q_OBJECT


public:
    explicit WidgetMooPointsList(OptimResult* ,QWidget *parent);
    virtual ~WidgetMooPointsList();


    void setShownPoints(QList<int> _list);

public :
    void showOnlyPoints(QList<int> );
    void showAllPoints();
    void showParetoPoints();

public slots :
    void setOnlyPareto(bool);
    void onExtSelectionChanged(QList<int>&);
    void onIntSelectionChanged(QList<int>&);
    void recomputeSelectedPoints();
    void exportSelectedPoints();


signals:
    void selectionChanged(QList<int>&);
    void shownPointsChanged(QList<int>&);
    void pointsRecomputed();

public:
    Ui::WidgetMooPointsListClass *_ui;
    OptimResult* _result;
    WidgetList *_listPoints;

    MOOptVector* _selectedExportVariables;
};

#endif
