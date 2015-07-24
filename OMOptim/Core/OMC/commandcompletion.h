/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2008, Linkopings University,
 * Department of Computer and Information Science,
 * SE-58183 Linkoping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF THIS OSMC PUBLIC
 * LICENSE (OSMC-PL). ANY USE, REPRODUCTION OR DISTRIBUTION OF
 * THIS PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE OF THE OSMC
 * PUBLIC LICENSE.
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from Linkopings University, either from the above address,
 * from the URL: http://www.ida.liu.se/projects/OpenModelica
 * and in the OpenModelica distribution.
 *
 * This program is distributed  WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS
 * OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)
 * For more information about the Qt-library visit TrollTech's webpage
 * regarding the Qt licence: http://www.trolltech.com/products/qt/licensing.html
 */

// FILE/CLASS ADDED 2005-12-12 /AF

/*!
* \file commandcompetion.h
* \author Anders Fernstrom
* \date 2005-12-12
*/

#ifndef COMMANDCOMPLETION_H
#define COMMANDCOMPLETION_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtCore/QHash>
#include <QtCore/QStringList>
#include <QtGui/QTextCursor>
#include <QtXml/QDomDocument>
#include <QtCore/QFile>
#endif

//IAEX Headers
#include "commandunit.h"


namespace IAEX
{
    class CommandCompletion : public QObject
    {
        Q_OBJECT

    public:
        static CommandCompletion *instance( const QString &filename );
        bool insertCommand( QTextCursor &cursor );
        bool nextCommand( QTextCursor &cursor );
        QString helpCommand();
        bool nextField( QTextCursor &cursor );


    private:
        void initializeCommands();
        void parseCommand(QDomNode node, CommandUnit *item) const;
        CommandCompletion( const QString &filename );

        static CommandCompletion *instance_;
        QDomDocument *doc_;

        int currentCommand_;
        int currentField_;
        int commandStartPos_;
        int commandEndPos_;

        QStringList *currentList_;
        QStringList commandList_;
        QHash<QString,CommandUnit*> commands_;
    };
}
#endif
