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
 * @file WidgetTableVar.h
 * @brief Comments for file documentation.
 * @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 * Company : CEP - ARMINES (France)
 * http://www-cep.ensmp.fr/english/
 * @version
 */


// $Id$


#ifndef WidgetTableVar_H
#define WidgetTableVar_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui/QDialog>
#include <QtGui/QWidget>
#include<QtGui/QFileDialog>
#endif

#include "Variable.h"
#include "MOTableView.h"
#include "Tools/GuiTools.h"
#include "MOVector.h"

class MOOptVector;

class WidgetTableVar : public QWidget
{
    Q_OBJECT

public:
    WidgetTableVar(MOVector<Variable> *,QWidget *parent,bool addFilter=true);


public:

    QGridLayout* _allLayout;
    MOVector<Variable> *_variables;
    MOTableView *_tableVariables;

    void setEditable(bool);
public slots :
    void refresh();
};


class WidgetTableOptVar : public QWidget
{
    Q_OBJECT

public:
    WidgetTableOptVar(MOOptVector *,QWidget *parent,bool addFilter=true);


public:

    QGridLayout* _allLayout;
    MOOptVector *_variables;
    MOTableView *_tableVariables;

public slots :
    void refresh();
    void exportCSV();
};

/**
  * Widget for displaying a vector
  */
class WidgetTable : public QWidget
{
    Q_OBJECT



public:
    WidgetTable(QAbstractItemModel *vector,QWidget *parent) :QWidget(parent)
    {
        _vector = vector;
        this->setLocale(QLocale::C);

        // Layouts
        _allLayout = new QGridLayout(this);
        this->setLayout(_allLayout);

        // variables table
        _table = new MOTableView(this);
        _table->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);


        GuiTools::ModelToView(vector,_table);
        //GuiTools::minimizeTableSize(_table);
        _allLayout->addWidget(_table);
    }

    QList<int> selectedRows()
    {
        QList<int> result;
        QModelIndexList indexes = _table->selectionModel()->selectedIndexes();
        for(int i=0;i<indexes.size();i++)
        {
            if(!result.contains(indexes.at(i).row()))
                result.push_back(indexes.at(i).row());
        }
        result.removeAll(-1);
        return result;
    }

public:

    QGridLayout* _allLayout;
    QAbstractItemModel *_vector;
    MOTableView *_table;


};


#endif
