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


#include "Variable.h"
#include "MOVector.h"
#include "MOomc.h"
#include "Info.h"
#include "ModLoader.h"
#include "ModelicaConnections.h"
#include "ModelicaModifier.h"
#include "BlockSubs/BlockSubstitution.h"
#include "MOSettings.h"
#include "ModModel.h"
#include "InfoSender.h"
#include "ModPlusCtrl.h"
#include "ModPlusDymolaCtrl.h"
#include "Variables.h"
#include "Dialogs/DlgSelectFiles.h"


using std::vector;
class Project;

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
    MOVector<ModelicaModifier> *_modifiers; /// @deprecated Never been used. Maybe in the future...
    QStringList _neededFiles; /// List of files needed in the simulation folder.
    QStringList _neededFolders; /// List of folders needed in the simulation folder.
    QStringList _moDependencies; /// List of .mo files needed to be loaded before simulation

    MOomc* _moomc;
    Project* _project;

    // status booleans
    bool _variablesRead; /// indicates whether variables have been read or not
    bool _modifiersRead; /// @deprecated Never been used. Maybe in the future... @sa _modifiers
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
    void addMoDependency(const QString &);
    void addMoDependencies(const QStringList &);
    void setMoDependencies(const QStringList &);
    QStringList moDependencies() const;

    //*************************
    // Path functions
    //*************************
    void setMmoFilePath(QString);
    void setMoFilePath(QString);
    void setmodelToConsider(QString);

    QString mmoFilePath();
    QString mmoFileName();
    QString mmoFolder();
    QString modModelName();
    QStringList neededFilesNames();
    QStringList neededFoldersNames();


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


