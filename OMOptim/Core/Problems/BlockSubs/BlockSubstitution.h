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
    @file BlockSubstitution.h
    @brief Comments for file documentation.
    @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
    Company : CEP - ARMINES (France)
    http://www-cep.ensmp.fr/english/
    @version

  */
#if !defined(_BLOCKSUBSTITUTION_H)
#define _BLOCKSUBSTITUTION_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtCore/QStringList>
#include <QtXml/QDomDocument>
#endif


class Project;
class ModModelPlus;
class BlockSubstitution

{
public:

    BlockSubstitution(Project* project,QString model, QString orgComponent, QString subComponent,
                      QStringList orgPorts,QList<QStringList> orgConnectedComps,
                      QStringList subPorts,QList<QStringList> subConnectedComps);

    BlockSubstitution(Project*,QString model,QString orgComponent,QString subComponent,bool doAutoConnect, bool &ok);
    BlockSubstitution(Project*,QDomElement domEl,bool &ok);


    ~BlockSubstitution(void);

    void setSubComponent(QString subComponent,bool doAutoConnect);

private :
    bool init(Project*,QString model,QString _orgComponent,QString _subComponent);

public :
    void autoConnect();

public :

    BlockSubstitution* clone() const;
    void copyFrom(BlockSubstitution*);

    QDomElement toXmlData(QDomDocument & doc);

    Project *_project;
    QString _model;

    QString _orgComponent;
    QString _subComponent;

    QStringList _orgPorts; // ports inside original component
    QList<QStringList> _orgConnectedComps; // components connected to original component ports

    QStringList _subPorts; // corresponding ports inside replacing component
    QList<QStringList> _subConnectedComps; // components connected to replacing component ports

};



#endif
