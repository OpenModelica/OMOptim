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
 * @file WidgetMOItem.h
 * @brief Comments for file documentation.
 * @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 * Company : CEP - ARMINES (France)
 * http://www-cep.ensmp.fr/english/
 * @version
 */


// $Id$


#ifndef WIDGETMOITEM_H
#define WIDGETMOITEM_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui/QDialog>
#include <QtGui/QWidget>
#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>
#include <QtGui/QSpinBox>
#include <QtGui/QCheckBox>
#include <QtGui/QLabel>
#include <QtGui/QErrorMessage>
#include <QtGui/QListWidget>
#endif

#include "MOItem.h"
#include "ui_WidgetMOItem.h"
#include "Tools/qsciencespinbox.h"

namespace Ui {
    class WidgetMOItem;
}


class QErrorMessage;


class WidgetMOItem : public QWidget {
    Q_OBJECT


public:
    WidgetMOItem(MOItem *_item,QWidget *parent,bool editable=false);
    virtual ~WidgetMOItem();

    Ui::WidgetMOItemClass *ui;

public:
    void setItem(MOItem*);
    void setEditable(bool editable);

protected :

    MOItem* item;
    bool _editable;
    void clearGui();
    void initializeGui();
    QWidget* itemWidget;

    QMap<int,QWidget*> fieldEditWidget;

    virtual QWidget* createEditWidget(MOItem* item, int iField, bool editable)=0;

    void updateWidgetValue(QWidget*, QVariant);

    void changeValue(QWidget*,const QVariant &);

    public slots:
        void onValueChanged(const QString &);
        void onLineEditValueChanged();
        void onCheckChanged(int state);
        void actualizeValues();

signals :
        void valueChanged();
};

#endif
