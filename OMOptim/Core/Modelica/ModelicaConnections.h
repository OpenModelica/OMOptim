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

 	@file ModelicaConnections.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#if !defined(_MODELICACONNECTIONS_H)
#define _MODELICACONNECTIONS_H

#include <QtCore/QAbstractItemModel>

#include "ModelicaConnection.h"
#include "MOVector.h"
#include "Modelica.h"
#include "ModClassTree.h"

class ModelicaConnections : public MOVector<ModelicaConnection>
{

public:
        ModelicaConnections(ModClassTree*);
	~ModelicaConnections(void);

private :
        ModClassTree* _modClassTree;

public :

	//void clear();

	//void addConnection(ModelicaConnection*);


	//// Abstract model overwrited functions
	//QVariant data(const QModelIndex &index, int role) const;
	//Qt::ItemFlags flags(const QModelIndex &index) const;
	//QVariant headerData(int section, Qt::Orientation orientation,
	//	int role = Qt::DisplayRole) const;
	//QModelIndex index(int row, int column,
	//	const QModelIndex &parent = QModelIndex()) const;
	//QModelIndex parent(const QModelIndex &index) const;
	//int rowCount(const QModelIndex &parent = QModelIndex()) const;
	//int columnCount(const QModelIndex &parent = QModelIndex()) const;


	//useful functions
public :
	ModelicaConnections* filter(ModClass*,bool includeChildren,bool acceptInternal);
	//void getOutside(ModClass* _element,bool includeChildren,QList<ModClass*> &_ports,QList<ModClass*> &_outsideComps);
	void getOutside(ModClass* _element,bool includeChildren,QStringList &ports, QStringList &outsideComps);
	void getOutside(ModClass* _element,bool includeChildren,QStringList &uniquePorts, QList<QStringList> &outsideComps);
	QList<ModClass*> getOutsideConnectedComps(ModClass*,bool includeChildren);
	QStringList getOutsideConnectedCompsNames(ModClass*,bool includeChildren,Modelica::NameFormat format);
	QList<ModClass*> getOutsideConnectingPorts(ModClass*,bool includeChildren);
	QStringList getOutsideConnectingPortsNames(ModClass*,bool includeChildren,Modelica::NameFormat format);

};


#endif
