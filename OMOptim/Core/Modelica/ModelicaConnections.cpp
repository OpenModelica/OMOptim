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
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)

 	@file ModelicaConnections.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "ModelicaConnections.h"
#include "ModClass.h"

ModelicaConnections::ModelicaConnections(ModReader* _modReader)
{
	modReader = _modReader;
}

ModelicaConnections::~ModelicaConnections(void)
{
	clear();
}

//void ModelicaConnections::clear()
//{
//	for(int i=0;i<items.size();i++)
//	{
//		delete items.at(i);
//	}
//}
//
//void ModelicaConnections::addConnection(ModelicaConnection* newConnection)
//{
//	int index=items.size();
//	insertRow(index);//,createIndex(0,0));
//	beginInsertRows(QModelIndex(),index,index);
//	items.push_back(newConnection);
//	endInsertRows();
//}
//
//
//
//int ModelicaConnections::columnCount(const QModelIndex &parent) const
// {
//     return 2;
// }
//
// QVariant ModelicaConnections::data(const QModelIndex &index, int role) const
// {
//     if (!index.isValid())
//         return QVariant();
//
//     if (role != Qt::DisplayRole)
//         return QVariant();
//
//	 ModelicaConnection *connect = items.at(index.row());
//
//	 if(index.column()==0)
//	 {
//		 return connect->getA()->name(Modelica::DYMOLA);
//	 }
//	 if(index.column()==1)
//	 {
//		 return connect->getB()->name(Modelica::DYMOLA);
//	 }
//	 return QVariant();
// }
//
// Qt::ItemFlags ModelicaConnections::flags(const QModelIndex &index) const
// {
//     if (!index.isValid())
//         return Qt::ItemIsEnabled;
//
//     return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
// }
//
// QVariant ModelicaConnections::headerData(int section, Qt::Orientation orientation,int role) const
// {
//     if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
//	 {
//		 switch(section)
//		 {
//		 case 0:
//			 return "A Name";
//		 case 1:
//			 return "B Name";
//		 default :
//			 return "Unknown field";
//		 }
//	 }
//
//     return QVariant();
// }
//
// QModelIndex ModelicaConnections::index(int row, int column, const QModelIndex &parent)const
// {
//         return createIndex(row, column);
// }
//
// QModelIndex ModelicaConnections::parent(const QModelIndex &index) const
// {
//   return QModelIndex();
//
// }
//
// int ModelicaConnections::rowCount(const QModelIndex &parent) const
// {  
//		 return items.size();
// }
//

ModelicaConnections* ModelicaConnections::filter(ModClass* _element,bool includeChildren,bool acceptInternal)
{
	ModelicaConnections* elConn = new ModelicaConnections(modReader);
	QString elName = _element->name();
	QString a,b;
	ModelicaConnection* curConn;

	for(int i=0;i<this->items.size();i++)
	{
		curConn = this->items.at(i);
		a = curConn->getA();
		b = curConn->getB();

		if((QString::compare(elName,a,Qt::CaseInsensitive)==0)
			||(QString::compare(elName,b,Qt::CaseInsensitive)==0))
			elConn->addItem(curConn);
		else
		{
			if(includeChildren)
			{
				bool aIsInChildren = (modReader->findInDescendants(_element,a)!=NULL);

				if(acceptInternal && aIsInChildren)
					elConn->addItem(curConn);
				else
				{
					bool bIsInChildren = (modReader->findInDescendants(_element,b)!=NULL);
					bool isConcerned = (aIsInChildren || bIsInChildren);
					bool isInternal = (aIsInChildren && bIsInChildren);
				
					if(isConcerned && !(isInternal&&!acceptInternal))
						elConn->addItem(curConn);
				}
			}
		}
	}
	return elConn;
}




//void ModelicaConnections::getOutside(ModClass* _element,bool includeChildren,QList<ModClass*> &_ports,QList<ModClass*> &_outsideComps)
//{
//	ModelicaConnections* elConn = filter(_element,includeChildren,false);
//
//	ModClass* aModClass,*bModClass;
//	QString aName,bName;
//	for(int i=0;i<elConn->items.size();i++)
//	{
//		aName = elConn->items.at(i)->getA();
//		aModClass = modReader->findInDescendants(_element,aName);
//		
//		bName = elConn->items.at(i)->getB();
//		bModClass = modReader->findInDescendants(_element,bName);
//		
//
//		if(aModClass)
//		{
//			_ports.push_back(aModClass);
//			_outsideComps.push_back(bModClass);
//		}
//		else
//		{
//			_ports.push_back(bModClass);
//			_outsideComps.push_back(aModClass);
//		}
//	}
//}

