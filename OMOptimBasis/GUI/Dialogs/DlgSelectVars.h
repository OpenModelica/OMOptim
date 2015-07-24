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

     @file DlgSelectVars.h
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version
*/

#if !defined(_DlgSelectVars_H)
#define _DlgSelectVars_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui/QWidget>
#include <QtGui/QDialog>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QHeaderView>
#include <QtGui/QSpinBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtCore/QObject>
#include <QtCore/QObjectList>
#include <QtGui/QScrollBar>
#endif

#include "MOVector.h"
#include "Variable.h"
#include "Widgets/WidgetSelectVars.h"

class DlgSelectVars : public QDialog
{
    Q_OBJECT

public:
        DlgSelectVars(MOVector<Variable>*,MOVector<Variable>* alreadySelected = NULL);
        DlgSelectVars(MOVector<Variable>* variables,QList<VariableCausality> causalities,MOVector<Variable>* alreadySelected = NULL);
        DlgSelectVars(MOOptVector *,MOOptVector *alreadySelected = NULL);



        static MOVector<Variable>* getSelectedVars(MOVector<Variable>*,MOVector<Variable>* alreadySelected=NULL);
        static MOVector<Variable>* getSelectedVars(MOVector<Variable>*,QList<VariableCausality> causalities,MOVector<Variable>* alreadySelected=NULL);

        static MOOptVector* getSelectedOptVars(MOOptVector *,MOOptVector *_alreadySelected = NULL);


private :
        bool _useOpt;
        MOVector<Variable> *_variables;
        MOVector<Variable>* _selectedVars;

        MOOptVector *_optVariables;
        MOOptVector* _selectedOptVars;


    QPushButton *pushOk;
    QPushButton *pushCancel;

    WidgetSelectVars* widgetSelectVars;

public slots:
    void pushedOk();
    void pushedCancel();



};



#endif
