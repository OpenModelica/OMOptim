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
        @version 0.9

  */
#if !defined(_MODMODELPLUS_H)
#define _MODMODELPLUS_H


#include "Variable.h"
#include "MOVector.h"
#include "MOomc.h"
#include "Info.h"
#include "ModReader.h"
#include "ModelicaConnections.h"
#include "ModelicaModifier.h"
#include "EIStream.h"
#include "BlockSubstitution.h"
#include "TableEIItems.h"
#include "MOSettings.h"
#include "ModModel.h"
#include "InfoSender.h"
#include "ModPlusCtrl.h"
#include "ModPlusDymolaCtrl.h"


using std::vector;
class Project;


/**
* \brief ModModelPlus is a class dedicated to information storage concerning Modelica models.
*
* Each ModModelPlus is associated to a ModModel. ModModelPlus contains informations that are used within OMOptim and which concern corresponding ModModel.
* These informations are variables, connections, modifiers (deprecated), dependencies...
*/
class ModModelPlus : public MOItem
{
        Q_OBJECT


public :

        //******************
        // Attributes
        //******************


protected :
        MOVector<Variable> *_variables;
        ModModel *_modModel;
        ModelicaConnections *_connections;
        MOVector<ModelicaModifier> *_modifiers;
        QStringList _neededFiles;
        QStringList _neededFolders;

        MOomc* _moomc;
        Project* _project; // project or database (for signal emitting)
        ModClassTree* _modClassTree;


        //**************************************
        // Controller
        //**************************************
        QMap<ModPlusCtrl::Type,ModPlusCtrl*> _modPlusCtrls;

        // status booleans
        bool _isDefined;
        bool _elementsRead;
        bool _variablesRead;
        bool _modifiersRead;
        bool _connectionsRead;

        QString _mmoFilePath; // .mmo filepath
        QStringList _otherFiles;
        QString _infos;
        ModPlusCtrl::Type _ctrlType;

public:

        ModModelPlus(MOomc*,Project*,ModClassTree*,ModModel*);
        ~ModModelPlus(void);
        virtual QString getClassName(){return "ModModelPlus";};


        //*************************
        // Get / Set functions
        //*************************
        void setOtherFiles(QStringList);
        void setInfos(QString);
        QStringList otherFiles();
        QString infos();
        void clearOtherFiles();
        bool isDefined();
        ModModel* modModel();
        QString getFieldName(int,int role){return "-";};
        unsigned getNbFields(){return 0;};
        ModPlusCtrl* ctrl();
        QMap<ModPlusCtrl::Type,ModPlusCtrl*>* ctrls();
        ModPlusCtrl::Type ctrlType();
        void setModModel(ModModel*);

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

        bool readAll();

        //************************
        //  Variables
        //************************
        MOVector<Variable>* variables(bool readIfNot=false);
        MOVector<Variable>* variables(ModClass*);

        //************************
        //  Connections
        //************************
        ModelicaConnections* connections();
        //void outsideConnections(QList<ModClass*> &ports, QList<ModClass*> &outsideComps);
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
        bool isCompiled();
        bool compile();




        //************************
        // Others
        //************************
        public slots:
                void openMoFolder();
                void openMmoFolder();
                void openMoFile();
//		void setCtrlOpenModelica();
//		void setCtrlDymola();
                void setCtrlType(ModPlusCtrl::Type);
                void setCtrlType();
                void openParametersDlg();

        virtual bool readConnections();
        virtual bool readVariables();
        void reloadModel();



protected :




        //**************************************
        // Variables
        //**************************************
        void addVariable(Variable*);

        //**************************************
        // Connections
        //**************************************
        bool readConnections(ModClass*,bool includeChildren);
        void addConnection(ModClass*, ModClass*);
        void addConnection(QString _aName,QString _bName);



signals:
        void saved();
        void loaded();
        void modified();
        void sendInfo(Info*);
        void componentsUpdated();
        void modifiersUpdated();
        void connectionsUpdated();
        void sendProgress(float);
        void sendProgress(float,int,int);
};








#endif  //_MODEL_H


