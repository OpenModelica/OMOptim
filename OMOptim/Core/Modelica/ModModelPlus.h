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

        @file ModModelPlus.h
        @brief Comments for file documentation.
        @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
        Company : CEP - ARMINES (France)
        http://www-cep.ensmp.fr/english/
        @version

  */
#if !defined(_MODMODELPLUS_H)
#define _MODMODELPLUS_H


#include <QObject>
#include <QFileInfoList>

#include "MOItem.h"
#include "MOVector.h"

class Project;
class InfoSender;
class Variables;
class ModModel;
class ModelicaConnections;
class MOomc;
class BlockSubstitution;
class ModPlusCtrl;
class ModelicaModifier;
class ModItem;
class Variable;

/**
* \brief ModModelPlus is a class dedicated to information storage concerning Modelica models.
*
* ModModelPlus is a class dedicated to information storage concerning Modelica models (ModModel).
* Each ModModelPlus is associated to a ModModel. ModModelPlus contains informations that are used within OMOptim and which concern corresponding ModModel.
* These informations are variables, connections, modifiers (deprecated), dependencies...
* It is also responsible for compiling and reading functions, though these functions require a controler (ModPlusCtrl).
*/
class ModModelPlus : public MOItem
{
    Q_OBJECT


public :

    //******************
    // Attributes
    //******************


protected :
    Variables *_variables; /// List of model variables
    ModelicaConnections *_connections; /// List of connections between components in Modelica model
    QString _modModelName; /// ModModelName
    QFileInfoList _neededFiles; /// List of files needed in the simulation folder.
    QStringList _neededFolders; /// List of folders needed in the simulation folder.
    QFileInfoList _moDependencies; /// List of .mo files needed to be loaded before simulation

    MOomc* _moomc;
    Project* _project;

    // status booleans
    bool _variablesRead; /// indicates whether variables have been read or not
    bool _connectionsRead;/// indicates whether variables have been read or not

    QString _mmoFilePath; /// File path of file where all informations are stored
    QString _infos;

public:

    ModModelPlus(Project*,QString modModelName);
    ~ModModelPlus(void);
    virtual QString getClassName(){return "ModModelPlus";};


    //*************************
    // Get / Set functions
    //*************************
    void setInfos(QString);
    QString infos();
    ModModel* modModel();
    QString getFieldName(int,int role){return "-";};
    unsigned getNbFields(){return 0;};

    void setModModelName(QString);
    void addMoDependency(const QFileInfo &);
    void addMoDependencies(const QFileInfoList &);
    void setMoDependencies(const QStringList &);
    void setMoDependencies(const QFileInfoList &);
    QFileInfoList moDependencies() const;

    //*************************
    // Path functions
    //*************************
    void setMmoFilePath(QString);
    void setmodelToConsider(QString);

    QString mmoFilePath();
    QString moFilePath();
    QString mmoFileName();
    QDir mmoFolder();
    QString modModelName();
    QFileInfoList neededFiles();
    QStringList neededFolders();


    //*************************
    // Read functions
    //*************************
    //virtual bool readElements(bool readAll = false);

    bool readAll(ModPlusCtrl*);
    void loadDependencies();

    //************************
    //  Variables
    //************************
    Variables* variables();
    Variables* variables(ModItem*);

    //************************
    //  Connections
    //************************
    ModelicaConnections* connections();
    //void outsideConnections(QList<ModItem*> &ports, QList<ModItem*> &outsideComps);
    //void outsideConnections(QStringList &ports, QStringList &outsideComps);
    //void outsideConnections(QStringList &uniqueports, QList<QStringList> &outsideComps);

    //************************
    // Block substitutions
    //************************
    bool applyBlockSub(BlockSubstitution *_blockSub,bool compile);


    //************************
    //  Main functions
    //************************
    virtual void clear();
    virtual void save();


    //************************
    //  Execution functions
    //************************
public slots :
    bool isCompiled(ModPlusCtrl* ctrl);
    bool compile(ModPlusCtrl* ctrl);


    //************************
    // Others
    //************************
public slots:
    void openMoFolder();
    void openMmoFolder();
    void openMoFile();
    void openDependenciesDlg();
    void openNeededFilesDlg();


    virtual bool readConnections();
    virtual bool readVariables(ModPlusCtrl*, bool forceRecompile=false);
    bool variablesRead() const;
    void reloadModel();


protected :

    //**************************************
    // Variables
    //**************************************
    void addVariable(Variable*);

    //**************************************
    // Connections
    //**************************************
    bool readConnections(ModItem*,bool includeChildren);
    void addConnection(ModItem*, ModItem*);
    void addConnection(QString _aName,QString _bName);

signals:
    void saved();
    void loaded();
    void modified();
    void sendInfo(Info*);
    void componentsUpdated();
    void modifiersUpdated();
    void connectionsUpdated();
    void variablesUpdated();
};








#endif  //_MODEL_H


