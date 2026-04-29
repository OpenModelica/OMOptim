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

#ifndef MODEXEPLUS_H
#define MODEXEPLUS_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtCore/QObject>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>
#include <QtCore/QStringList>
#include <QFileInfoList>
#endif

#include "ModelPlus.h"

class ExeModel;

class ModExePlus : public ModelPlus

{
    Q_OBJECT


public :


    ModExePlus(Project*,QString modModelName);
    ModExePlus(Project*,const QDomElement & );
    virtual ~ModExePlus();

    virtual QString getClassName() const {return ModExePlus::className();}
    static QString className(){return "ModExePlus";}

    // model type
    virtual ModelType modelType(){return ModelPlus::EXECUTABLE;}

    // files
    QFileInfo exeFile();
    QFileInfo inputFile();

    //*************************
    // Get / Set functions
    //*************************

    ExeModel* modModel();


    void setModModelName(QString);

    //*************************
    // Path functions
    //*************************
    virtual QDomElement toXmlData(QDomDocument &doc);

    //*************************
    // Read functions
    //*************************
    bool readAll(ModPlusCtrl*);

    //************************
    //  Main functions
    //************************
    virtual void clear();
    virtual void save();


    //************************
    //  Execution functions
    //************************
    bool isCompiled(ModPlusCtrl* ctrl);

public slots :
    bool compile(ModPlusCtrl* ctrl,QFileInfoList filesToCopy = QFileInfoList());


    //************************
    // Others
    //************************
    virtual bool readVariables(ModPlusCtrl*, bool forceRecompile = false);
    bool variablesRead() const;


};







#endif // MODEXEPLUS_H
