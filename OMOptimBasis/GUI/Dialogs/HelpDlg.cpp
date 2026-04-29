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
 * @file HelpDlg.cpp
 * @brief Comments for file documentation.
 * @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 * Company : CEP - ARMINES (France)
 * http://www-cep.ensmp.fr/english/
 * @version
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES, Paris, France
 * Main contributor 2009, Hubert Thierot, CEP - ARMINES, Paris, France
 * Copyright:   Copyright (C) 2010 CEP - ARMINES. All rights reserved.
 */


// $Id: HelpDlg.cpp 15963 2013-05-02 19:17:13Z sjoelund.se $


#include "HelpDlg.h"
#include "ui_HelpDlg.h"

HelpDlg::HelpDlg( QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpDlgClass)
{
    ui->setupUi(this);
}

HelpDlg::~HelpDlg()
{
    delete ui;
}

TextDlg::TextDlg(QString text, QString title, QWidget *parent)
    :QDialog(parent)
{
    QGridLayout* layout = new QGridLayout(this);

    this->setWindowTitle(title);

    // add textviewer
    QTextBrowser *textBrower = new QTextBrowser(this);
    textBrower->setText(text);

    layout->addWidget(textBrower,0,0,1,1);

    // add spacer and ok button
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    layout->addWidget(buttonBox,1,0,1,1);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
}


