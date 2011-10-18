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

        @file ModPlusCtrl.h
        @brief Comments for file documentation.
        @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
        Company : CEP - ARMINES (France)
        http://www-cep.ensmp.fr/english/
        @version

  */
#ifndef _MODPLUSCTRL_H
#define _MODPLUSCTRL_H

#include "Modelica.h"
#include "ModClass.h"
#include "ModPackage.h"
#include "ModModel.h"
#include "ModComponent.h"
#include "MOSettings.h"
#include "MOParameter.h"

class ModModelPlus;
class Project;

class ModPlusCtrl :public QObject
{
    /*! \class ModPlusCtrl
    * \brief Class providing control functions for ModModelPlus.
    *
    *  Nearly all functions defined in this class are pure virtual.
    *  They concern reading variables, simulation and parameters.
    *  This class should be inherited and implemented for each software used to simulate Modelica models.
    *  Currently, two inheriting classes exist : ModPlusOMCtrl working with OpenModelica and ModPlusDymolaCtrl working with Dymola.
    */

        public:

        enum Type
        {
                OPENMODELICA,
                DYMOLA
        };

        ModPlusCtrl(Project*,ModModelPlus* modModelPlus,MOomc* moomc,QString mmoFolder,QString moFilePath,QString modModelName);
        ~ModPlusCtrl(void);

        // Variables functions
        virtual bool readOutputVariables(MOVector<Variable> *,QString folder="") = 0;
        virtual bool readInitialVariables(MOVector<Variable> *,QString folder="") = 0;

        // Compile function
        virtual bool isCompiled() = 0;
        virtual bool compile(const QStringList & moDependencies=QStringList()) = 0;

        // Parameters
        virtual void setDefaultParameters() = 0;

        // Info function
        virtual ModPlusCtrl::Type type() = 0;
        virtual QString name() = 0;

        // Simulate function
        virtual bool simulate(QString tempDir,MOVector<Variable> * inputVars,MOVector<Variable> * outputVars,
                              QStringList filesToCopy = QStringList(),QStringList moDependencies=QStringList()) = 0;
        virtual void stopSimulation(){}
        virtual bool canBeStoped(){return false;}

        virtual void setMmoFolder(QString);
        void setMoFilePath(QString);

        MOParameters* parameters();

protected:
        ModModelPlus* _modModelPlus;
        QString _mmoFolder;
        QString _moFilePath;
        QString _modModelName;
        bool _copyAllMoOfFolder;
        MOParameters *_parameters;
        MOomc* _moomc;
        Project* _project;

};

#endif
