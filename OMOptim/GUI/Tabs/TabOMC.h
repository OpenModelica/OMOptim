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

     @file tabOMC.h
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version
*/

#ifndef TABOMC_CLASS_H
#define TABOMC_CLASS_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui/QWidget>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QHeaderView>
#include <QtGui/QTextEdit>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QtGui>
#include <QtGui/QKeyEvent>
#include <QtGui/QTextBlock>
#endif

#include "OptimResult.h"
#include "ui_TabOMC.h"
#include "MOomc.h"
#include "Plots/MOOptPlot.h"
#include "Project.h"

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
#include "Tabs/MOTab.h"

namespace Ui {
    class TabOMC_Class;
}


class TabOMC : public MOTabSimple {
    Q_OBJECT

public:
    TabOMC(Project *project, QWidget *parent = 0);
    ~TabOMC();
    TabType tabType(){return TABOTHER;};



        Project *project;



    //UI
    Ui::TabOMC_Class *ui;



public slots:
    void returnPressed();
    void insertNewline();
    void prevCommand();
    void nextCommand();
    void goHome( bool shift );
    //void codeCompletion( bool same );
    void codeNextField();

    void cut();
    void copy();
    void paste();







private:
    void createMoshEdit();
    //void createMoshError();
    void createAction();
    void createMenu();
    void createToolbar();
        void exceptionInEval(std::exception &e);
    void addCommandLine();
    void selectCommandLine();
    QStringList getFunctionNames(QString);


    QTextCursor cursor_;
    QTextEdit* moshEdit_;
    //QTextEdit* moshError_;
    QVBoxLayout* layout_;
    QString clipboard_;
    QString omc_version_;


    //IAEX::CommandCompletion* commandcompletion_;
    int fontSize_;

    int currentFunction_;
    QString currentFunctionName_;
    QStringList* functionList_;

    int currentCommand_;
    QStringList* commands_;
    QTextCharFormat commandSignFormat_;
    QTextCharFormat textFormat_;



};

//********************************
class MyTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    MyTextEdit( QWidget* parent = 0 );
    virtual ~MyTextEdit();
    void sendKey( QKeyEvent *event );

signals:
    void returnPressed();
    void insertNewline();
    void prevCommand();
    void nextCommand();
    void goHome( bool shift );
    void codeCompletion( bool same );
    void codeNextField();

protected:
    void keyPressEvent(QKeyEvent *event);
    void insertFromMimeData(const QMimeData *source);    // Added 2006-01-30

private:
    bool insideCommandSign();
    bool startOfCommandSign();
    bool sameTab_;
};








#endif // TABOMC_CLASS_H
