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

// $Id$
/**
@file ModPlusDymolaCtrl.h
@brief Comments for file documentation.
@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
Company : CEP - ARMINES (France)
http://www-cep.ensmp.fr/english/
@version

*/
#ifndef _MODPLUSDYMOLACTRL_H
#define _MODPLUSDYMOLACTRL_H


#include <QtCore>
#include <limits>

#include "ModPlusCtrl.h"
#include "DymolaParameters.h"




class ModModelPlus;
class MOomc;
class Project;
class Variable;

class ModPlusDymolaCtrl :public ModPlusCtrl
{


public:

        ModPlusDymolaCtrl(Project* project,ModModelPlus* model,MOomc* omc);
        ~ModPlusDymolaCtrl(void);
        ModPlusCtrl* clone();

        ModPlusCtrl::Type type() const;
        QString name();

        // Variables functions
        bool readOutputVariables(MOVector<Variable> *,QString path);
        bool readOutputVariablesDSRES(MOVector<Variable> *,QString dsresFile);
        bool readOutputVariablesDSFINAL(MOVector<Variable> *,QString dsfinalFile);
        bool readInitialVariables(MOVector<Variable> *, const QFileInfoList filesToCopy,bool forceRecompile, QString dsinFile="");

        // compatible models
        virtual QList<ModelPlus::ModelType> compatibleModels();


        // Compile function
        bool createDsin(QFileInfoList moDeps,QFileInfoList filesToCopy);
        bool isCompiled();
        bool uncompile();
        bool compile(const QFileInfoList & moDependencies, const QFileInfoList filesToCopy);

        // Simulate function
        bool simulate(QDir tempDir,MOVector<Variable> * updatedVars,MOVector<Variable> * outputVars,
                      QFileInfoList filesTocopy,QFileInfoList moDependencies);
        void stopSimulation();
        bool canBeStoped();
        bool setStopTime(double time);



private:
        QString _dsinFile;
        QString _dsresFile;
        QString _dsfinalFile;
        QProcess _simProcess;
        ModModelPlus *_modModelPlus; /// cast of _ModelPlus

};



#endif
