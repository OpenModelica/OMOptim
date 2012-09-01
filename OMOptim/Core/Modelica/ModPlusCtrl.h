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

#include <QObject>
#include <QString>
#include <QFileInfo>

#include "MOVector.h"


#include "ModelPlus.h";



class Project;
class Variable;
class MOomc;
class MOParameters;


class ModPlusCtrl :public QObject
{
    Q_OBJECT

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
                DYMOLA,
                OMEXECUTABLE,
                DYMOLAEXECUTABLE,
                BLACKBOXEXECUTABLE
        };
        static const int nbTypes = 4;

        ModPlusCtrl(Project*,ModelPlus* ModPlus,MOomc* moomc);
        virtual ~ModPlusCtrl(void);
        virtual ModPlusCtrl* clone()=0;

        // Variables functions
        virtual bool readOutputVariables(MOVector<Variable> *,QString outputfile="") = 0;
        virtual bool readInitialVariables(MOVector<Variable> *,bool forceRecompile = false,QString initfile="") = 0;

        // compatible models
        virtual QList<ModelPlus::ModelType> compatibleModels() = 0;

        // Compile function
        virtual bool isCompiled() = 0;
        virtual bool compile(const QFileInfoList & moDependencies) = 0;

        // Info function
        virtual ModPlusCtrl::Type type()const = 0;
        virtual QString name() = 0;

        // Simulate function
        virtual bool simulate(QDir tempDir,MOVector<Variable> * inputVars,MOVector<Variable> * outputVars,
                              QFileInfoList filesToCopy ,QFileInfoList moDependencies) = 0;
        virtual void stopSimulation(){}
        virtual bool canBeStoped(){return false;}

        MOParameters* parameters() const;

        //bool operator==(const ModPlusCtrl &) const;

        friend bool operator==(const ModPlusCtrl & a,const ModPlusCtrl & b);


protected:
        ModelPlus* _ModelPlus;
        bool _copyAllMoOfFolder;
        MOParameters *_parameters;
        MOomc* _moomc;
        Project* _project;


signals :
        void modified();

};



#endif
