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

 	@file EIReader.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#ifndef _EIReader_H
#define _EIReader_H


#include "EIItem.h"
#include "EIStream.h"
#include "EIGroup.h"

#include "MOSettings.h"

class EIReader : public QObject
{
	Q_OBJECT

public:

	EIReader();

	//Copy functions
        static bool copy(EIItem* org,EIItem* dest);

	// Read functions
        static bool isInDescendants(EIItem* parent, QString fullName);
	
        static EIItem* findInDescendants(EIItem* parent,QString fullName);
        static EIItem* findInDescendants(EIItem* parent, EI::Type eiType, QVariant itemFieldValue, int iField);
	
	// Extract functions
        static QList<EIStream*> getStreams(EIItem* parent);
        static QList<EIItem*> getItems(EIItem* parent,bool recursive,EI::Type filter = EI::GENERIC);

        static QList<EIStream*> getValidStreams(EIItem*parent,MOOptVector *variables,bool onlyChecked);
        static QStringList getAllItemNames(EIItem* item,EI::Type filter = EI::GENERIC);
	// Analyse functions
        static void getValidTk(EIItem* parent, QList<METemperature> & Tk,MOOptVector *variables);
	

	// Mult fact
        static void getFirstParentGroupFact(EIItem*,EIGroupFact* &,EIGroup* &);

        // Streams filter and sort functions
        static QList<EIStream*> getStreamsAboveT(METemperature T,QList<EIStream*> allStreams,MOOptVector* variables);
        static QList<EIStream*> getStreamsBelowT(METemperature T,QList<EIStream*> allStreams,MOOptVector* variables);
        static void sortByCp(QList<EIStream*> & allStreams,Qt::SortOrder);

private:
        static bool CpLowerThan(const EIStream* s1, const EIStream* s2); //comparator used for sorting
        static bool CpGreaterThan(const EIStream* s1, const EIStream* s2); //comparator used for sorting



};



#endif
