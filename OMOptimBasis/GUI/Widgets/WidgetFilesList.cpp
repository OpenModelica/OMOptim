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
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)

     @file WidgetFilesList.cpp
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version
*/

#include "Widgets/WidgetFilesList.h"
#include "ui_WidgetFilesList.h"


WidgetFilesList::WidgetFilesList(QFileInfoList & filesList,QWidget *parent,QString filter,QString infos,bool editable):
    QWidget(parent),_filesList(filesList),
    _ui(new Ui::WidgetFilesListClass)
{
    _ui->setupUi(this);
    _filter = filter;

    for(int i=0;i<_filesList.size();i++)
        _ui->filesList->addItem(_filesList.at(i).absoluteFilePath());

    setInfos(infos);

    _editable = editable;
    if(!editable)
    {
        _ui->pushAddFiles->hide();
        _ui->pushRemoveFiles->hide();
    }

    connect(_ui->pushAddFiles,SIGNAL(clicked()),this,SLOT(addFiles()));
    connect(_ui->pushRemoveFiles,SIGNAL(clicked()),this,SLOT(removeFiles()));
}

WidgetFilesList::~WidgetFilesList()
{
    delete _ui;
}



void WidgetFilesList::addFiles()
{
    //get last .mo folder
    QSettings settings("OMOptim", "Settings");
    QString lastMoFolder = settings.value("LastMoFolder").toString();

    QStringList fileNames = QFileDialog::getOpenFileNames(
            this,
            "Files to copy for simulation",
            lastMoFolder,
            _filter );

    addFiles(fileNames);
}

void WidgetFilesList::addFiles  (QStringList list)
{
    for(int i=0;i<list.size();i++)
    {
        if(!_filesList.contains(list.at(i)))
        {
            _ui->filesList->addItem(list.at(i));
            _filesList.push_back(list.at(i));
        }
    }
}

void WidgetFilesList::setInfos(const QString &infos)
{
    _ui->labelInfos->setText(infos);
    if(infos.isEmpty())
        _ui->labelInfos->hide();
    else
        _ui->labelInfos->show();
}

void WidgetFilesList::removeFiles()
{
        QList<QListWidgetItem*> itemsList = _ui->filesList->selectedItems();
        int iFile;
        for(int i=0;i<itemsList.size();i++)
    {
                iFile = _filesList.indexOf(itemsList.at(i)->text());
                if(iFile>-1)
                {
                    _filesList.removeAt(iFile);
                }
    }
        //remove from listWidget
        qDeleteAll(itemsList);
}


