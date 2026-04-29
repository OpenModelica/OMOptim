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

/*
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)
 * @file ModPlusOMCtrl.h
 * @brief Comments for file documentation.
 * @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 * Company : CEP - ARMINES (France)
 * http://www-cep.ensmp.fr/english/
 * @version
 */


// $Id$

#ifndef _MODPLUSOMCTRL_H
#define _MODPLUSOMCTRL_H

#include "ModPlusCtrl.h"
#include "OpenModelica.h"
#include <limits>

class ModModelPlus;

class ModPlusOMCtrl :public ModPlusCtrl
{
    public:

        ModPlusOMCtrl(Project* project,ModModelPlus* model,MOomc* oms);
        ~ModPlusOMCtrl(void);
        ModPlusCtrl* clone();

        ModPlusCtrl::Type type() const;
        QString name();
        bool useMat();
        QString resFile();

        // Variables functions
        bool readOutputVariables(MOVector<Variable> *,QString resFile="");
        bool readInitialVariables(MOVector<Variable> *, const QFileInfoList filesToCopy,bool forceRecompile,QString initFile="");


        // compatible models
        virtual QList<ModelPlus::ModelType> compatibleModels();


        // Compile function
        bool isCompiled();
        bool compile(const QFileInfoList & moDeps, const QFileInfoList filesToCopy);
        bool uncompile();
        bool createInitFile(const QFileInfoList & moDeps, const QFileInfoList filesToCopy );

        // Simulate function
        bool simulate(QDir tempDir,MOVector<Variable> * inputVars,MOVector<Variable> * outputVars,QFileInfoList filesToCopy ,QFileInfoList moDependencies);
        bool setStopTime(double time);

private :
        QString _initFileXml;
        QString _initFileTxt;
        QString _exeFile;
        ModModelPlus *_modModelPlus;



};



#endif
