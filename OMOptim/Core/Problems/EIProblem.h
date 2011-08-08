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

        @file EIProblem.h
        @brief Comments for file documentation.
        @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
        Company : CEP - ARMINES (France)
        http://www-cep.ensmp.fr/english/
        @version 0.9

  */

#ifndef PROBLEMEI_H
#define PROBLEMEI_H

#include "Problem.h"
#include "EIItem.h"
#include "EIModelContainer.h"
#include "EIModelExtractor.h"
#include "EIConnConstrs.h"
#include "EIReader.h"
#include "MOomc.h"
#include "EITree.h"
#include "EIControler.h"
#include "EIHEN1Parameters.h"

class EIProblem : public Problem
{
    Q_OBJECT

public:
        EIProblem(Project*,ModClassTree*,MOomc*);
        EIProblem(Project* project,ModClassTree* modClassTree,MOomc* moomc,QDomElement domProblem);
        EIProblem(const EIProblem &);
        virtual ~EIProblem(void);

        virtual void loadModel(ModClassTree*,ModModel*);
        virtual void unloadModel(ModModel*,bool &ok);


        virtual QDomElement toXmlData(QDomDocument &doc);
        virtual Problem* clone(){return new EIProblem(*this);};
        virtual Result* launch(ProblemConfig){return NULL;};
        virtual bool checkBeforeComp(QString&){return false;};

        static QString className(){return "EIProblem";};
        virtual QString getClassName(){return EIProblem::className();};

        void clearInputVars();
        void updateInputVars(MOOptVector *);
        void setInputVars(MOOptVector*);
        MOOptVector * inputVars();
        EITree* eiTree();
        void setEITree(const EITree &);

        QList<ModModel*> _modelsLoaded;

        MOomc* _moomc;
        MOOptVector * _inputVars;


        EIConnConstrs* connConstrs();
        void setConnConstrs(EIConnConstrs*);


protected :
        EIConnConstrs *_connConstrs; // connnection constraints
        EITree* _eiTree;



signals :
        void inputVarsModified();
};


#endif