void ModelicaConnections::getOutside(ModClass* _element,bool includeChildren,QStringList &ports, QStringList &outsideComps)
{
	QString aName;
	QString bName;
	ModClass* aModClass;
	ModClass* bModClass;

	ModelicaConnections* elConn = filter(_element,includeChildren,false);

	ports.clear();
	outsideComps.clear();

	for(int i=0;i<elConn->items.size();i++)
	{
		aName = elConn->items.at(i)->getA();
		aModClass = modReader->findInDescendants(_element,aName);
		
		bName = elConn->items.at(i)->getB();
		bModClass = modReader->findInDescendants(_element,bName);
		

		if(aModClass && ! bModClass)
		{
			ports.push_back(aName);
			outsideComps.push_back(bName);
		}
		
		if(!aModClass && bModClass)
		{
			ports.push_back(bName);
			outsideComps.push_back(aName);
		}
	}
}

void ModelicaConnections::getOutside(ModClass* _element,bool includeChildren,QStringList &uniquePorts, QList<QStringList> &outsideComps)
{
	QStringList listPorts;
	QStringList listOutsideComps;

	getOutside(_element,includeChildren,listPorts,listOutsideComps);

	QMultiMap<QString,QString> map;
	for(int i=0;i<listPorts.size();i++)
	{
		map.insert(listPorts.at(i),listOutsideComps.at(i));
	}

	uniquePorts = map.uniqueKeys();
	outsideComps.clear();
	for(int i=0;i<uniquePorts.size();i++)
	{
		outsideComps.push_back(map.values(uniquePorts.at(i)));
	}
}

QList<ModClass*> ModelicaConnections::getOutsideConnectedComps(ModClass* _element,bool includeChildren)
{
	ModelicaConnections* elConns= filter(_element,includeChildren,false);
	QList<ModClass*> connectedComps ;

	ModClass *aModClass,*bModClass;
	QString aName,bName;


	// looking for connections from itself
	for(int iConn=0;iConn<elConns->items.size();iConn++)
	{
		aName = elConns->items.at(iConn)->getA();
		bName = elConns->items.at(iConn)->getB();
		aModClass = modReader->findInDescendants(_element,aName);
		bModClass = modReader->findInDescendants(_element,bName);
		
		if(aModClass)
			connectedComps.push_back(bModClass);
		else
			connectedComps.push_back(aModClass);
	}
	return connectedComps;
}

QStringList ModelicaConnections::getOutsideConnectedCompsNames(ModClass* _element,bool includeChildren,Modelica::NameFormat format)
{
	QList<ModClass*> connectedComps = getOutsideConnectedComps(_element,includeChildren);
	QStringList names;
	for(int i=0;i<connectedComps.size();i++)
	{
		names.push_back(connectedComps.at(i)->name(format));
	}
	return names;
}

QList<ModClass*> ModelicaConnections::getOutsideConnectingPorts(ModClass* _element,bool includeChildren)
{
	ModelicaConnections* elConns= filter(_element,includeChildren,false);

	QList<ModClass*> connectedComps ;
	ModClass *aModClass,*bModClass;
	QString aName,bName;
	

	// looking for connections from itself
	for(int iConn=0;iConn<elConns->items.size();iConn++)
	{
		aName = elConns->items.at(iConn)->getA();
		bName = elConns->items.at(iConn)->getB();
		aModClass = modReader->findInDescendants(_element,aName);
		bModClass = modReader->findInDescendants(_element,bName);
		
		if(aModClass)
			connectedComps.push_back(aModClass);
		else
			connectedComps.push_back(bModClass);
	}
	return connectedComps;

}
QStringList ModelicaConnections::getOutsideConnectingPortsNames(ModClass* _element,bool includeChildren,Modelica::NameFormat format)
{
	QList<ModClass*> connectedComps = getOutsideConnectingPorts(_element,includeChildren);
	QStringList names;
	for(int i=0;i<connectedComps.size();i++)
	{
		names.push_back(connectedComps.at(i)->name(format));
	}
	return names;
}



