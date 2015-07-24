// $Id: WidgetSelectEditVars.h 15938 2013-04-30 12:51:39Z hudson $
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

    @file WidgetSelectEditVars.h
    @brief Comments for file documentation.
    @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
    Company : CEP - ARMINES (France)
    http://www-cep.ensmp.fr/english/
    @version
*/

#ifndef WidgetSelectEditVars_H
#define WidgetSelectEditVars_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui/QDialog>
#include <QtGui/QWidget>
#include<QtGui/QFileDialog>
#include <QtGui/QErrorMessage>
#endif

#include "ui_WidgetSelectEditVars.h"
#include "OptimResult.h"
#include "MOTableView.h"
#include "Tools/GuiTools.h"
#include "MyDelegates.h"
#include "OptObjectives.h"

namespace Ui {
class WidgetSelectEditVarsClass;
}


class QErrorMessage;


class WidgetSelectEditVars : public QWidget {
    Q_OBJECT


public:
    explicit WidgetSelectEditVars(Project* project,bool isEditable,bool showAllVariablesOnFront,QWidget *parent);
    //WidgetSelectEditVars(QWidget* parent);
    virtual ~WidgetSelectEditVars();



public:
    Ui::WidgetSelectEditVarsClass *_ui;

    void addPermanentVars(Variables*);
    void addPermanentVar(const Variable &);
    void removePermanentVars(QString model);
    void setShownColumns();
    void setDelegates();
    void initialize();




public slots :
    void actualizeGui();
    void addOptVariables();
    void deleteOptVariables();
    void addOptObjectives();
    void deleteOptObjectives();
    void addSamplingVariables();
    void deleteSamplingVariables();
    void addOverVariables();
    void deleteOverVariables();
    void readVariables();
    void refreshAllModelsVars();


protected:

    void initToolBar();
    void initShownTables();
    QList<Variable *> getSelectedVariables();
    // functions to overwrite
    // if not for one, no table use for corresponding variables
    virtual OptVariables* optimizedVariables() const {return NULL;}
    virtual ScannedVariables* scannedVariables() const {return NULL;}
    virtual Variables* overwritedVariables() const {return NULL;}
    virtual OptObjectives* objectives() const {return NULL;}
    virtual QStringList models() const {return QStringList();}
    virtual ModPlusCtrl* ctrl(QString model) const {return NULL;}

    QMap<MOTableView*,QList<int> > _hiddenFields;
    int iColumn(int iField, MOTableView *table) const;



    Project* _project;
    bool _showAllVariablesOnFront;
    bool _isEditable;
    Variables *_allModelsVars; /// Variables of optimization models
    Variables *_permanentVars; /// Variables that do not come from models

    QSortFilterProxyModel *_variableProxyModel;
    QSortFilterProxyModel *_optVariableProxyModel;
    QSortFilterProxyModel *_objectiveProxyModel;
    QSortFilterProxyModel *_samplingProxyModel;
    QSortFilterProxyModel *_overVarsProxyModel;

    MOTableView *_tableVariables;
    MOTableView *_tableOptimizedVars;
    MOTableView *_tableSamplingVars;
    MOTableView *_tableOverVars;
    MOTableView *_tableObjectives;

    QAction* _actionReadVariables;
    QAction* _actionShowSampling;
    QAction* _actionShowParameters;
    QAction* _actionShowObjectives;
    QAction* _actionShowOptimized;

    void clearDelegates();
    QList<QStyledItemDelegate*> _delegates;
};

#endif
