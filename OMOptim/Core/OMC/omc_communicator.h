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

#ifndef OMC_COMMUNICATOR_H
#define OMC_COMMUNICATOR_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtCore/QObject>
#endif

// Omc communication interface
#include "omc_communication.h"
#include "vqtconvert.h"


/*
 * \brief The OmcCommunicator handles all low level communication with Omc.
 *
 * This class is a singleton class, use the static getInstance() member function to obtain a
 * reference to its instance.
 */
class OmcCommunicator : public QObject
{
  Q_OBJECT

public:
  ~OmcCommunicator();
   static OmcCommunicator& getInstance();

   bool establishConnection();
   void closeConnection();
   bool isConnected() const;

   QString callOmc(const QString& fnCall);

private:
   // Enforce the singleon's uniqueness.
   OmcCommunicator();
   OmcCommunicator(const OmcCommunicator&);
   OmcCommunicator& operator=(const OmcCommunicator&);

   OmcCommunication_var omc_;
signals :
   void omcOutput(QString);
};

#endif

