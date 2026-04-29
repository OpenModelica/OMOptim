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

#ifndef MODELPLUS_H
#define MODELPLUS_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QString>
#include <QDomElement>
#include <QFileInfoList>
#endif

#include "MOItem.h"


using std::vector;
class Project;
class ModPlusCtrl;
class Variable;
class Variables;
class Info;
class ModModel;
class ModItem;

/**
*
* ModelPlus is a class dedicated to store information concerning models (e.g. files concerned, variables, dependencies...)
* ModelPlus is an abstract class and should be inherited.
*/
class ModelPlus : public MOItem
{
    Q_OBJECT

public :
    enum ModelType
    {
        MODELICA,
        EXECUTABLE
    };
    //******************
    // Attributes
    //******************


protected :
    Variables *_variables; /// List of model variables
    QFileInfoList _neededFiles; /// List of files needed in the simulation folder.
    QFileInfoList _neededFolders; /// List of folders needed in the simulation folder.

    Project* _project;
    QString _modelName;      /// ModelName
    // status booleans
    bool _variablesRead; /// indicates whether variables have been read or not
    QStringList _otherFiles;
    QString _infos;

    QString _relMmoFilePath; /// relative file path of file where all informations are stored


public:

    ModelPlus(Project*,QString modelName);
    ModelPlus(Project* project,const QDomElement &);

    virtual ~ModelPlus();

    virtual QString getClassName() const {return ModelPlus::className();}
    static QString className(){return "ModelPlus";}

    // model type
    virtual ModelType modelType() = 0;

    //*************************
    // Get / Set functions
    //*************************
    void setOtherFiles(QStringList);
    QString infos();
    void setInfos(QString);
    QStringList otherFiles();

    void clearOtherFiles();
    QString getFieldName(int,int role) const {return "-";};
    unsigned getNbFields( ) const {return 0;};
    Project* getProject() ;
    ModModel* modModel();

    void setMmoFilePath(QString);

    QString modelName();
    QFileInfoList neededFiles();
    QFileInfoList neededFolders();
    virtual bool isCompiled(ModPlusCtrl* ctrl) =0;
    virtual bool compile(ModPlusCtrl* ctrl,QFileInfoList filesToCopy = QFileInfoList()) =0;
    QDir mmoFolder();

    virtual QFileInfo mmoFilePath();

    //************************
    //  Main functions
    //************************
    virtual void clear();
    virtual void save();

    //************************
    //  Variables
    //************************
    Variables* variables();
    Variables* variables(ModItem* element);

    //************************
    // Others
    //************************
    virtual QDomElement toXmlData(QDomDocument & doc);

public slots:

    virtual bool readVariables(ModPlusCtrl*, const QFileInfoList filesToCopy, bool forceRecompile=false);
    bool variablesRead() const;


protected :

    //**************************************
    // Variables
    //**************************************
    void addVariable(Variable*);

signals:
    void saved();
    void modified();
    void variablesUpdated();
};





#endif // MODELPLUS_H
