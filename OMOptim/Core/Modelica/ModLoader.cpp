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

        @file ModLoader.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version

  */
#include "ModLoader.h"
#include "ModItemsTree.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QMutex>


ModLoader::ModLoader(MOomc *moomc)
{
    _moomc = moomc;
}



void ModLoader::loadMoFile(ModItemsTree* modItemsTree,QString filePath,bool forceLoad)
{
    // Read it in moomc
    if( !QFile::exists(filePath))
    {
        InfoSender::instance()->send(Info(ListInfo::FILENOTEXISTS,filePath));
        return ;
    }

    // loading file in moomc
    QString error;
    bool loadOk;


    // Load file
    _moomc->loadModel(filePath,forceLoad,loadOk,error);


    // clear tree
    modItemsTree->clear();


    // reread first elements of modItemsTree
    modItemsTree->readFromOMCWThread(modItemsTree->rootElement(),2);

}



void ModLoader::loadMoFiles(ModItemsTree* modItemsTree,QStringList filePaths, bool forceLoad)
{
    QString error;
    bool loadOk;

    for(int i=0;i<filePaths.size();i++)
    {
        // Read it in moomc
        if( !QFile::exists(filePaths.at(i)))
        {
            InfoSender::instance()->send(Info(ListInfo::FILENOTEXISTS,filePaths.at(i)));
        }
        else
        {
            // loading file in moomc
            _moomc->loadModel(filePaths.at(i),forceLoad,loadOk,error);
        }
    }

    // clear tree
    modItemsTree->clear();

    // reread first elements of modItemsTree
    modItemsTree->readFromOMCWThread(modItemsTree->rootElement(),2);
}

void ModLoader::unloadMoFile(ModItemsTree *modItemsTree, QString filePath)
{
    InfoSender::sendCurrentTask("Unloading .mo file : "+filePath);
    // loading file in moomc
    QString error;
    bool loadOk;

    // get classNames of file
    QStringList classNames = _moomc->getClassesOfFile(filePath);

    // delete classes
    for(int i=0;i<classNames.size();i++)
        _moomc->deleteClass(classNames.at(i));

    // clear tree
    modItemsTree->clear();


    // reread first elements of modItemsTree
    modItemsTree->readFromOMCWThread(modItemsTree->rootElement(),2);
}

int ModLoader::getDepthMax()
{
    int depthMax = MOSettings::value("DepthReadWhileLoadingModModel").toInt();
    if(depthMax==-1)
        depthMax = 100000;
    return depthMax;
}




