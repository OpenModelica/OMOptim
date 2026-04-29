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
 * @file ModLoader.h
 * @brief Comments for file documentation.
 * @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 * Company : CEP - ARMINES (France)
 * http://www-cep.ensmp.fr/english/
 * @version
 */


// $Id$

#ifndef _ModLoader_H
#define _ModLoader_H

#include "OMOptimModelica.h"
#include "ModItem.h"
#include "ModPackage.h"
#include "ModModel.h"
#include "ModRecord.h"
#include "ModComponent.h"
#include "MOSettings.h"

class ModModelPlus;
class ModItemsTree;

/**
  * ModLoader offers loading functions for openmodelica.
  * @sa MOomc
  */
class ModLoader : public QObject
{
    Q_OBJECT

public:

        ModLoader(MOomc *moomc);

    // Load functions
        bool loadMoFile(ModItemsTree* modItemsTree,QString filePath,QString &msg,bool forceLoad = true);
        void loadMoFiles(ModItemsTree* modItemsTree,QStringList filePaths, bool forceLoad = true);
        void unloadMoFile(ModItemsTree* modItemsTree,QString filePath);

        int getDepthMax();

private :
        MOomc* _moomc;

};





#endif
