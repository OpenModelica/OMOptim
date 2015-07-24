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
 * \file commandunit.h
 * \author Anders Fernstrom
 */

#ifndef COMMANDUNIT_H
#define COMMANDUNIT_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtCore/QHash>
#include <QtCore/QString>
#endif

namespace IAEX
{
    /*!
     * \class CommandUnit
     * \author Anders Fernstrom
     * \date 2005-12-12
     *
     * \brief A class that store an omc command
     */
    class CommandUnit
    {
    public:
        CommandUnit( QString name ) : name_(name){}
        virtual ~CommandUnit(){}

        QString name(){ return name_; }
        QString fullName()
        {
            QString tmp = name_;
            QHash<QString,QString>::iterator d_iter = datafields_.begin();
            while( d_iter != datafields_.end() )
            {
                tmp.replace( d_iter.key(), d_iter.value() );
                ++d_iter;
            }

            return tmp;
        }
        QString helptext(){ return helptext_; }

        int numbersField(){ return datafields_.size(); }
        QString datafield( QString fieldID )
        {
            if( datafields_.contains( fieldID ))
                return datafields_[fieldID];
            else
                return QString::null;
        }
        void addDataField( QString fieldID, QString data )
        {
            datafields_[fieldID] = data;
        }

        void setHelptext( QString text ){ helptext_ = text; }

    private:
        QString name_;
        QHash<QString,QString> datafields_;
        QString helptext_;
    };
}

#endif
