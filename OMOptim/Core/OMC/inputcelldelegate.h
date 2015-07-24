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

#ifndef _INPUTCELLDELEGATE_H
#define _INPUTCELLDELEGATE_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtCore/QObject>
#include <QtCore/QString>
#endif

namespace IAEX
{
   /*!
    * \interface InputCellDelegate
    *
    * \brief Describes what members a delegate to the inputcell should
    * implement.
    *
    * This interface should be subclassed in order to extend the
    * applications functionality to evaluate another type of
    * expressions. At the moment a Modelica environment is
    * implemented, and also a Standard ML environment is implemented.
    *
    */
    class InputCellDelegate : public QObject
   {
       Q_OBJECT


   public:
      virtual QString result() = 0;
      virtual void evalExpression(QString& expr) = 0;
      virtual void closeConnection() = 0;                // Added 2006-02-02 AF
        virtual void reconnect() = 0;                        // Added 2006-02-09 AF
      virtual bool startDelegate() = 0;                    // Added 2006-02-09 AF
   };

}
#endif
