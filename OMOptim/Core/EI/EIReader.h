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
	bool copy(EIItem* org,EIItem* dest);

	// Read functions
	bool isInDescendants(EIItem* parent, QString fullName);
	EIItem* findInDescendants(EIItem* parent,QString fullName);
	
	// Extract functions
	QList<EIStream*> getStreams(EIItem* parent);
	QList<EIItem*> getItems(EIItem* parent,bool recursive,EI::Type filter = EI::GENERIC);

	QList<EIStream*> getValidStreams(EIItem*parent,MOOptVector *variables,bool onlyChecked);
	QStringList getAllItemNames(EIItem* item,EI::Type filter = EI::GENERIC);
	// Analyse functions
	void getValidTk(EIItem* parent, QList<METemperature> & Tk,MOOptVector *variables);
	
	//Edit functions
	void addEmptyStream(EIItem* parent);
	void addEmptyGroup(EIItem* parent);
	void removeItem(EIItem*);

	// Mult fact
	void getFirstGroupFact(EIItem*,EIGroupFact* &,EIGroup* &);


	//XML functions
	void setItems(QDomElement & domEl,EIItem* rootEI);

};



#endif
