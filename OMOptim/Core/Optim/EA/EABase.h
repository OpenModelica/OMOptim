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

    @file EABase.h
    @brief Comments for file documentation.
    @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
    Company : CEP - ARMINES (France)
    http://www-cep.ensmp.fr/english/
    @version

  */
#if !defined(_EABASE_H)
#define _EABASE_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtCore/QObject>
#include <QVector>
#endif

#include <time.h> //needed in paradiseo (not included otherwhere!!)

#include "OptimAlgo.h"

class Project;
class Problem;
class Result;
//class BlockSubstitutions;
class ModModelPlus;
class ModItemsTree;
class ModelPlus;

/**
  * Base class for evolutionary algorithms (e.g. SPEA2, NSGA2)
  */
class EABase : public OptimAlgo
{
    Q_OBJECT

public:
    EABase(void);
    EABase(Project *project,Problem* problem);
    EABase(const EABase &);

    virtual ~EABase(void);

    virtual EABase* clone() const = 0;
    //virtual Result* launch(QString tempDir) = 0;


//    // subModels (for Optimization problems)
//    bool _useSubModels;
//    void setSubModels(QList<QList<ModelPlus *> >,QList<BlockSubstitutions*>);

protected:

    ModItemsTree* _modItemsTree;

    // for Optimization problems
//    QList<QList<ModelPlus*> > _subModels;
//    QList<BlockSubstitutions*> _subBlocks;

    // solve mixing pointdep-pointindep in bounds
    QVector<QVector<int> > _index;
    bool _quickEnd; // force end but keeps results
    bool _keepResults; // at  the end, keep results (is set to false only when stop() is called)

public slots:
    void quickEnd();
    void stop();

};

#endif
