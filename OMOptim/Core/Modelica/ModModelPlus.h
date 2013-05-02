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


#include "ModelPlus.h"
#include "MOVector.h"

class Project;
class InfoSender;
class Variables;
class ModModel;
//class ModelicaConnections;
class MOomc;
//class BlockSubstitution;
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
class ModModelPlus : public ModelPlus
{
    Q_OBJECT


public :

    //******************
    // Attributes
    //******************


protected :

  //  ModelicaConnections *_connections; /// List of connections between components in Modelica model
    QFileInfoList _moDependencies; /// List of .mo files needed to be loaded before simulation
    MOomc* _moomc;

    // status booleans
    bool _connectionsRead;/// indicates whether variables have been read or not


public:

    ModModelPlus(Project* project,QString modModelName);
    ModModelPlus(Project* project,const QDomElement & );
    virtual ~ModModelPlus();

    virtual QString getClassName() const {return ModModelPlus::className();};
    static QString className(){return "ModModelPlus";};

    // model type
    virtual ModelType modelType(){return ModelPlus::MODELICA;}

    //*************************
    // Get / Set functions
    //*************************

    void addMoDependency(const QFileInfo &);
    void addMoDependencies(const QFileInfoList &);
    void setMoDependencies(const QStringList &);
    void setMoDependencies(const QFileInfoList &);
    QFileInfoList moDependencies() const;
    QString getFieldName(int,int role){return "-";};
    unsigned getNbFields( ) const {return 0;};
    //*************************
    // Path functions
    //*************************
    void setmodelToConsider(QString);

    QString moFilePath();
    QString mmoFileName();

    //*************************
    // Read functions
    //*************************
    bool readAll(ModPlusCtrl*);
    void loadDependencies();


    //************************
    //  Connections
    //************************
 //   ModelicaConnections* connections();

    //************************
    // Block substitutions
    //************************
//    bool applyBlockSub(BlockSubstitution *_blockSub,bool compile);


    //************************
    //  Main functions
    //************************
    virtual void clear();
    virtual void uncompile();
    virtual void save();


    //**************************************
    // Variables
    //**************************************
    void addVariable(Variable*);
    virtual QDomElement toXmlData(QDomDocument &doc);
    //**************************************
    // Connections
    //**************************************
//    bool readConnections(ModItem*,bool includeChildren);
//    void addConnection(ModItem*, ModItem*);
//    void addConnection(QString _aName,QString _bName);


    //************************
    //  Execution functions
    //************************

    virtual  bool isCompiled(ModPlusCtrl* ctrl);

public slots :
    bool compile(ModPlusCtrl* ctrl,QFileInfoList filesToCopy = QFileInfoList());


    //************************
    // Others
    //************************
public slots:
    void openMoFolder();
    void openMmoFolder();
    void openMoFile();
    void openDependenciesDlg();
    void openNeededFilesDlg();


  //  virtual bool readConnections();

    virtual bool readVariables(ModPlusCtrl*, QFileInfoList filesToCopy = QFileInfoList(), bool forceRecompile=false);

    bool variablesRead() const;
    void reloadModel();


public :


signals:
    void saved();
    void loaded();
    void modified();
    void sendInfo(Info*);
};








#endif  //_MODEL_H


