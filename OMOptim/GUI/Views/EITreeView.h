// $Id: EITreeView.h 9677 2011-08-24 13:09:00Z hubert.thieriot $
/**
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linkpings universitet, Department of Computer and Information Science,
 * SE-58183 Linkping, Sweden.
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
 * Main contributor 2011, Hubert Thierot, CEP - ARMINES (France)

 	@file EITreeView.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 
*/

#ifndef EITREEVIEW_H
#define EITREEVIEW_H

#include <QtGui/QDialog>
#include <QtGui/QWidget>
#include<QtGui/QFileDialog>

#include "MyTreeView.h"
#include "MyDelegates.h"
#include "EITree.h"
#include "MOTableView.h"
#include "GuiTools.h"
#include "WidgetEIGroup.h"
#include "WidgetSelectModModel.h"
#include "ModReader.h"
#include "ModClass.h"
#include "EIModelicaExtractor.h"
#include "EIControler.h"

namespace Ui {
    class EITreeViewClass;
}


class QErrorMessage;
class Project;

class EITreeView : public MyTreeView
{
    Q_OBJECT


public:
    explicit EITreeView(EITree* eiTree,bool editable,QWidget *parent,MOOptVector *variables=NULL);
    virtual ~EITreeView();

    EIItem* getContainer(QModelIndex index);
    void setEditable(bool editable);

public slots:

    void refresh();
    void onSelectItemChanged(QModelIndex);
    void updateCompleters(MOOptVector*);

protected :


private:
    EITree* _eiTree;
    bool _editable;
    MOOptVector *_variables;

    GenericDelegate *_tinUDlg;
    GenericDelegate *_toutUDlg;
    GenericDelegate *_qUDlg;
    GenericDelegate *_mUDlg;
    GenericDelegate *_htUDlg;

    VarCompleterDelegate *_compltDlg1;
    VarCompleterDelegate *_compltDlg2;
    VarCompleterDelegate *_compltDlg3;

signals :
    void selectGroupChanged(EIGroup*);
};

#endif 
