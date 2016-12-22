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

     @file BlockSubstituteConnDlg.h
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version
*/

#ifndef BLOCKSUBSTITUTECONNDLG_H
#define BLOCKSUBSTITUTECONNDLG_H

#include <QDialog>
#include <QWidget>
#include <QComboBox>
#include <QFileDialog>
#include "Project.h"
#include "ui_BlockSubstituteConnDlg.h"
#include "BlockSubs/BlockSubstitution.h"
#include <QLabel>

namespace Ui {
    class BlockSubstituteConnDlg;
}


class QErrorMessage;


class BlockSubstituteConnDlg : public QDialog {
    Q_OBJECT


public:
    BlockSubstituteConnDlg(BlockSubstitution*,bool _isEditable,QWidget *parent = 0);
    ~BlockSubstituteConnDlg();


    bool isEditable;

    BlockSubstitution* orgBlockSub;
    BlockSubstitution* tmpBlockSub;

    void initCombos();
    void actualizeGui();

public:
    Ui::BlockSubstituteConnDlgClass *ui;

    public slots :

        void validate();
        //void actualizeComboConnections();
        void addPortCombo();




private :


    QList<QLabel*> portLabels;
    QList<QGridLayout*> portLayouts;
    QList<QList<QComboBox* > > portCombos;

    QStringList basicItems;
    QStringList basicItemsData;

    QComboBox* addCombo(QStringList items, QStringList itemsData,QString _portName,int iPos=-1,QString dataSel="");

    void fillBasicItems();
    void joinAuto();

};

#endif
