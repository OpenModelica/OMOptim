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

    @file BlockSubstitutions.h
    @brief Comments for file documentation.
    @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
    Company : CEP - ARMINES (France)
    http://www-cep.ensmp.fr/english/
    @version

  */
#if !defined(_BLOCKSUBSTITUTIONS_H)
#define _BLOCKSUBSTITUTIONS_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtCore/QStringList>
#include <QtXml/QDomDocument>
#endif

#include "Problem.h"
#include "ModItemsTree.h"
#include "ModModelPlus.h"
#include "BlockSubstitution.h"


class Project;

class BlockSubstitutions : public QObject,public QVector<BlockSubstitution*>
{
    Q_OBJECT

public:
    BlockSubstitutions(void);

    BlockSubstitutions(Project* project,const QDomElement & el);

    ~BlockSubstitutions(void);

    BlockSubstitutions* clone() const;

    QStringList getReplacedComponentsNames();
    bool isReplacedComponent(QString);
    QList<int> getReplacedComponentIndexes(QString _name);
    QStringList getReplacingComponentNames(QString);
    QStringList getReplacingComponentNames(ModItem*);

    void add(BlockSubstitution*);

    BlockSubstitution* find(QString replacedName,QString replacingName);

    int getSize();
    BlockSubstitution* getAt(int i);


    bool removeBlock(QString replacedName,QString replacingName);
    bool removeBlocks(QString replacedName);


    QDomElement toXmlData(QDomDocument &);

signals:
    void changed();


};

#endif
